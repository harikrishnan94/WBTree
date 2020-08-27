#pragma once

#include <type_traits>

namespace wbtree {
// std::visit helper for variant
template <class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
// explicit deduction guide (not needed as of C++20)
template <class... Ts> overloaded(Ts...) -> overloaded<Ts...>;
} // namespace wbtree

// NOLINTNEXTLINE
#define FINALLY(func) auto UniqName(final_action_) = gsl::finally(func)
