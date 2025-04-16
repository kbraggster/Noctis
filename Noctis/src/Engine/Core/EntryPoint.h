#pragma once

#include "Base.h"
#include "Application.h"

extern Noctis::Application* Noctis::CreateApplication();

int main(int argc, char** argv)
{
    Noctis::Log::Init();
    NOC_CORE_INFO("Initialized Log");

    const auto app = Noctis::CreateApplication();
    app->Run();
    delete app;
}