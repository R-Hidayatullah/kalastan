#ifndef GW2DATTOOLS_COMPRESSION_INFLATEBUFFER_H
#define GW2DATTOOLS_COMPRESSION_INFLATEBUFFER_H

#include <cstdint>
#include <string>
#include <stdexcept>

namespace gw2dt
{
    namespace compression
    {

        /** @Inputs:
         *    - iInputTab: Pointer to the buffer to inflate
         *    - iInputSize: Size of the input buffer
         *    - ioOutputSize: if the value is 0 then we decode everything
         *                    else we decode until we reach the io_outputSize
         *  @Outputs:
         *    - ioOutputSize: size of the outputBuffer
         *    - ioErrorMsg: error that occured if it failed
         *  @Return:
         *    - Pointer to the outputBuffer
         *  @Throws:
         *    - gw2dt::exception::Exception in case of error
         */
        uint8_t *inflateBuffer(uint32_t iInputSize, const uint32_t *iInputTab, uint32_t &ioOutputSize, uint8_t *ioOutputTab = nullptr);

    }
}

#endif // GW2DATTOOLS_COMPRESSION_INFLATEBUFFER_H
