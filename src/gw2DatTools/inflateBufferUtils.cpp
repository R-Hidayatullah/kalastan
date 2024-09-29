#include "gw2DatTools/inflateBufferUtils.h"

#include <memory.h>

namespace gw2dt
{
    namespace compression
    {

        // Static HuffmanTreeDict
        HuffmanTree HuffmanTreeDict;

        void readCode(const HuffmanTree &iHuffmanTree, State &ioState, uint16_t &ioCode)
        {
            if (iHuffmanTree.codeCompTab[0] == 0)
            {
                throw std::runtime_error("Trying to read code from an empty HuffmanTree.");
            }

            needBits(ioState, 32);
            uint16_t anIndex = 0;
            while (readBits(ioState, 32) < iHuffmanTree.codeCompTab[anIndex])
            {
                ++anIndex;
            }

            uint8_t aNbBits = iHuffmanTree.codeBitsTab[anIndex];
            ioCode = iHuffmanTree.symbolValueTab[iHuffmanTree.symbolValueTabOffsetTab[anIndex] -
                                                 ((readBits(ioState, 32) - iHuffmanTree.codeCompTab[anIndex]) >> (32 - aNbBits))];
            return dropBits(ioState, aNbBits);
        }

        void buildHuffmanTree(HuffmanTree &ioHuffmanTree, int16_t *ioWorkingBitTab, int16_t *ioWorkingCodeTab)
        {
            // Building the HuffmanTree
            uint32_t aCode = 0;
            uint8_t aNbBits = 0;
            uint16_t aCodeCompTabIndex = 0;
            uint16_t aSymbolOffset = 0;

            while (aNbBits < MaxCodeBitsLength)
            {
                if (ioWorkingBitTab[aNbBits] != -1)
                {
                    int16_t aCurrentSymbol = ioWorkingBitTab[aNbBits];
                    while (aCurrentSymbol != -1)
                    {
                        // Registering the code
                        ioHuffmanTree.symbolValueTab[aSymbolOffset] = aCurrentSymbol;

                        ++aSymbolOffset;
                        aCurrentSymbol = ioWorkingCodeTab[aCurrentSymbol];
                        --aCode;
                    }

                    // Minimum code value for aNbBits bits
                    ioHuffmanTree.codeCompTab[aCodeCompTabIndex] = ((aCode + 1) << (32 - aNbBits));

                    // Number of bits for l_codeCompIndex index
                    ioHuffmanTree.codeBitsTab[aCodeCompTabIndex] = aNbBits;

                    // Offset in symbolValueTab table to reach the value
                    ioHuffmanTree.symbolValueTabOffsetTab[aCodeCompTabIndex] = aSymbolOffset - 1;

                    ++aCodeCompTabIndex;
                }
                aCode = (aCode << 1) + 1;
                ++aNbBits;
            }
        }

        void parseHuffmanTree(State &ioState, HuffmanTree &ioHuffmanTree)
        {
            // Reading the number of symbols to read
            needBits(ioState, 16);
            uint16_t aNumberOfSymbols = static_cast<uint16_t>(readBits(ioState, 16));
            dropBits(ioState, 16);

            if (aNumberOfSymbols > MaxSymbolValue)
            {
                throw std::runtime_error("Too many symbols to decode.");
            }

            int16_t aWorkingBitTab[MaxCodeBitsLength];
            int16_t aWorkingCodeTab[MaxSymbolValue];

            // Initialize our workingTabs
            memset(&aWorkingBitTab, 0xFF, MaxCodeBitsLength * sizeof(int16_t));
            memset(&aWorkingCodeTab, 0xFF, MaxSymbolValue * sizeof(int16_t));

            int16_t aRemainingSymbols = aNumberOfSymbols - 1;

            // Fetching the code repartition
            while (aRemainingSymbols > -1)
            {
                uint16_t aCode = 0;
                readCode(HuffmanTreeDict, ioState, aCode);

                uint16_t aCodeNumberOfBits = aCode & 0x1F;
                uint16_t aCodeNumberOfSymbols = (aCode >> 5) + 1;

                if (aCodeNumberOfBits == 0)
                {
                    aRemainingSymbols -= aCodeNumberOfSymbols;
                }
                else
                {
                    while (aCodeNumberOfSymbols > 0)
                    {
                        if (aWorkingBitTab[aCodeNumberOfBits] == -1)
                        {
                            aWorkingBitTab[aCodeNumberOfBits] = aRemainingSymbols;
                        }
                        else
                        {
                            aWorkingCodeTab[aRemainingSymbols] = aWorkingBitTab[aCodeNumberOfBits];
                            aWorkingBitTab[aCodeNumberOfBits] = aRemainingSymbols;
                        }
                        --aRemainingSymbols;
                        --aCodeNumberOfSymbols;
                    }
                }
            }

            // Effectively build the Huffmanree
            return buildHuffmanTree(ioHuffmanTree, &aWorkingBitTab[0], &aWorkingCodeTab[0]);
        }

        void fillTabsHelper(const uint8_t iBits, const int16_t iSymbol, int16_t *ioWorkingBitTab, int16_t *ioWorkingCodeTab)
        {
            // checking out of bounds
            if (iBits >= MaxCodeBitsLength)
            {
                throw std::runtime_error("Too many bits.");
            }

            if (iSymbol >= MaxSymbolValue)
            {
                throw std::runtime_error("Too high symbol.");
            }

            if (ioWorkingBitTab[iBits] == -1)
            {
                ioWorkingBitTab[iBits] = iSymbol;
            }
            else
            {
                ioWorkingCodeTab[iSymbol] = ioWorkingBitTab[iBits];
                ioWorkingBitTab[iBits] = iSymbol;
            }
        }

        void initializeHuffmanTreeDict()
        {
            int16_t aWorkingBitTab[MaxCodeBitsLength];
            int16_t aWorkingCodeTab[MaxSymbolValue];

            // Initialize our workingTabs
            memset(&aWorkingBitTab, 0xFF, MaxCodeBitsLength * sizeof(int16_t));
            memset(&aWorkingCodeTab, 0xFF, MaxSymbolValue * sizeof(int16_t));

            std::vector<uint8_t> ibits = {
                3,
                3,
                3,
                4,
                4,
                4,
                4,
                5,
                5,
                5,
                5,
                6,
                6,
                6,
                6,
                6,
                6,
                6,
                6,
                7,
                7,
                7,
                7,
                7,
                7,
                7,
                8,
                8,
                8,
                8,
                8,
                8,
                9,
                9,
                9,
                9,
                9,
                9,
                9,
                9,
                9,
                9,
                10,
                10,
                10,
                10,
                10,
                10,
                10,
                10,
                10,
                10,
                10,
                10,
                10,
                10,
                10,
                10,
                11,
                11,
                11,
                11,
                11,
                11,
                11,
                11,
                11,
                11,
                11,
                11,
                11,
                12,
                12,
                12,
                12,
                12,
                12,
                12,
                13,
                13,
                13,
                13,
                13,
                13,
                14,
                14,
                14,
                14,
                15,
                15,
                15,
                15,
                15,
                15,
                15,
                15,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
                16,
            };
            std::vector<uint8_t> isymbols = {
                0x0A,
                0x09,
                0x08,
                0x0C,
                0x0B,
                0x07,
                0x00,
                0xE0,
                0x2A,
                0x29,
                0x06,
                0x4A,
                0x40,
                0x2C,
                0x2B,
                0x28,
                0x20,
                0x05,
                0x04,
                0x49,
                0x48,
                0x27,
                0x26,
                0x25,
                0x0D,
                0x03,
                0x6A,
                0x69,
                0x4C,
                0x4B,
                0x47,
                0x24,
                0xE8,
                0xA0,
                0x89,
                0x88,
                0x68,
                0x67,
                0x63,
                0x60,
                0x46,
                0x23,
                0xE9,
                0xC9,
                0xC0,
                0xA9,
                0xA8,
                0x8A,
                0x87,
                0x80,
                0x66,
                0x65,
                0x45,
                0x44,
                0x43,
                0x2D,
                0x02,
                0x01,
                0xE5,
                0xC8,
                0xAA,
                0xA5,
                0xA4,
                0x8B,
                0x85,
                0x84,
                0x6C,
                0x6B,
                0x64,
                0x4D,
                0x0E,
                0xE7,
                0xCA,
                0xC7,
                0xA7,
                0xA6,
                0x86,
                0x83,
                0xE6,
                0xE4,
                0xC4,
                0x8C,
                0x2E,
                0x22,
                0xEC,
                0xC6,
                0x6D,
                0x4E,
                0xEA,
                0xCC,
                0xAC,
                0xAB,
                0x8D,
                0x11,
                0x10,
                0x0F,
                0xFF,
                0xFE,
                0xFD,
                0xFC,
                0xFB,
                0xFA,
                0xF9,
                0xF8,
                0xF7,
                0xF6,
                0xF5,
                0xF4,
                0xF3,
                0xF2,
                0xF1,
                0xF0,
                0xEF,
                0xEE,
                0xED,
                0xEB,
                0xE3,
                0xE2,
                0xE1,
                0xDF,
                0xDE,
                0xDD,
                0xDC,
                0xDB,
                0xDA,
                0xD9,
                0xD8,
                0xD7,
                0xD6,
                0xD5,
                0xD4,
                0xD3,
                0xD2,
                0xD1,
                0xD0,
                0xCF,
                0xCE,
                0xCD,
                0xCB,
                0xC5,
                0xC3,
                0xC2,
                0xC1,
                0xBF,
                0xBE,
                0xBD,
                0xBC,
                0xBB,
                0xBA,
                0xB9,
                0xB8,
                0xB7,
                0xB6,
                0xB5,
                0xB4,
                0xB3,
                0xB2,
                0xB1,
                0xB0,
                0xAF,
                0xAE,
                0xAD,
                0xA3,
                0xA2,
                0xA1,
                0x9F,
                0x9E,
                0x9D,
                0x9C,
                0x9B,
                0x9A,
                0x99,
                0x98,
                0x97,
                0x96,
                0x95,
                0x94,
                0x93,
                0x92,
                0x91,
                0x90,
                0x8F,
                0x8E,
                0x82,
                0x81,
                0x7F,
                0x7E,
                0x7D,
                0x7C,
                0x7B,
                0x7A,
                0x79,
                0x78,
                0x77,
                0x76,
                0x75,
                0x74,
                0x73,
                0x72,
                0x71,
                0x70,
                0x6F,
                0x6E,
                0x62,
                0x61,
                0x5F,
                0x5E,
                0x5D,
                0x5C,
                0x5B,
                0x5A,
                0x59,
                0x58,
                0x57,
                0x56,
                0x55,
                0x54,
                0x53,
                0x52,
                0x51,
                0x50,
                0x4F,
                0x42,
                0x41,
                0x3F,
                0x3E,
                0x3D,
                0x3C,
                0x3B,
                0x3A,
                0x39,
                0x38,
                0x37,
                0x36,
                0x35,
                0x34,
                0x33,
                0x32,
                0x31,
                0x30,
                0x2F,
                0x21,
                0x1F,
                0x1E,
                0x1D,
                0x1C,
                0x1B,
                0x1A,
                0x19,
                0x18,
                0x17,
                0x16,
                0x15,
                0x14,
                0x13,
                0x12,
            };
            for (size_t i = 0; i < ibits.size(); i++)
            {
                fillTabsHelper(ibits[i], isymbols[i], &aWorkingBitTab[0], &aWorkingCodeTab[0]);
            }

            return buildHuffmanTree(HuffmanTreeDict, &aWorkingBitTab[0], &aWorkingCodeTab[0]);
        }

    }
}
