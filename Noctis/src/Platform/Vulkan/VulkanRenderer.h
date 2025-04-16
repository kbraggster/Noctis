#pragma once

#include "Engine/Renderer/RendererAPI.h"

namespace Noctis
{

class VulkanRenderer : public RendererAPI
{
  public:
    void Init() override;
};

} // namespace Noctis