#pragma once

#include "VulkanDevice.h"

#include <vulkan/vulkan.h>

namespace Noctis
{

class VulkanSwapchain
{
  public:
    VulkanSwapchain(const VulkanDevice& device, const VkSurfaceKHR& surface);
    ~VulkanSwapchain();

    void CreateSwapchain(uint32_t width, uint32_t height);

  private:
    struct SwapchainSupportDetails
    {
        VkSurfaceCapabilitiesKHR Capabilities;
        std::vector<VkSurfaceFormatKHR> Formats;
        std::vector<VkPresentModeKHR> PresentModes;
    };
    SwapchainSupportDetails QuerySwapchainSupport(VkPhysicalDevice device, VkSurfaceKHR surface);
    VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
    VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
    VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, uint32_t width, uint32_t height);

  private:
    const VulkanDevice& m_Device;
    VkSurfaceKHR m_Surface;

    VkSwapchainKHR m_Swapchain = VK_NULL_HANDLE;
    std::vector<VkImage> m_SwapchainImages;
    std::vector<VkImageView> m_SwapchainImageViews;

    VkFormat m_SwapchainImageFormat;
    VkExtent2D m_SwapchainExtent;
};

} // namespace Noctis