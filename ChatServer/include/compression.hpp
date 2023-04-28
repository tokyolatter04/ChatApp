
#pragma once

#include <string>

namespace Compression {
	class GZIP {
	public:
		static bool CompressBytes(std::string bytes, std::string* result);
		static bool DecompressBytes(std::string bytes, std::string* result);
	};
}
