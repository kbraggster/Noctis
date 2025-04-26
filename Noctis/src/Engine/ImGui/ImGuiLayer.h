#pragma once

#include "Engine/Core/Layer.h"
#include "Engine/Core/Window.h"

#include <Platform/Vulkan/VulkanContext.h>

namespace Noctis
{

class ImGuiLayer : public Layer
{
  public:
    ImGuiLayer(Window& window);
    ~ImGuiLayer() override = default;

    void OnAttach() override;
    void OnDetach() override;
    void OnImGuiRender() override;

    void Begin();
    void End();

  private:
    Ref<VulkanContext> m_Context;
    float m_Time = 0.0f;
};

} // namespace Noctis