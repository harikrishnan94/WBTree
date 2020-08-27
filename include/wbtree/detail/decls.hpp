#pragma once

#include <optional>

#include "wbtree/common/inttypes.hpp"
#include "wbtree/common/strong_integer.hpp"

namespace wbtree {
template <typename T> using Option = std::optional<T>;
static constexpr auto None = std::nullopt;

using Oid = Strong<u64, struct OidTag>;
using PageNum = Strong<u64, struct PageNumTag>;
using LogSeqNum = Strong<u64, struct LogSeqNumTag>;
using WALSegNum = Strong<u64, struct WALSegNumTag>;

struct PageID {
  Oid relno;
  PageNum pageno;
};

} // namespace wbtree