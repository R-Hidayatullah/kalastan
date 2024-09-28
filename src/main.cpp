#include "parser/ks_parser.h"
#include "gw2DatTools/inflateBuffer.h"

int main()
{
    try
    {
        KSParser parser("Local.dat");
        KSArchive archive = parser.loadFromFile();

        // You can access the parsed data from 'archive' here
        std::cout << "Parsed " << archive.mft_data.size() << " MFT entries." << std::endl;
        std::cout << "Parsed " << archive.mft_index_data.size() << " MFT index entries." << std::endl;

        // Extract MFT data and save it into a variable
        std::vector<uint8_t> mft_data = parser.extractMFTData(ArchiveId::BaseId, 19);

        // Print the buffer size before decompression
        std::cout << "\nBuffer size before decompression: " << mft_data.size() << " bytes" << std::endl;

        // Print the first 16 bytes of the original MFT data (before decompression) in hexadecimal
        std::cout << "First 16 bytes of original MFT data (Hex): " << std::endl;
        for (size_t i = 0; i < 16 && i < mft_data.size(); ++i)
        {
            std::cout << std::hex << std::uppercase << std::setw(2) << std::setfill('0')
                      << static_cast<int>(mft_data[i]) << " ";
        }
        std::cout << std::endl;

        // Print the first 16 bytes of the original MFT data (ASCII)
        std::cout << "First 16 bytes of original MFT data (ASCII): " << std::endl;
        for (size_t i = 0; i < 16 && i < mft_data.size(); ++i)
        {
            if (std::isprint(mft_data[i]))
            {
                std::cout << static_cast<char>(mft_data[i]);
            }
            else
            {
                std::cout << '.';
            }
        }
        std::cout << std::endl;

        uint32_t output_size = 0;
        uint32_t input_size = static_cast<uint32_t>(mft_data.size());

        // Call the inflateBuffer function
        uint8_t *result = gw2dt::compression::inflateBuffer(reinterpret_cast<uint32_t *>(mft_data.data()), input_size, output_size);

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
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
