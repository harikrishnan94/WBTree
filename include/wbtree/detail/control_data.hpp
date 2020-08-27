#pragma once

#include <array>
#include <filesystem>
#include <string_view>

#include "decls.hpp"

namespace wbtree::detail {
static constexpr auto MAGIC_LEN = 32;
static constexpr std::array<char, MAGIC_LEN> MAGIC = {"WBTREE MAGIC\0"};
static constexpr u64 VERSION = 000'001'000; // MAJOR/MINOR/PATCH
static constexpr std::string_view CONTROL_FILE_NAME = "wbt_control";
static constexpr auto DISK_ATOMIC_IO_SIZE = 512;

class ControlData {
public:
  static constexpr usize WAL_SEGMENT_LEN = 16 * 1024 * 1024;

  explicit ControlData(u64 page_size) : m_page_size(page_size) {}

  [[nodiscard]] constexpr auto PageSize() const { return m_page_size; }
  [[nodiscard]] constexpr auto RedoLSN() const { return m_redo_lsn; }
  [[nodiscard]] constexpr auto NextOid() const { return m_next_oid; }
  [[nodiscard]] constexpr auto CurrentWALSegment() const { return m_cur_wal_seg; }

  static auto Load(std::filesystem::path datadir) -> ControlData;
  void Save(std::filesystem::path datadir) const;

private:
  [[nodiscard]] auto checksum() const -> u32;

  std::array<char, MAGIC_LEN> m_magic = MAGIC;
  u64 m_version = VERSION;
  u64 m_page_size;
  LogSeqNum m_redo_lsn;
  Oid m_next_oid;
  WALSegNum m_cur_wal_seg;
  mutable u32 m_crc;
};

static_assert(std::is_trivially_copyable_v<ControlData>, "control data must be memcpy'able");
static_assert(std::is_standard_layout_v<ControlData>, "control data must be in standard layout");
static_assert(sizeof(ControlData) <= DISK_ATOMIC_IO_SIZE);

} // namespace wbtree::detail