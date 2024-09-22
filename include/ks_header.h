#if !defined(KS_HEADER_H)
#define KS_HEADER_H

#include "kalastan.h"
static const size_t DAT_MAGIC_NUMBER = 3;

#pragma pack(push, 1)
struct KSHeader
{
    uint8_t version;
    uint8_t identifier[DAT_MAGIC_NUMBER];
    uint32_t header_size;
    uint32_t unknown_field;
    uint32_t chunk_size;
    uint32_t crc;
    uint32_t unknown_field_2;
    uint64_t mft_offset;
    uint32_t mft_size;
    uint32_t flags;
};
#pragma pack(pop)

#endif // KS_HEADER_H
