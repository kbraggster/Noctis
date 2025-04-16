#pragma once

#include <memory>

#define NOC_BIND_EVENT_FN(fn)                                                                                          \
    [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

// TODO: Platform specific assertions
#ifdef NOC_ENABLE_ASSERTS
#define NOC_ASSERT(x, ...)                                                                                             \
    {                                                                                                                  \
        if (!(x))                                                                                                      \
        {                                                                                                              \
            NOC_ERROR("Assertion Failed: {0}", __VA_ARGS__);                                                           \
            __builtin_debugtrap();                                                                                     \
        }                                                                                                              \
    }
#define NOC_CORE_ASSERT(x, ...)                                                                                        \
    {                                                                                                                  \
        if (!(x))                                                                                                      \
        {                                                                                                              \
            NOC_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__);                                                      \
            __builtin_debugtrap();                                                                                     \
        }                                                                                                              \
    }
#else
#define NOC_ASSERT(x, ...)
#define NOC_CORE_ASSERT(x, ...)
#endif

#define BIT(x) (1 << x)

namespace Noctis
{

template <typename T> using Scope = std::unique_ptr<T>;
template <typename T, typename... Args> constexpr Scope<T> CreateScope(Args&&... args)
{
    return std::make_unique<T>(std::forward<Args>(args)...);
}

template <typename T> using Ref = std::shared_ptr<T>;
template <typename T, typename... Args> constexpr Ref<T> CreateRef(Args&&... args)
{
    return std::make_shared<T>(std::forward<Args>(args)...);
}

} // namespace Noctis