#pragma once

#include <string>
#include <vector>

//produce a nicely formatted hex dump of some data:
std::string hex_dump(void const *data, size_t size);

//helper for usage on vectors of data:
template< typename T >
std::string hex_dump(std::vector< T > const &data) {
	return hex_dump(data.data(), data.size() * sizeof(T));
}
