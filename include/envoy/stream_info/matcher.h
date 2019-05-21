#pragma once

#include "envoy/common/pure.h"

namespace Envoy {
namespace StreamInfo {

/**
 * TODO(mergeconflict): document me.
 */
class Matcher {
public:
  virtual ~Matcher() = default;

  virtual bool matches() const PURE;
  virtual bool canChange() const PURE;
};

} // namespace StreamInfo
} // namespace Envoy
