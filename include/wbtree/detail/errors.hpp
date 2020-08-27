#pragma once

#include <boost/static_string.hpp>
#include <fmt/format.h>
#include <stdexcept>
#include <string>
#include <string_view>

namespace wbtree::error {
using errno_t = std::decay_t<decltype(errno)>;

namespace detail {
template <typename Error> struct error_base : std::exception {
  template <typename... Args> explicit error_base(Args &&... args) {
    fmt::format_to(std::back_inserter(msg), std::forward<Args>(args)...,
                   fmt::arg("prefix", Error::PREFIX));
  }
  [[nodiscard]] auto what() const noexcept -> const char * override { return msg.c_str(); }

private:
  static constexpr auto EXCEPTION_MSG_LEN = 256;

  boost::static_string<EXCEPTION_MSG_LEN> msg = "";
};
} // namespace detail

struct InvalidConfig : detail::error_base<InvalidConfig> {
  using error_base<InvalidConfig>::error_base;
  static constexpr std::string_view PREFIX = "invalid configuration provided";
};

struct InvalidDataDirectory : detail::error_base<InvalidDataDirectory> {
  using error_base<InvalidDataDirectory>::error_base;
  static constexpr std::string_view PREFIX = "invalid data directory";
};

struct DbInitError : detail::error_base<DbInitError> {
  using error_base<DbInitError>::error_base;
  static constexpr std::string_view PREFIX = "cannot access control file";
};

struct ControlFileAccess : detail::error_base<ControlFileAccess> {
  using error_base<ControlFileAccess>::error_base;
  static constexpr std::string_view PREFIX = "bogus control file";
};

struct ControlFileSanity : detail::error_base<ControlFileSanity> {
  using error_base<ControlFileSanity>::error_base;
  static constexpr std::string_view PREFIX = "{}";
};

struct InvalidValue : detail::error_base<InvalidValue> {
  using error_base<InvalidValue>::error_base;
  static constexpr std::string_view PREFIX = "cannot reconstruct Value from bytes: {0}";
};

struct BlockIO : detail::error_base<BlockIO> {
  using error_base<BlockIO>::error_base;
  static constexpr std::string_view PREFIX = "{}";
};

struct ReadOnly : public std::exception {
public:
  [[nodiscard]] auto what() const noexcept -> const char * override { return PREFIX.data(); }

private:
  static constexpr std::string_view PREFIX = "cannot write to read only database";
};

struct RelCreate : detail::error_base<RelCreate> {
  using error_base<RelCreate>::error_base;
  static constexpr std::string_view PREFIX = "relation already exists";
};

struct RelOpen : detail::error_base<RelOpen> {
  using error_base<RelOpen>::error_base;
  static constexpr std::string_view PREFIX = "cannot open relation ";
};

struct ElementTooBig : detail::error_base<ElementTooBig> {
  using error_base<ElementTooBig>::error_base;
  static constexpr std::string_view PREFIX = "entry too big";
};

struct CorruptPage : detail::error_base<CorruptPage> {
  using error_base<CorruptPage>::error_base;
  static constexpr std::string_view PREFIX = "{}";
};

struct BufferOverflow : public std::exception {
public:
  [[nodiscard]] auto what() const noexcept -> const char * override { return PREFIX.data(); }

private:
  static constexpr std::string_view PREFIX = "nbuffers too small";
};

struct WALFlushFail : detail::error_base<WALFlushFail> {
  using error_base<WALFlushFail>::error_base;
  static constexpr std::string_view PREFIX = "{}";
};

struct WouldBlock : public std::exception {
public:
  [[nodiscard]] auto what() const noexcept -> const char * override { return PREFIX.data(); }

private:
  static constexpr std::string_view PREFIX = "content lock held on the page";
};
} // namespace wbtree::error
