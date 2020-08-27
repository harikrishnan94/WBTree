#pragma once

#include <boost/operators.hpp>
#include <cinttypes>
#include <limits>
#include <type_traits>

namespace wbtree {
template <typename IntT, typename TagT, typename = void> class Strong;

template <typename Type, typename TagT, typename> class Strong {
  static_assert(std::numeric_limits<Type>::is_integer || std::is_same_v<Type, bool>,
                "only integer types and bool type are supported");
};

template <typename IntT, typename TagT>
class Strong<IntT, TagT, std::enable_if_t<std::numeric_limits<IntT>::is_integer>>
    // Use baseclass chaining to maintain standard layout
    : public boost::totally_ordered<
          Strong<IntT, TagT>,
          boost::integer_arithmetic<
              Strong<IntT, TagT>,
              boost::bitwise<Strong<IntT, TagT>,
                             boost::unit_steppable<Strong<IntT, TagT>,
                                                   boost::shiftable<Strong<IntT, TagT>>>>>> {
public:
  using Self = Strong<IntT, TagT>;
  using Int = IntT;
  using Tag = TagT;

  Strong() = default;
  explicit constexpr Strong(Int v) : val(v) {}
  template <typename Result> explicit constexpr operator Result() const {
    return static_cast<Result>(val);
  }

  constexpr auto get() const { return val; }

  template <class CharT, class Traits>
  friend auto operator<<(std::basic_ostream<CharT, Traits> &os, const Strong &i)
      -> std::basic_ostream<CharT, Traits> & {
    return os << i.val;
  }

  template <class CharT, class Traits>
  friend inline auto operator>>(std::basic_istream<CharT, Traits> &is, Strong &i)
      -> std::basic_istream<CharT, Traits> & {
    return is >> i.val;
  }

  constexpr auto operator<(const Strong &x) const -> bool { return val < x.val; }
  constexpr auto operator==(const Strong &x) const -> bool { return val == x.val; }

  constexpr auto operator+=(const Strong &x) -> Strong & {
    val += x.val;
    return *this;
  }
  constexpr auto operator-=(const Strong &x) -> Strong & {
    val -= x.val;
    return *this;
  }
  constexpr auto operator*=(const Strong &x) -> Strong & {
    val *= x.val;
    return *this;
  }
  constexpr auto operator/=(const Strong &x) -> Strong & {
    val /= x.val;
    return *this;
  }
  constexpr auto operator%=(const Strong &x) -> Strong & {
    val %= x.val;
    return *this;
  }
  constexpr auto operator|=(const Strong &x) -> Strong & {
    val |= x.val;
    return *this;
  }
  constexpr auto operator&=(const Strong &x) -> Strong & {
    val &= x.val;
    return *this;
  }
  constexpr auto operator^=(const Strong &x) -> Strong & {
    val ^= x.val;
    return *this;
  }
  constexpr auto operator<<=(const Strong &x) -> Strong & {
    val <<= x.val;
    return *this;
  }
  constexpr auto operator>>=(const Strong &x) -> Strong & {
    val >>= x.val;
    return *this;
  }
  constexpr auto operator++() -> Strong & {
    ++val;
    return *this;
  }
  constexpr auto operator--() -> Strong & {
    --val;
    return *this;
  }

  constexpr auto operator+() const { // unary plus
    return *this;
  }
  constexpr auto operator-() const { // unary minus
    return Strong(0) - *this;
  }

private:
  Int val = 0;
};

template <typename TagT>
class Strong<bool, TagT> : public boost::equality_comparable<Strong<bool, TagT>> {
public:
  using Self = Strong<bool, TagT>;
  using Int = bool;
  using Tag = TagT;

  explicit constexpr Strong() = default;
  explicit constexpr Strong(bool v) : val(v) {}
  explicit constexpr operator bool() const { return val; }

  constexpr auto operator==(const Strong &x) const -> bool { return val == x.val; }
  constexpr auto operator!() const -> bool { return !val; }

private:
  bool val = false;
};
} // namespace wbtree

namespace std {
template <typename Int, typename Tag> struct numeric_limits<wbtree::Strong<Int, Tag>> {
  static constexpr auto is_specialized = true;
  static constexpr auto is_signed = std::numeric_limits<Int>::is_signed;
  static constexpr auto is_integer = std::numeric_limits<Int>::is_integer;
  static constexpr auto is_exact = std::numeric_limits<Int>::is_exact;
  static constexpr auto has_infinity = std::numeric_limits<Int>::has_infinity;
  static constexpr auto has_quiet_NaN = std::numeric_limits<Int>::has_quiet_NaN;
  static constexpr auto has_signaling_NaN = std::numeric_limits<Int>::has_signaling_NaN;
  static constexpr auto has_denorm = std::numeric_limits<Int>::has_denorm;
  static constexpr auto has_denorm_loss = std::numeric_limits<Int>::has_denorm_loss;
  static constexpr auto round_style = std::numeric_limits<Int>::round_style;
  static constexpr auto is_iec559 = std::numeric_limits<Int>::is_iec559;
  static constexpr auto is_bounded = std::numeric_limits<Int>::is_bounded;
  static constexpr auto is_modulo = std::numeric_limits<Int>::is_modulo;
  static constexpr auto digits = std::numeric_limits<Int>::digits;
  static constexpr auto digits10 = std::numeric_limits<Int>::digits10;
  static constexpr auto max_digits10 = std::numeric_limits<Int>::max_digits10;
  static constexpr auto radix = std::numeric_limits<Int>::radix;
  static constexpr auto min_exponent = std::numeric_limits<Int>::min_exponent;
  static constexpr auto min_exponent10 = std::numeric_limits<Int>::min_exponent10;
  static constexpr auto max_exponent = std::numeric_limits<Int>::max_exponent;
  static constexpr auto max_exponent10 = std::numeric_limits<Int>::max_exponent10;
  static constexpr auto traps = std::numeric_limits<Int>::traps;
  static constexpr auto tinyness_before = std::numeric_limits<Int>::tinyness_before;

  static constexpr auto min() { return wbtree::Strong<Int, Tag>(std::numeric_limits<Int>::min()); }
  static constexpr auto lowest() {
    return wbtree::Strong<Int, Tag>(std::numeric_limits<Int>::lowest());
  }
  static constexpr auto max() { return wbtree::Strong<Int, Tag>(std::numeric_limits<Int>::max()); }
  static constexpr auto epsilon() {
    return wbtree::Strong<Int, Tag>(std::numeric_limits<Int>::epsilon());
  }
  static constexpr auto round_error() {
    return wbtree::Strong<Int, Tag>(std::numeric_limits<Int>::round_error());
  }
  static constexpr auto infinity() {
    return wbtree::Strong<Int, Tag>(std::numeric_limits<Int>::infinity());
  }
  static constexpr auto quiet_NaN() {
    return wbtree::Strong<Int, Tag>(std::numeric_limits<Int>::quiet_NaN());
  }
  static constexpr auto signaling_NaN() {
    return wbtree::Strong<Int, Tag>(std::numeric_limits<Int>::signaling_NaN());
  }
  static constexpr auto denorm_min() {
    return wbtree::Strong<Int, Tag>(std::numeric_limits<Int>::denorm_min());
  }
};
} // namespace std