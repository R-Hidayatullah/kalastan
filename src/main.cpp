#include "parser/ks_parser.h"

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
        std::vector<uint8_t> mft_data = parser.extractMFTData(ArchiveId::BaseId, 16);

        // Print the first 16 bytes
        std::cout << "First 16 bytes of MFT data: " << std::endl;
        for (size_t i = 0; i < 16 && i < mft_data.size(); ++i)
        {
            // Print each byte in hexadecimal format
            std::cout << std::hex << std::uppercase << std::setw(2) << std::setfill('0')
                      << static_cast<int>(mft_data[i]) << " ";
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
