#include <crc32c/crc32c.h>
#include <fmt/ostream.h>

#include "wbtree/detail/blockio.hpp"
#include "wbtree/detail/buffer_view.hpp"
#include "wbtree/detail/control_data.hpp"
#include "wbtree/detail/errors.hpp"

using namespace wbtree::blockio;

namespace wbtree::detail {
auto ControlData::Load(std::filesystem::path datadir) -> ControlData {
  datadir /= CONTROL_FILE_NAME;
  auto file = Open(datadir.c_str(), OpenFlags::READ);
  ReadBufferView<ControlData> ctlbytes;
  auto readsize = file.Read<ControlData>(ctlbytes);
  auto &ctldata = ctlbytes.Data();

  if (readsize != ctlbytes.Size()) {
    throw error::ControlFileAccess(R"({prefix}: expected {} bytes, but found only {} bytes)",
                                   sizeof(ControlData), readsize);
  }

  auto crc = ctldata.checksum();
  if (ctldata.checksum() != ctldata.m_crc) {
    throw error::ControlFileSanity(
        R"({prefix}: control data crc mismatch: expected "{}", got "{}")", ctldata.m_crc, crc);
  }

  if (std::string(ctldata.m_magic.data()) != MAGIC.data()) {
    throw error::ControlFileSanity(
        R"({prefix}: control data magic mismatch: expected "{}", got "{}")", MAGIC.data(),
        ctldata.m_magic.data());
  }

  if (ctldata.m_version != VERSION) {
    throw error::ControlFileSanity(
        R"({prefix}: control data version mismatch: expected "{}", got "{}")", VERSION,
        ctldata.m_version);
  }

  return ctldata;
}

void ControlData::Save(std::filesystem::path datadir) const {
  datadir /= CONTROL_FILE_NAME;
  auto file = Open(datadir.c_str(), OpenFlags::READ | OpenFlags::WRITE,
                   CreateMode::USR_WRITE | CreateMode::USR_READ);
  m_crc = checksum();

  if (file.Write<ControlData>({this, 1}) != sizeof(*this))
    throw error::ControlFileAccess("{prefix}");
}

auto ControlData::checksum() const -> u32 {
  return crc32c::Crc32c(reinterpret_cast<const char *>(this), offsetof(ControlData, m_crc));
}

} // namespace wbtree::detail