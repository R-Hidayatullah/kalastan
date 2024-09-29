#include "gw2DatTools/inflateBuffer.h"

#include <memory.h>
#include <stdio.h>
#include "gw2DatTools/inflateBufferUtils.h"

namespace gw2dt
{
    namespace compression
    {

        bool HuffmanTreeDictInitialized(false);

        void inflate_data(State &ioState, uint8_t *ioOutputTab, const uint32_t iOutputSize)
        {
            uint32_t anOutputPos = 0;

            // Reading the const write size addition value
            needBits(ioState, 8);
            dropBits(ioState, 4);
            uint16_t aWriteSizeConstAdd = readBits(ioState, 4) + 1;
            dropBits(ioState, 4);

            // Declaring our HuffmanTrees
            HuffmanTree aHuffmanTreeSymbol;
            HuffmanTree aHuffmanTreeCopy;

            while (anOutputPos < iOutputSize)
            {
                // Resetting Huffmantrees
                memset(&aHuffmanTreeSymbol, 0, sizeof(HuffmanTree));
                memset(&aHuffmanTreeCopy, 0, sizeof(HuffmanTree));

                // Reading HuffmanTrees
                parseHuffmanTree(ioState, aHuffmanTreeSymbol);
                parseHuffmanTree(ioState, aHuffmanTreeCopy);

                // Reading MaxCount
                needBits(ioState, 4);
                uint32_t aMaxCount = (readBits(ioState, 4) + 1) << 12;
                dropBits(ioState, 4);

                uint32_t aCurrentCodeReadCount = 0;

                while ((aCurrentCodeReadCount < aMaxCount) &&
                       (anOutputPos < iOutputSize))
                {
                    ++aCurrentCodeReadCount;

                    // Reading next code
                    uint16_t aCode = 0;
                    readCode(aHuffmanTreeSymbol, ioState, aCode);

                    if (aCode < 0x100)
                    {
                        ioOutputTab[anOutputPos] = static_cast<uint8_t>(aCode);
                        ++anOutputPos;
                        continue;
                    }

                    // We are in copy mode !
                    // Reading the additional info to know the write size
                    aCode -= 0x100;

                    // write size
                    div_t aCodeDiv4 = div(aCode, 4);

                    uint32_t aWriteSize = 0;
                    if (aCodeDiv4.quot == 0)
                    {
                        aWriteSize = aCode;
                    }
                    else if (aCodeDiv4.quot < 7)
                    {
                        aWriteSize = ((1 << (aCodeDiv4.quot - 1)) * (4 + aCodeDiv4.rem));
                    }
                    else if (aCode == 28)
                    {
                        aWriteSize = 0xFF;
                    }
                    else
                    {
                        throw std::runtime_error("Invalid value for writeSize code.");
                    }

                    // additional bits
                    if (aCodeDiv4.quot > 1 && aCode != 28)
                    {
                        uint8_t aWriteSizeAddBits = aCodeDiv4.quot - 1;
                        needBits(ioState, aWriteSizeAddBits);
                        aWriteSize |= readBits(ioState, aWriteSizeAddBits);
                        dropBits(ioState, aWriteSizeAddBits);
                    }
                    aWriteSize += aWriteSizeConstAdd;

                    // write offset
                    // Reading the write offset
                    readCode(aHuffmanTreeCopy, ioState, aCode);

                    div_t aCodeDiv2 = div(aCode, 2);

                    uint32_t aWriteOffset = 0;
                    if (aCodeDiv2.quot == 0)
                    {
                        aWriteOffset = aCode;
                    }
                    else if (aCodeDiv2.quot < 17)
                    {
                        aWriteOffset = ((1 << (aCodeDiv2.quot - 1)) * (2 + aCodeDiv2.rem));
                    }
                    else
                    {
                        throw std::runtime_error("Invalid value for writeOffset code.");
                    }

                    // additional bits
                    if (aCodeDiv2.quot > 1)
                    {
                        uint8_t aWriteOffsetAddBits = aCodeDiv2.quot - 1;
                        needBits(ioState, aWriteOffsetAddBits);
                        aWriteOffset |= readBits(ioState, aWriteOffsetAddBits);
                        dropBits(ioState, aWriteOffsetAddBits);
                    }
                    aWriteOffset += 1;

                    uint32_t anAlreadyWritten = 0;
                    while ((anAlreadyWritten < aWriteSize) &&
                           (anOutputPos < iOutputSize))
                    {
                        ioOutputTab[anOutputPos] = ioOutputTab[anOutputPos - aWriteOffset];
                        ++anOutputPos;
                        ++anAlreadyWritten;
                    }
                }
            }
        }

        uint8_t *inflateBuffer(uint32_t iInputSize, const uint32_t *iInputTab, uint32_t &ioOutputSize, uint8_t *ioOutputTab)
        {
            if (iInputTab == nullptr)
            {
                throw std::runtime_error("Input buffer is null.");
            }

            if (!HuffmanTreeDictInitialized)
            {
                initializeHuffmanTreeDict();
                HuffmanTreeDictInitialized = true;
            }

            try
            {
                // Initialize state
                State aState;
                aState.input = reinterpret_cast<const uint32_t *>(iInputTab);
                aState.inputSize = iInputSize;
                aState.inputPos = 0;

                aState.head = 0;
                aState.bits = 0;
                aState.buffer = 0;

                aState.isEmpty = false;

                // Skipping header & Getting size of the uncompressed data
                needBits(aState, 32);
                dropBits(aState, 32);

                // Getting size of the uncompressed data
                needBits(aState, 32);
                uint32_t anOutputSize = readBits(aState, 32);
                dropBits(aState, 32);

                if (ioOutputSize != 0)
                {
                    // We do not take max here as we won't be able to have more than the output available
                    if (anOutputSize > ioOutputSize)
                    {
                        anOutputSize = ioOutputSize;
                    }
                }

                ioOutputSize = anOutputSize;

                ioOutputTab = static_cast<uint8_t *>(malloc(sizeof(uint8_t) * anOutputSize));

                inflate_data(aState, ioOutputTab, anOutputSize);

                return ioOutputTab;
            }
            catch (std::exception &iException)
            {
                free(ioOutputTab);
                ioOutputTab = nullptr;
                ioOutputSize = 0;

                throw iException; // Rethrow exception
            }
        }

    }
}
