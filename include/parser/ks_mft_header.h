#if !defined(KS_MFT_HEADER_H)
#define KS_MFT_HEADER_H
#include "kalastan.h"

static const size_t MFT_MAGIC_NUMBER = 4;

#pragma pack(push, 1)
struct KSMFTHeader
{
    uint8_t identifier[MFT_MAGIC_NUMBER];
    uint64_t unknown_field;
    uint32_t num_entries;
    uint32_t unknown_field_2;
    uint32_t unknown_field_3;
};
#pragma pack(pop)

#endif // KS_MFT_HEADER_H
