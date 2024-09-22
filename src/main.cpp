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
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
