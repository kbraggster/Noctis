#include "VulkanRenderer.h"

namespace Noctis
{

void VulkanRenderer::Init()
{
    NOC_CORE_INFO("Initializing Vulkan Renderer API");

    m_Context = VulkanContext::Get();

    CreateCommandPool();
    CreateCommandBuffers();
    CreateSyncObjects();

    CreateRenderPass();
    CreateFramebuffers();

    NOC_CORE_INFO("Vulkan Renderer API initialized successfully");
}

void VulkanRenderer::Shutdown()
{
    const auto device = m_Context->GetDevice()->GetVkDevice();

    vkDeviceWaitIdle(device);

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        vkDestroySemaphore(device, m_ImageAvailableSemaphores[i], nullptr);
        vkDestroySemaphore(device, m_RenderFinishedSemaphores[i], nullptr);
        vkDestroyFence(device, m_InFlightFences[i], nullptr);
    }

    vkDestroyCommandPool(device, m_CommandPool, nullptr);
    m_CommandPool = nullptr;
}

void VulkanRenderer::CreateCommandPool()
{
    QueueFamilyIndices indices = m_Context->GetDevice()->GetQueueFamilyIndices();

    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags            = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = indices.GraphicsFamily;

    VK_CHECK_RESULT(vkCreateCommandPool(m_Context->GetDevice()->GetVkDevice(), &poolInfo, nullptr, &m_CommandPool));
}

void VulkanRenderer::CreateCommandBuffers()
{
    m_CommandBuffers.resize(MAX_FRAMES_IN_FLIGHT);

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool        = m_CommandPool;
    allocInfo.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = static_cast<uint32_t>(m_CommandBuffers.size());

    VK_CHECK_RESULT(
        vkAllocateCommandBuffers(m_Context->GetDevice()->GetVkDevice(), &allocInfo, m_CommandBuffers.data()));
}

void VulkanRenderer::CreateSyncObjects()
{
    m_ImageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    m_RenderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    m_InFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    auto device = m_Context->GetDevice()->GetVkDevice();

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        VK_CHECK_RESULT(vkCreateSemaphore(device, &semaphoreInfo, nullptr, &m_ImageAvailableSemaphores[i]));
        VK_CHECK_RESULT(vkCreateSemaphore(device, &semaphoreInfo, nullptr, &m_RenderFinishedSemaphores[i]));
        VK_CHECK_RESULT(vkCreateFence(device, &fenceInfo, nullptr, &m_InFlightFences[i]));
    }
}

void VulkanRenderer::CreateRenderPass()
{
    VkAttachmentDescription attachment{};
    attachment.loadOp        = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attachment.finalLayout   = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    attachment.storeOp       = VK_ATTACHMENT_STORE_OP_STORE;
    attachment.samples       = VK_SAMPLE_COUNT_1_BIT;
    attachment.format        = m_Context->GetSwapchain()->GetVkSwapchainImageFormat();

    VkAttachmentReference colorAttachmentReference{};
    colorAttachmentReference.attachment = 0;
    colorAttachmentReference.layout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments    = &colorAttachmentReference;

    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments    = &attachment;
    renderPassInfo.subpassCount    = 1;
    renderPassInfo.pSubpasses      = &subpass;

    VK_CHECK_RESULT(vkCreateRenderPass(m_Context->GetDevice()->GetVkDevice(), &renderPassInfo, nullptr, &m_RenderPass))
}

void VulkanRenderer::CreateFramebuffers()
{
    auto swapChainImageCount = m_Context->GetSwapchain()->GetVkSwapchainImages().size();
    m_Framebuffers.resize(swapChainImageCount);

    VkFramebufferCreateInfo framebufferInfo{};
    framebufferInfo.sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.width           = m_Context->GetSwapchain()->GetVkSwapchainExtent().width;
    framebufferInfo.height          = m_Context->GetSwapchain()->GetVkSwapchainExtent().height;
    framebufferInfo.renderPass      = m_RenderPass;
    framebufferInfo.layers          = 1;
    framebufferInfo.attachmentCount = 1;

    for (uint32_t i = 0; i < m_Context->GetSwapchain()->GetVkSwapchainImages().size(); i++)
    {
        framebufferInfo.pAttachments = &m_Context->GetSwapchain()->GetVkSwapchainImageViews()[i];

        VK_CHECK_RESULT(
            vkCreateFramebuffer(m_Context->GetDevice()->GetVkDevice(), &framebufferInfo, nullptr, &m_Framebuffers[i]))
    }
}

void VulkanRenderer::BeginFrame()
{
    auto device    = m_Context->GetDevice()->GetVkDevice();
    auto swapChain = m_Context->GetSwapchain()->GetVkSwapchain();

    vkWaitForFences(device, 1, &m_InFlightFences[m_CurrentFrame], VK_TRUE, UINT64_MAX);

    VK_CHECK_RESULT(vkAcquireNextImageKHR(device, swapChain, UINT64_MAX, m_ImageAvailableSemaphores[m_CurrentFrame],
                                          VK_NULL_HANDLE, &m_CurrentImageIndex));

    vkResetFences(device, 1, &m_InFlightFences[m_CurrentFrame]);

    vkResetCommandBuffer(m_CommandBuffers[m_CurrentFrame], 0);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    VK_CHECK_RESULT(vkBeginCommandBuffer(m_CommandBuffers[m_CurrentFrame], &beginInfo));

    // Begin render pass
    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType             = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass        = m_RenderPass;
    renderPassInfo.framebuffer       = m_Framebuffers[m_CurrentImageIndex];
    renderPassInfo.renderArea.extent = m_Context->GetSwapchain()->GetVkSwapchainExtent();

    VkClearValue clearColor{};
    clearColor.color               = {1.0f, 0.0f, 1.0f, 1.0f};
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues    = &clearColor;

    vkCmdBeginRenderPass(m_CommandBuffers[m_CurrentFrame], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
}

void VulkanRenderer::EndFrame()
{
    vkCmdEndRenderPass(m_CommandBuffers[m_CurrentFrame]);

    VK_CHECK_RESULT(vkEndCommandBuffer(m_CommandBuffers[m_CurrentFrame]));

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[]      = {m_ImageAvailableSemaphores[m_CurrentFrame]};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount     = 1;
    submitInfo.pWaitSemaphores        = waitSemaphores;
    submitInfo.pWaitDstStageMask      = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers    = &m_CommandBuffers[m_CurrentFrame];

    VkSemaphore signalSemaphores[]  = {m_RenderFinishedSemaphores[m_CurrentFrame]};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores    = signalSemaphores;

    VK_CHECK_RESULT(
        vkQueueSubmit(m_Context->GetDevice()->GetVkGraphicsQueue(), 1, &submitInfo, m_InFlightFences[m_CurrentFrame]));

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores    = signalSemaphores;

    VkSwapchainKHR swapChains[] = {m_Context->GetSwapchain()->GetVkSwapchain()};
    presentInfo.swapchainCount  = 1;
    presentInfo.pSwapchains     = swapChains;
    presentInfo.pImageIndices   = &m_CurrentImageIndex;

    VK_CHECK_RESULT(vkQueuePresentKHR(m_Context->GetDevice()->GetVkPresentQueue(), &presentInfo));

    m_CurrentFrame = (m_CurrentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

} // namespace Noctis