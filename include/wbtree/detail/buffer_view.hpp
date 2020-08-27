#pragma once

#include <cstddef>
#include <gsl/span>
#include <type_traits>

namespace wbtree::detail {
template <typename T, std::size_t N = 1> class ReadBufferView {
public:
  ReadBufferView() = default;
  ~ReadBufferView() = default;

  ReadBufferView(ReadBufferView &) = delete;
  ReadBufferView(ReadBufferView &&) = delete;
  auto operator=(const ReadBufferView &) -> ReadBufferView & = delete;
  auto operator=(ReadBufferView &&) -> ReadBufferView & = delete;

  [[nodiscard]] constexpr auto Size() const noexcept { return sizeof(T); }
  [[nodiscard]] constexpr auto Bytes() noexcept { return &store.bytes; }
  [[nodiscard]] constexpr auto Data(std::size_t i = 0) noexcept -> T & { return store.val[i]; }

  constexpr operator gsl::span<T>() noexcept { // NOLINT
    return store.val;
  }

private:
  static_assert(std::is_trivially_copyable_v<T>, "T must be trivially copyable");

  union {
    std::byte bytes;
    std::array<T, N> val;
  } store{};
};
} // namespace wbtree::detail