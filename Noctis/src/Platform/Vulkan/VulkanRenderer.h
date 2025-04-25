#pragma once

#include "VulkanContext.h"
#include "Engine/Renderer/RendererAPI.h"

namespace Noctis
{

class VulkanRenderer : public RendererAPI
{
  public:
    void Init() override;
    void Shutdown() override;

    void BeginFrame() override;
    void EndFrame() override;

  private:
    void CreateCommandPool();
    void CreateCommandBuffers();
    void CreateSyncObjects();

  private:
    Ref<VulkanContext> m_Context;
    VkCommandPool m_CommandPool = VK_NULL_HANDLE;
    std::vector<VkCommandBuffer> m_CommandBuffers;

    // Synchronization objects
    std::vector<VkSemaphore> m_ImageAvailableSemaphores;
    std::vector<VkSemaphore> m_RenderFinishedSemaphores;
    std::vector<VkFence> m_InFlightFences;

    static constexpr int MAX_FRAMES_IN_FLIGHT = 2;
    uint32_t m_CurrentFrame                   = 0;
};

} // namespace Noctis