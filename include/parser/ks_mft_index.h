#if !defined(KS_MFT_INDEX_H)
#define KS_MFT_INDEX_H
#include "kalastan.h"
#pragma pack(push, 1)
struct KSMFTIndex
{
    uint32_t file_id;
    uint32_t base_id;
};
#pragma pack(pop)
#endif // KS_MFT_INDEX_H
