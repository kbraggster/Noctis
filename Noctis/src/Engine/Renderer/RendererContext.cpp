#include "RendererContext.h"

#include "Engine/Renderer/RendererAPI.h"

#include "Platform/Vulkan/VulkanContext.h"

namespace Noctis
{

Ref<RendererContext> RendererContext::Create()
{
    switch (RendererAPI::GetAPI())
    {
        case RendererAPI::API::None:
            NOC_CORE_ASSERT(false, "RendererAPI::None is not supported!");
            return nullptr;
        case RendererAPI::API::Vulkan:
            return CreateRef<VulkanContext>();
    }
    NOC_CORE_ASSERT(false, "Unknown RendererAPI!")
    return nullptr;
}

} // namespace Noctis