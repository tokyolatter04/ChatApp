
#include <sstream>

#include "../include/compression.hpp"

#include <ZLIB/zlib.h>

namespace Compression {

    bool GZIP::CompressBytes(std::string bytes, std::string* result) {
        const size_t BUFFER_SIZE = 128 * 1024;
        char buffer[BUFFER_SIZE];

        z_stream stream = {};
        stream.zalloc = Z_NULL;
        stream.zfree = Z_NULL;
        stream.opaque = Z_NULL;

        if (deflateInit2(&stream, Z_DEFAULT_COMPRESSION, Z_DEFLATED, 31, 8, Z_DEFAULT_STRATEGY) != Z_OK) {
            // Failed to initialise ZLIB

            return false;
        }

        std::stringstream compressed_stream;

        stream.avail_in = static_cast<uInt>(bytes.size());
        stream.next_in = reinterpret_cast<Bytef*>(const_cast<char*>(bytes.data()));

        do {
            stream.avail_out = BUFFER_SIZE;
            stream.next_out = reinterpret_cast<Bytef*>(buffer);

            if (deflate(&stream, Z_FINISH) == Z_STREAM_ERROR) {
                // Failed to compress data

                deflateEnd(&stream);
                return false;
            }

            compressed_stream.write(buffer, BUFFER_SIZE - stream.avail_out);
        } while (stream.avail_out == 0);

        deflateEnd(&stream);

        *result = compressed_stream.str();
    }

    bool GZIP::DecompressBytes(std::string bytes, std::string* result) {
        const size_t buffer_size = 32768;
        std::string uncompressed_data;
        int ret;

        z_stream stream;
        std::memset(&stream, 0, sizeof(stream));

        stream.next_in = (Bytef*)reinterpret_cast<const Bytef*>(bytes.data());
        stream.avail_in = static_cast<uInt>(bytes.size());

        if (inflateInit2(&stream, 16 + MAX_WBITS) != Z_OK)
        {
            // Failed to initialise ZLIB

            return false;
        }

        unsigned char out_buffer[buffer_size];

        do
        {
            stream.next_out = out_buffer;
            stream.avail_out = buffer_size;

            if (inflate(&stream, Z_NO_FLUSH) == Z_STREAM_ERROR)
            {
                // Failed to decompress data

                inflateEnd(&stream);
                return false;
            }

            uncompressed_data.append(reinterpret_cast<const char*>(out_buffer), buffer_size - stream.avail_out);

        } while (stream.avail_out == 0);

        inflateEnd(&stream);

        *result = uncompressed_data;
    }
}
