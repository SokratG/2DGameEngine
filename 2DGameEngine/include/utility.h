#pragma once
#include <fstream>
#include <string>
#include <list>
#include <random>
#include <limits>
#include <chrono>

using std::list;
using std::string;
using std::numeric_limits;

namespace GameEngine {
	class Random
	{
	private:
		static std::mt19937 _RandomEngine;
		static std::uniform_int_distribution<std::mt19937::result_type> _Distribution;
	public:
		static void init() {
			_RandomEngine.seed(std::random_device()());
		}
		static float rand() {
			return static_cast<float>(_Distribution(_RandomEngine)) / numeric_limits<uint32_t>::max();
		}
	};



	class CheckTime
	{
	public:
		CheckTime() {
			start = std::chrono::high_resolution_clock::now();
		}
		~CheckTime() {
			end = std::chrono::high_resolution_clock::now();
			auto duration = end - start;
			// duration.count(); // profile time in ns
		}
	private:
		std::chrono::time_point<std::chrono::steady_clock> start, end;
	};

	template<typename T>
	void write_pod(std::ofstream& out, T& t)
	{
		out.write(reinterpret_cast<char*>(&t), sizeof(T));
	}


	template<typename T>
	void read_pod(std::ifstream& in, T& t)
	{
		in.read(reinterpret_cast<char*>(&t), sizeof(T));
	}


	bool startwith(const string& from, const string str);

	list<string> SplitToStringList(const string& str, const char ch = ' ') noexcept;

	bool replace(std::string& str, const std::string& from, const std::string& to) noexcept;

	void replaceAll(std::string& str, const std::string& from, const std::string& to) noexcept;

}