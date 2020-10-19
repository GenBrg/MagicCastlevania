#include "hex_dump.hpp"

std::string hex_dump(void const *data, size_t size) {
	//format of dump will be as per xxd:
	//0000ADDR: xxxx xxxx xxxx xxxx xxxx xxxx xxxx xxxx  asciitextfordump
	constexpr size_t row_bytes = 16;
	static_assert(row_bytes % 2 == 0, "must use even rows");

	auto hex_digit = [](uint8_t val) {
		if (val < 10) return '0' + val;
		else return 'a' + (val - 10);
	};

	std::string ret = "";
	size_t expected_length = ((size + (row_bytes-1))/row_bytes) * (8 + 2 + 5 * (row_bytes / 2) + 2 + row_bytes + 1);
	ret.reserve(expected_length);
	for (size_t ofs = 0; ofs <= size; ofs += row_bytes) {
		{ //"ADDRADDR:"
			size_t addr = ofs;
			for (uint32_t d = 7; d < 8; --d) {
				ret += hex_digit((addr >> (4*d)) % 16);
			}
			ret += ':';
		}
		//" xxxx xxxx" ....
		for (size_t b = 0; b < row_bytes; ++b) {
			if (b % 2 == 0) ret += ' ';
			size_t src = ofs + b;
			if (src < size) {
				uint8_t byte = reinterpret_cast< uint8_t const * >(data)[src];
				ret += hex_digit((byte >> 4) % 16);
				ret += hex_digit(byte % 16);
			} else {
				ret += ' ';
				ret += ' ';
			}
		}
		//'  asciitextfor...'
		ret += ' ';
		ret += ' ';
		for (size_t b = 0; b < row_bytes; ++b) {
			size_t src = ofs + b;
			if (src < size) {
				uint8_t byte = reinterpret_cast< uint8_t const * >(data)[src];
				if (32 <= byte && byte <= 126) {
					ret += static_cast< char >(byte);
				} else {
					ret += '.';
				}
			} else {
				ret += ' ';
			}
		}
		ret += '\n';
	}
	return ret;
}
