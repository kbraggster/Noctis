#pragma once

#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

namespace Noctis
{

class Log
{
  public:
    static void Init();

    static Ref<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
    static Ref<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }

  private:
    static Ref<spdlog::logger> s_CoreLogger;
    static Ref<spdlog::logger> s_ClientLogger;
};

} // namespace Noctis

// Core log macros
#define NOC_CORE_TRACE(...) Noctis::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define NOC_CORE_INFO(...) Noctis::Log::GetCoreLogger()->info(__VA_ARGS__)
#define NOC_CORE_WARN(...) Noctis::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define NOC_CORE_ERROR(...) Noctis::Log::GetCoreLogger()->error(__VA_ARGS__)
#define NOC_CORE_CRITICAL(...) Noctis::Log::GetCoreLogger()->critical(__VA_ARGS__)

// Client log macros
#define NOC_TRACE(...) Noctis::Log::GetClientLogger()->trace(__VA_ARGS__)
#define NOC_INFO(...) Noctis::Log::GetClientLogger()->info(__VA_ARGS__)
#define NOC_WARN(...) Noctis::Log::GetClientLogger()->warn(__VA_ARGS__)
#define NOC_ERROR(...) Noctis::Log::GetClientLogger()->error(__VA_ARGS__)
#define NOC_CRITICAL(...) Noctis::Log::GetClientLogger()->critical(__VA_ARGS__)