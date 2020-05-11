#include <utility.h>
#include <sstream>




namespace GameEngine {
	// init static random class members
	std::mt19937 Random::_RandomEngine;
	std::uniform_int_distribution<std::mt19937::result_type> Random::_Distribution;




	bool startwith(const string& from, const string str) {
		return  (from.rfind(str, 0) == 0); //check start position must be 0
	}


	list<string> SplitToStringList(const string& str, const char ch) noexcept
	{
		list<string> temp;
		std::stringstream ss(str);
		string item;
		while (std::getline(ss, item, ch)) {
			temp.push_back(item);
		}
		return std::move(temp);
	}


	bool replace(std::string& str, const std::string& from, const std::string& to) noexcept {
		size_t start_pos = str.find(from);
		if (start_pos == std::string::npos)
			return false;
		str.replace(start_pos, from.length(), to);
		return true;
	}

	void replaceAll(std::string& str, const std::string& from, const std::string& to) noexcept {
		if (from.empty())
			return;
		size_t start_pos = 0;
		while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
			str.replace(start_pos, from.length(), to);
			start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
		}
	}

}
