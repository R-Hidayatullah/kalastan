#include "parser/ks_parser.h"
#include "gw2DatTools/inflateBuffer.h"

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

std::vector<uint8_t> KSParser::extractMFTData(ArchiveId number_type, uint32_t number)
{
    size_t index_number = 0;
    bool found = false;

    // Search for the appropriate entry based on number_type
    switch (number_type)
    {
    case FileId:
        for (size_t i = 0; i < ks_archive_data.mft_index_data.size(); i++)
        {
            if (ks_archive_data.mft_index_data[i].file_id == number)
            {
                std::cout << "Found!" << std::endl;
                std::cout << "MFT Index Entry " << i << ":" << std::endl;
                std::cout << "File ID: " << ks_archive_data.mft_index_data[i].file_id << std::endl;
                std::cout << "Base ID: " << ks_archive_data.mft_index_data[i].base_id << std::endl;
                index_number = i;
                found = true;
                break;
            }
        }
        break;
    case BaseId:
        for (size_t i = 0; i < ks_archive_data.mft_index_data.size(); i++)
        {
            if (ks_archive_data.mft_index_data[i].base_id == number)
            {
                std::cout << "Found!" << std::endl;
                std::cout << "MFT Index Entry " << i << ":" << std::endl;
                std::cout << "File ID: " << ks_archive_data.mft_index_data[i].file_id << std::endl;
                std::cout << "Base ID: " << ks_archive_data.mft_index_data[i].base_id << std::endl;
                index_number = i;
                found = true;
                break;
            }
        }
        break;
    default:
        throw std::invalid_argument("Invalid number_type provided.");
    }

    // Ensure that the entry was found
    if (!found)
    {
        throw std::runtime_error("MFT entry not found!");
    }

    // Get the MFT data corresponding to the found index
    const KSMFTData &mft_entry = ks_archive_data.mft_data[index_number];

    // Check if the file is compressed
    if (mft_entry.compression_flag != 0)
    {
        std::cout << "File is compressed!" << std::endl;
    }

    // Seek to the file offset and read the data
    fileStream.seekg(mft_entry.offset, std::ios::beg);
    if (!fileStream.good())
    {
        throw std::runtime_error("Failed to seek to file offset!");
    }
    size_t mft_data_size = mft_entry.size;
    std::vector<uint8_t> buffer(mft_data_size);

    std::cout << "Buffer size: " << mft_data_size << std::endl;
    fileStream.read(reinterpret_cast<char *>(buffer.data()), mft_data_size);

    // Check if the read was successful
    if (!fileStream.good())
    {
        throw std::runtime_error("Failed to read data into buffer!");
    }

    // Print the buffer size before decompression
    std::cout << "\nBuffer size before decompression: " << buffer.size() << " bytes" << std::endl;

    // Print the first 16 bytes of the original MFT data (before decompression) in hexadecimal
    std::cout << "First 16 bytes of original MFT data (Hex): " << std::endl;
    for (size_t i = 0; i < 16 && i < buffer.size(); ++i)
    {
        std::cout << std::hex << std::uppercase << std::setw(2) << std::setfill('0')
                  << static_cast<int>(buffer[i]) << " ";
    }
    std::cout << std::endl;

    // Print the first 16 bytes of the original MFT data (ASCII)
    std::cout << "First 16 bytes of original MFT data (ASCII): " << std::endl;
    for (size_t i = 0; i < 16 && i < buffer.size(); ++i)
    {
        if (std::isprint(buffer[i]))
        {
            std::cout << static_cast<char>(buffer[i]);
        }
        else
        {
            std::cout << '.';
        }
    }
    std::cout << std::endl;
    if (mft_entry.compression_flag != 0)
    {

        uint32_t output_size = 0;
        uint32_t input_size = static_cast<uint32_t>(buffer.size());

        // Call the inflateBuffer function
        uint8_t *result = gw2dt::compression::inflateBuffer(input_size, reinterpret_cast<uint32_t *>(buffer.data()), output_size);

        if (result == nullptr)
        {
            throw std::runtime_error("Decompression failed");
        }

        // Print the buffer size after decompression using printf
        printf("\nBuffer size after decompression: %u bytes\n", output_size);

        // Print the first 16 bytes of the decompressed data in hexadecimal
        std::cout << "First 16 bytes of decompressed MFT data (Hex): " << std::endl;
        for (size_t i = 0; i < 16 && i < output_size; ++i)
        {
            std::cout << std::hex << std::uppercase << std::setw(2) << std::setfill('0')
                      << static_cast<int>(result[i]) << " ";
        }
        std::cout << std::endl;

        // Print the first 16 bytes of the decompressed data in ASCII
        std::cout << "First 16 bytes of decompressed MFT data (ASCII): " << std::endl;
        for (size_t i = 0; i < 16 && i < output_size; ++i)
        {
            if (std::isprint(result[i]))
            {
                std::cout << static_cast<char>(result[i]);
            }
            else
            {
                std::cout << '.';
            }
        }
        std::cout << std::endl;
    }

    // Return the buffer containing the file data
    return buffer;
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
    }
}
