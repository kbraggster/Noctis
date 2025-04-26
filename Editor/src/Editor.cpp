#include "Noctis.h"
#include "Engine/Core/EntryPoint.h"

namespace Noctis
{

class ExampleLayer final : public Layer
{
  public:
    ExampleLayer() : Layer("Example") {}

    void OnUpdate() override {}

    void OnEvent(Event& event) override {}
};

class Editor : public Application
{
  public:
    Editor() { PushLayer(new ExampleLayer()); }

    ~Editor() = default;
};

Application* CreateApplication()
{
    return new Editor();
}

} // namespace Noctis