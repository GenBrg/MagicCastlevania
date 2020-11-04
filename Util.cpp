#include "Util.hpp"

#include <iostream>

namespace util
{
	void PrintVec2(const glm::vec2& vec2)
	{
		std::cout << vec2.x << " " << vec2.y << std::endl;
	}

	void PrintMat3(const glm::mat3& mat3)
	{
		for (int i = 0; i < 3; ++i) {
			for (int j = 0; j < 3; ++j) {
				std::cout << mat3[j][i] << " ";
			}
			std::cout << std::endl;
		}
	}
} // namespace util
