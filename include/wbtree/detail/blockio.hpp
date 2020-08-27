#pragma once

#include <gsl/span>
#include <stdexcept>

#include "wbtree/common/inttypes.hpp"
#include "wbtree/common/strong_integer.hpp"
#include "wbtree/detail/errors.hpp"

namespace wbtree::blockio {
namespace OpenFlags {
static constexpr u32 READ = 1;
static constexpr u32 WRITE = 2;
static constexpr u32 APPEND = 4;
static constexpr u32 CREAT = 8;
static constexpr u32 TRUNC = 16;
static constexpr u32 EXCL = 32;
static constexpr u32 SYNC = 64;    // Unix only
static constexpr u32 DIRECT = 128; // Linux only
} // namespace OpenFlags

namespace CreateMode {
static constexpr u32 USR_READ = 1;
static constexpr u32 USR_WRITE = 2;
static constexpr u32 USR_EXEC = 3;
} // namespace CreateMode

enum class Whence : unsigned { SET, CUR, END, LAST };

using fd_t = Strong<int, struct fd_tag>;

static constexpr auto INVALID_FD = std::numeric_limits<fd_t>::min();

struct IOException : std::system_error {
  explicit IOException(error::errno_t errn) : std::system_error(errn, std::generic_category()) {}
};

struct IOMethods {
  virtual ~IOMethods() = default;

  virtual auto Open(std::string_view path, u32 flags, u32 mode) -> fd_t = 0;
  virtual void Close(fd_t fd) = 0;

  [[nodiscard]] virtual auto Seek(fd_t fd, isize off, Whence whence) -> isize = 0;

  [[nodiscard]] virtual auto Write(fd_t fd, const void *buf, usize size) -> isize = 0;
  [[nodiscard]] virtual auto Write(fd_t fd, const void *buf, usize size, isize off) -> isize = 0;
  [[nodiscard]] virtual auto Read(fd_t fd, void *buf, usize size) -> isize = 0;
  [[nodiscard]] virtual auto Read(fd_t fd, void *buf, usize size, isize off) -> isize = 0;

  virtual void Sync(fd_t fd) = 0;
  virtual void DataSync(fd_t fd) = 0;
  virtual void Truncate(fd_t fd, isize off) = 0;
};

#if !defined(__unix__) && !defined(_WIN32)
static_assert(false, "IOMethods are not implemented for this platform");
#endif

struct SystemIO : IOMethods {
  [[nodiscard]] auto Open(std::string_view path, u32 flags, u32 mode) -> fd_t override;
  void Close(fd_t fd) override;

  [[nodiscard]] auto Seek(fd_t fd, isize off, Whence whence) -> isize override;

  [[nodiscard]] auto Write(fd_t fd, const void *buf, usize size) -> isize override;
  [[nodiscard]] auto Write(fd_t fd, const void *buf, usize size, isize off) -> isize override;
  [[nodiscard]] auto Read(fd_t fd, void *buf, usize size) -> isize override;
  [[nodiscard]] auto Read(fd_t fd, void *buf, usize size, isize off) -> isize override;

  void Sync(fd_t fd) override;
  void DataSync(fd_t fd) override;
  void Truncate(fd_t fd, isize off) override;
};

class FileDesc {
public:
  ~FileDesc() {
    try {
      Close();
    } catch (const IOException &e) {
      std::terminate();
    }
  }

  FileDesc(const FileDesc &) = delete;
  FileDesc(FileDesc &&o) noexcept : m_fd(std::exchange(o.m_fd, INVALID_FD)), m_io(o.m_io) {}
  auto operator=(const FileDesc &) -> FileDesc & = delete;
  auto operator=(FileDesc &&o) noexcept -> FileDesc & {
    m_fd = std::exchange(o.m_fd, INVALID_FD);
    m_io = o.m_io;
    return *this;
  }
  explicit operator bool() const { return m_fd != INVALID_FD; }

  void Close() {
    if (*this) {
      m_io.Close(m_fd);
      m_fd = INVALID_FD;
    }
  }
  void Sync() const {
    BOOST_ASSERT(static_cast<bool>(*this) == true);
    return m_io.Sync(m_fd);
  }
  void DataSync() const {
    BOOST_ASSERT(static_cast<bool>(*this) == true);
    return m_io.DataSync(m_fd);
  }

  [[nodiscard]] auto Seek(isize off, Whence whence) const -> isize {
    BOOST_ASSERT(static_cast<bool>(*this) == true);
    return m_io.Seek(m_fd, off, whence);
  }

  void Truncate(isize off) const { return m_io.Truncate(m_fd, off); }

  // Returns num of bytes Written to the file
  template <typename T, typename = std::enable_if_t<std::is_trivially_copyable_v<T>>>
  auto Write(gsl::span<const T> data) const -> isize {
    return Write(reinterpret_cast<const void *>(data.data()), data.size_bytes());
  }
  template <typename T, typename = std::enable_if_t<std::is_trivially_copyable_v<T>>>
  auto Write(gsl::span<const T> data, isize off) const -> isize {
    return Write(reinterpret_cast<const void *>(data.data()), data.size_bytes(), off);
  }

  // Returns num of bytes Read from the file
  template <typename T, typename = std::enable_if_t<std::is_trivially_copyable_v<T>>>
  auto Read(gsl::span<T> data) const -> isize {
    return Read(reinterpret_cast<void *>(data.data()), data.size_bytes());
  }
  template <typename T, typename = std::enable_if_t<std::is_trivially_copyable_v<T>>>
  auto Read(gsl::span<T> data, isize off) const -> isize {
    return Read(reinterpret_cast<void *>(data.data()), data.size_bytes(), off);
  }

  [[nodiscard]] auto Write(const void *buf, usize size) const -> isize {
    BOOST_ASSERT(static_cast<bool>(*this) == true);
    return m_io.Write(m_fd, buf, size);
  }
  [[nodiscard]] auto Write(const void *buf, usize size, isize off) const -> isize {
    BOOST_ASSERT(static_cast<bool>(*this) == true);
    return m_io.Write(m_fd, buf, size, off);
  }
  [[nodiscard]] auto Read(void *buf, usize size) const -> isize {
    BOOST_ASSERT(static_cast<bool>(*this) == true);
    return m_io.Read(m_fd, buf, size);
  }
  [[nodiscard]] auto Read(void *buf, usize size, isize off) const -> isize {
    BOOST_ASSERT(static_cast<bool>(*this) == true);
    return m_io.Read(m_fd, buf, size, off);
  }

private:
  explicit FileDesc(fd_t fd, IOMethods &io) : m_fd(fd), m_io(io) {}

  friend auto OpenWith(IOMethods &io, std::string_view path, u32 flags, u32 mode) -> FileDesc;

  fd_t m_fd = INVALID_FD;
  IOMethods &m_io;
};

inline auto OpenWith(IOMethods &io, std::string_view path, u32 flags, u32 mode = 0) -> FileDesc {
  return FileDesc(io.Open(path, flags, mode), io);
}

inline auto Open(std::string_view path, u32 flags, u32 mode = 0) -> FileDesc {
  SystemIO io;
  return OpenWith(io, path, flags, mode);
}

inline auto Tell(const FileDesc &fd) { return fd.Seek(0, Whence::CUR); }

} // namespace wbtree::blockio