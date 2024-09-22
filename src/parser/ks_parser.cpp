#include "parser/ks_parser.h"

KSParser::KSParser(const std::string &filePath)
{
    // Check file extension
    if (filePath.substr(filePath.find_last_of(".") + 1) != "dat")
    {
        throw std::runtime_error("Invalid file extension. Expected '.dat'.");
    }

    fileStream.open(filePath, std::ios::binary);
    if (!fileStream.is_open())
    {
        throw std::runtime_error("Failed to open file: " + filePath);
    }
}

KSArchive KSParser::loadFromFile()
{
    readHeader();
    readMFTHeader();
    readMFTData();
    readMFTIndex();

    return ks_archive_data;
}

void KSParser::readHeader()
{
    fileStream.read(reinterpret_cast<char *>(&ks_archive_data.dat_header), sizeof(KSHeader));
    if (!fileStream.good())
    {
        throw std::runtime_error("Failed to read DAT header.");
    }

    // Check magic number
    const uint8_t check_magic[3] = {0x41, 0x4E, 0x1A}; // Example magic
    if (std::memcmp(ks_archive_data.dat_header.identifier, check_magic, sizeof(check_magic)) != 0)
    {
        throw std::runtime_error("Invalid header magic.");
    }

    // Print header information
    std::cout << "DAT Header:" << std::endl;
    std::cout << "Version: " << static_cast<int>(ks_archive_data.dat_header.version) << std::endl;
    std::cout << "Identifier: ";
    for (const auto &id : ks_archive_data.dat_header.identifier)
    {
        std::cout << std::hex << static_cast<int>(id) << " ";
    }
    std::cout << std::dec << std::endl;
    std::cout << "Header Size: " << ks_archive_data.dat_header.header_size << std::endl;
    std::cout << "MFT Offset: " << ks_archive_data.dat_header.mft_offset << std::endl;
    std::cout << "MFT Size: " << ks_archive_data.dat_header.mft_size << std::endl;
}

void KSParser::readMFTHeader()
{
    fileStream.seekg(ks_archive_data.dat_header.mft_offset, std::ios::beg);
    fileStream.read(reinterpret_cast<char *>(&ks_archive_data.mft_header), sizeof(KSMFTHeader));
    if (!fileStream.good())
    {
        throw std::runtime_error("Failed to read MFT header.");
    }

    // Print MFT header information
    std::cout << "MFT Header:" << std::endl;
    std::cout << "Identifier: ";
    for (const auto &id : ks_archive_data.mft_header.identifier)
    {
        std::cout << std::hex << static_cast<int>(id) << " ";
    }
    std::cout << std::dec << std::endl;
    std::cout << "Number of Entries: " << ks_archive_data.mft_header.num_entries << std::endl;
}

void KSParser::readMFTData()
{
    ks_archive_data.mft_data.resize(ks_archive_data.mft_header.num_entries);
    for (size_t i = 0; i < ks_archive_data.mft_data.size(); ++i)
    {
        fileStream.read(reinterpret_cast<char *>(&ks_archive_data.mft_data[i]), sizeof(KSMFTData));
        if (!fileStream.good())
        {
            throw std::runtime_error("Failed to read MFT data.");
        }

        // Print MFT data information
        // std::cout << "MFT Data Entry " << i << ":" << std::endl;
        // std::cout << "Offset: " << ks_archive_data.mft_data[i].offset << std::endl;
        // std::cout << "Size: " << ks_archive_data.mft_data[i].size << std::endl;
        // std::cout << "Compression Flag: " << ks_archive_data.mft_data[i].compression_flag << std::endl;
        // std::cout << "Entry Flag: " << ks_archive_data.mft_data[i].entry_flag << std::endl;
        // std::cout << "Counter: " << ks_archive_data.mft_data[i].counter << std::endl;
        // std::cout << "CRC: " << ks_archive_data.mft_data[i].crc << std::endl;
    }
}

void KSParser::readMFTIndex()
{
    size_t num_file_id_entries = ks_archive_data.mft_data[MFT_ENTRY_INDEX_NUM].size / sizeof(KSMFTIndex); // Assuming the first entry is for the index
    ks_archive_data.mft_index_data.resize(num_file_id_entries);

    fileStream.seekg(ks_archive_data.mft_data[MFT_ENTRY_INDEX_NUM].offset, std::ios::beg);
    for (size_t i = 0; i < ks_archive_data.mft_index_data.size(); ++i)
    {
        fileStream.read(reinterpret_cast<char *>(&ks_archive_data.mft_index_data[i]), sizeof(KSMFTIndex));
        if (!fileStream.good())
        {
            throw std::runtime_error("Failed to read MFT index.");
        }
        // size_t find_file = 970673;
        // if (ks_archive_data.mft_index_data[i].file_id == find_file || ks_archive_data.mft_index_data[i].base_id == find_file)
        // {
        //     // Print MFT index information
        //     std::cout << "Found!" << std::endl;
        //     std::cout << "MFT Index Entry " << i << ":" << std::endl;
        //     std::cout << "File ID: " << ks_archive_data.mft_index_data[i].file_id << std::endl;
        //     std::cout << "Base ID: " << ks_archive_data.mft_index_data[i].base_id << std::endl;
        // }
    }
}
