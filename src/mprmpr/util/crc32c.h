#ifndef TENSORFLOW_LIB_HASH_CRC32C_H_
#define TENSORFLOW_LIB_HASH_CRC32C_H_

#include <stddef.h>
#include "mprmpr/base/port.h"

namespace mprmpr {
namespace crc32c {

// Return the crc32c of concat(A, data[0,n-1]) where init_crc is the
// crc32c of some string A.  Extend() is often used to maintain the
// crc32c of a stream of data.
extern uint32 Extend(uint32 init_crc, const char* data, size_t n);

// Return the crc32c of data[0,n-1]
inline uint32 Value(const char* data, size_t n) { return Extend(0, data, n); }
inline uint32 Crc32c(const char* data, size_t n) { return Value(data, n); }

static const uint32 kMaskDelta = 0xa282ead8ul;

// Return a masked representation of crc.
//
// Motivation: it is problematic to compute the CRC of a string that
// contains embedded CRCs.  Therefore we recommend that CRCs stored
// somewhere (e.g., in files) should be masked before being stored.
inline uint32 Mask(uint32 crc) {
  // Rotate right by 15 bits and add a constant.
  return ((crc >> 15) | (crc << 17)) + kMaskDelta;
}

// Return the crc whose masked representation is masked_crc.
inline uint32 Unmask(uint32 masked_crc) {
  uint32 rot = masked_crc - kMaskDelta;
  return ((rot >> 17) | (rot << 15));
}

}  // namespace crc32c
}  // namespace mprmpr

#endif  // TENSORFLOW_LIB_HASH_CRC32C_H_
