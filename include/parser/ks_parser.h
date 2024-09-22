#ifndef KS_PARSER_H
#define KS_PARSER_H

#include "kalastan.h"
#include "parser/ks_header.h"
#include "parser/ks_mft_header.h"
#include "parser/ks_mft_data.h"
#include "parser/ks_mft_index.h"
static const size_t MFT_ENTRY_INDEX_NUM = 1;

#pragma pack(push, 1)
struct KSArchive
{
    KSHeader dat_header;
    KSMFTHeader mft_header;
    std::vector<KSMFTData> mft_data;
    std::vector<KSMFTIndex> mft_index_data;
};
#pragma pack(pop)

class KSParser
{
public:
    KSParser(const std::string &filePath);
    ~KSParser() = default;

    KSArchive loadFromFile();

private:
    void readHeader();
    void readMFTHeader();
    void readMFTData();
    void readMFTIndex();

    std::ifstream fileStream;
    KSArchive ks_archive_data;
};

#endif // KS_PARSER_H
