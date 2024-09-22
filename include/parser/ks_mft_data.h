#if !defined(KS_MFT_DATA_H)
#define KS_MFT_DATA_H
#include "kalastan.h"
#pragma pack(push, 1)
struct KSMFTData
{
    uint64_t offset;
    uint32_t size;
    uint16_t compression_flag;
    uint16_t entry_flag;
    uint32_t counter;
    uint32_t crc;
};
#pragma pack(pop)

#endif // KS_MFT_DATA_H
