#pragma once

#include "inttypes.hpp"

#include <climits>
#include <cstddef>
#include <functional>

namespace wbtree {
template <typename Int> class Bits {
public:
  template <typename... Bits> static constexpr auto NumWords(usize num_bits) -> usize {
    return num_bits / sizeof(Int) + (num_bits % sizeof(Int) ? 1 : 0);
  }

  template <typename... Bits> static constexpr auto Set(Int w, Bits... bits) -> Int {
    return w | get_mask<FORWARD>(bits...);
  }

  // Indexes bits in reverse order
  template <typename... Bits> static constexpr auto SetReverse(Int w, Bits... bits) -> Int {
    return w | get_mask<REVERSE>(bits...);
  }

  template <typename... Bits> static constexpr auto Clear(Int w, Bits... bits) -> Int {
    return w & ~get_mask<FORWARD>(bits...);
  }

  template <typename... Bits> static constexpr auto ClearReverse(Int w, Bits... bits) -> Int {
    return w & ~get_mask<REVERSE>(bits...);
  }

  template <typename... Bits> static constexpr auto Get(Int w, Bits... bits) -> Int {
    return w & get_mask<FORWARD>(bits...);
  }

  template <typename... Bits> static constexpr auto GetReverse(Int w, Bits... bits) -> Int {
    return w & get_mask<REVERSE>(bits...);
  }

  template <typename... Bits> static constexpr auto IsAllSet(Int w, Bits... bits) -> bool {
    auto mask = get_mask<FORWARD>(bits...);
    return (w & mask) == mask;
  }

  template <typename... Bits> static constexpr auto IsAllSetReverse(Int w, Bits... bits) -> bool {
    auto mask = get_mask<REVERSE>(bits...);
    return (w & mask) == mask;
  }

  template <typename... Bits> static constexpr auto IsAnySet(Int w, Bits... bits) -> bool {
    return Get(w, bits...) != 0;
  }

  template <typename... Bits> static constexpr auto IsAnySetReverse(Int w, Bits... bits) -> bool {
    return Get<REVERSE>(w, bits...) != 0;
  }

  template <typename Oper, typename... Bits>
  static constexpr auto MaskedOp(Int w, Oper &&op, Bits... bits) -> Int {
    auto mask = get_mask<FORWARD>(bits...);
    auto masked_bits = w & mask;

    w &= ~mask;

    return std::invoke(std::forward<Oper>(op), w) | masked_bits;
  }

private:
  template <int Direction, typename... Bits> static constexpr auto get_mask(Bits... bits) -> Int {
    return (... | (Direction == FORWARD ? (ONE << bits) : (ONE << (NUMBITS - bits - 1))));
  }

  static constexpr Int ONE{1};
  static constexpr int NUMBITS = sizeof(Int) * CHAR_BIT;
  enum { FORWARD, REVERSE };
};

} // namespace wbtree