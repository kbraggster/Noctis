#pragma once

#include "RendererContext.h"
#include "Engine/Renderer/RendererAPI.h"

namespace Noctis
{

class Application;
class Renderer
{
  public:
    static void Init();
    static void Shutdown();

    static void Render();

    static Ref<RendererContext> GetContext();
    static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }
};

} // namespace Noctis