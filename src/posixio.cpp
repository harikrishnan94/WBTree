#ifdef __unix__
#include <boost/config.hpp>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#include "wbtree/detail/blockio.hpp"

namespace wbtree::blockio {
auto SystemIO::Open(std::string_view path, u32 flags, u32 mode) -> fd_t {
  u32 os_flags = 0;
  u32 os_mode = 0;
  bool is_create = false;

  using namespace OpenFlags;
  using namespace CreateMode;

  if ((flags & SYNC) != 0)
    os_flags |= u32(O_SYNC);

  if ((flags & READ & WRITE) != 0)
    os_flags |= u32(O_RDWR);
  if ((flags & READ) != 0)
    os_flags |= u32(O_RDONLY);
  if ((flags & WRITE) != 0)
    os_flags |= u32(O_WRONLY);
  if ((flags & APPEND) != 0)
    os_flags |= u32(O_APPEND);
  if ((flags & CREAT) != 0) {
    os_flags |= u32(O_CREAT);
    is_create = true;
  }
  if ((flags & TRUNC) != 0)
    os_flags |= u32(O_TRUNC);
  if ((flags & EXCL) != 0)
    os_flags |= u32(O_EXCL);
  if ((flags & SYNC) != 0)
    os_flags |= u32(O_SYNC);
#ifdef __APPLE__
  if ((flags & DIRECT) != 0) {
    errno = EINVAL;
    throw IOException(errno);
  }
#else
  if ((flags & DIRECT) != 0) {
    os_flags |= u32(O_DIRECT);
  }
#endif

  if ((mode & USR_READ) != 0)
    os_mode |= u32(S_IRUSR);
  if ((mode & USR_WRITE) != 0)
    os_mode |= u32(S_IWUSR);
  if ((mode & USR_EXEC) != 0)
    os_mode |= u32(S_IXUSR);

  auto fd = [&] {
    if (is_create)
      return open(path.data(), os_flags, os_mode); // NOLINT
    return open(path.data(), os_flags);            // NOLINT
  }();
  if (fd == -1)
    throw IOException(errno);
  return fd_t(fd);
}

void SystemIO::Close(fd_t fd) {
  BOOST_ASSERT(fd != INVALID_FD);
  if (::close(fd.get()) != 0)
    throw IOException(errno);
}

auto SystemIO::Write(fd_t fd, const void *buf, usize size) -> isize {
  BOOST_ASSERT(fd != INVALID_FD);
  auto writsize = ::write(fd.get(), buf, size);

  if (writsize == -1)
    throw IOException(errno);
  return writsize;
}

auto SystemIO::Write(fd_t fd, const void *buf, usize size, isize off) -> isize {
  BOOST_ASSERT(fd != INVALID_FD);
  auto writsize = pwrite(fd.get(), buf, size, off);

  if (writsize == -1)
    throw IOException(errno);
  return writsize;
}

auto SystemIO::Read(fd_t fd, void *buf, usize size) -> isize {
  BOOST_ASSERT(fd != INVALID_FD);
  auto readsize = ::read(fd.get(), buf, size);

  if (readsize == -1)
    throw IOException(errno);
  return readsize;
}

auto SystemIO::Read(fd_t fd, void *buf, usize size, isize off) -> isize {
  BOOST_ASSERT(fd != INVALID_FD);
  auto readsize = ::pread(fd.get(), buf, size, off);

  if (readsize == -1)
    throw IOException(errno);
  return readsize;
}

void SystemIO::Sync(fd_t fd) {
  if (fsync(fd.get()) != 0)
    throw IOException(errno);
}

void SystemIO::DataSync(fd_t fd) {
#ifdef __linux__
  if (fdatasync(fd.get()) != 0)
    throw IOException(errno);
#else
  return Sync(fd);
#endif
}

auto SystemIO::Seek(fd_t fd, isize off, Whence whence) -> isize {
  if (whence >= Whence::LAST)
    throw IOException(errno);

  int os_whence = [whence] {
    switch (whence) {
    case Whence::CUR:
      return SEEK_CUR;
    case Whence::SET:
      return SEEK_SET;
    case Whence::END:
      return SEEK_END;
    default:
      BOOST_UNREACHABLE_RETURN();
    }
  }();

  auto res = lseek(fd.get(), off, os_whence);
  if (res == -1)
    throw IOException(errno);
  return res;
}

void SystemIO::Truncate(fd_t fd, isize off) {
  if (ftruncate(fd.get(), off) != 0)
    throw IOException(errno);
}
} // namespace wbtree::blockio
#endif
