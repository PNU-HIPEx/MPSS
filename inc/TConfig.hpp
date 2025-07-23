
/**
 * @file TConfig.hpp
 * @author Yongjun Choi (chldydwns2014@gmail.com)
 * @brief
 * @version 1.0
 * @date 17-05-2025
 *
 * @copyright Copyright (c) 2025
 *
*/

#ifndef __TCONFIG_H__
#define __TCONFIG_H__

#include<filesystem>
#include<string>
#include<vector>
#include<fstream>
#include<iostream>
#include<array>
#include<sstream>
#include<algorithm>

namespace KEI {
	// This is a part of KEI name space.

	class TDictionary {
	public:
		TDictionary(std::string_view key, std::string_view value) : mKey(key), mValue(value) { }
	private:
		std::string mKey;
		std::string mValue;
	public:
		std::string getKey() const { return mKey; }
		void setValue(const std::string& value) { mValue = value; }
		// This is 
		template <typename T> static constexpr bool isInterger8() {
			return std::is_same_v<T, int8_t> || std::is_same_v<T, signed char>;
		}
		template <typename T> static constexpr bool isInteger16() {
			return std::is_same_v<T, int16_t> || std::is_same_v<T, short> || std::is_same_v<T, short int> || std::is_same_v<T, signed short> || std::is_same_v<T, signed short int>;
		}
		template <typename T> static constexpr bool isInteger32() {
			return std::is_same_v<T, int> || std::is_same_v<T, signed> || std::is_same_v < T, signed int> || std::is_same_v<T, int32_t> || std::is_same_v<T, long> || std::is_same_v<T, signed long> || std::is_same_v<T, signed long int>;
		}
		template <typename T> static constexpr bool isInteger64() {
			return std::is_same_v<T, long long> || std::is_same_v<T, signed long long> || std::is_same_v<T, signed long long int> || std::is_same_v<T, int64_t>;
		}
		template <typename T> static constexpr bool isUnsignedInteger8() {
			return std::is_same_v<T, uint8_t> || std::is_same_v<T, unsigned char>;
		}
		template <typename T> static constexpr bool isUnsignedInteger16() {
			return std::is_same_v<T, uint16_t> || std::is_same_v<T, unsigned short> || std::is_same_v<T, unsigned short int>;
		}
		template <typename T> static constexpr bool isUnsignedInteger32() {
			return std::is_same_v<T, unsigned int> || std::is_same_v<T, unsigned> || std::is_same_v<T, uint32_t> || std::is_same_v<T, unsigned long> || std::is_same_v<T, unsigned long int>;
		}
		template <typename T> static constexpr bool isUnsignedInteger64() {
			return std::is_same_v<T, unsigned long long> || std::is_same_v<T, unsigned long long int> || std::is_same_v<T, uint64_t>;
		}

		template <typename T> T getValue() const {
			if constexpr ( isInteger32<T>() ) {
				return std::stoi(mValue);
			} else if constexpr ( isUnsignedInteger32<T>() ) {
				return static_cast<unsigned int>(std::stoul(mValue));
			} else if constexpr ( isInterger8<T>() ) {
				return static_cast<int8_t>(mValue.at(0));
			} else if constexpr ( isUnsignedInteger8<T>() ) {
				return static_cast<uint8_t>(mValue.at(0));
			} else if constexpr ( isInteger16<T>() ) {
				return static_cast<int16_t>(std::stoi(mValue));
			} else if constexpr ( isUnsignedInteger16<T>() ) {
				return static_cast<uint16_t>(std::stoul(mValue));
			} else if constexpr ( isInteger64<T>() ) {
				return static_cast<int64_t>(std::stoll(mValue));
			} else if constexpr ( isUnsignedInteger64<T>() ) {
				return static_cast<uint64_t>(std::stoull(mValue));
			} else if constexpr ( std::is_same_v<T, bool> ) {
				if ( mValue == "true" || mValue == "1" || mValue == "True" || mValue == "TRUE" ) {
					return true;
				} else if ( mValue == "false" || mValue == "0" || mValue == "False" || mValue == "FALSE" ) {
					return false;
				}
			} else if constexpr ( std::is_same_v<T, char> ) {
				return mValue.at(0);
			} else if constexpr ( std::is_same_v<T, float> ) {
				return static_cast<float>(std::stof(mValue));
			} else if constexpr ( std::is_same_v<T, double> ) {
				return static_cast<double>(std::stod(mValue));
			} else if constexpr ( std::is_same_v<T, long double> ) {
				return static_cast<long double>(std::stold(mValue));
			} else if constexpr ( std::is_same_v<T, std::string> ) {
				return mValue;
			} else {
				static_assert(!std::is_same_v<T, T>, "Unsupported type");
			}
			return T();
		}

		template<typename T, int N> std::array<T, N> getValue() const {
			static_assert(N > 0, "N must be greater than 0");
			std::array<T, N> arr;
			std::istringstream valueList(mValue);
			std::string value;
			int iValue = 0;
			while ( valueList >> value, ' ' ) {
				if constexpr ( std::is_same_v<T, int> ) {
					arr[iValue] = std::stoi(value);
				} else if constexpr ( std::is_same_v<T, unsigned int> ) {
					arr[iValue] = static_cast<unsigned int>(std::stoul(value));
				} else if constexpr ( std::is_same_v<T, float> ) {
					arr[iValue] = static_cast<float>(std::stof(value));
				} else if constexpr ( std::is_same_v<T, double> ) {
					arr[iValue] = static_cast<double>(std::stod(value));
				} else if constexpr ( std::is_same_v<T, long double> ) {
					arr[iValue] = static_cast<long double>(std::stold(value));
				} else if constexpr ( std::is_same_v<T, char> ) {
					arr[iValue] = value.at(0);
				} else if constexpr ( std::is_same_v<T, std::string> ) {
					arr[iValue] = value;
				} else {
					static_assert(!std::is_same_v<T, T>, "Unsupported type");
				}
				iValue++;
				if ( iValue == N ) {
					break;
				}
			}
			if ( arr.size() != N ) {
				throw std::runtime_error("Array size mismatch");
			}
			return arr;
		}
	};

	class TConfig {
	public:
		TConfig(std::string_view title) : mTitle(title) { }
	private:
		std::string mTitle;
		std::vector<TConfig> mConfigSet;
		std::vector<TDictionary> mDictSet;
	public:
		void addConfig(const TConfig& config) {
			mConfigSet.push_back(config);
		}
		void addDictionary(const TDictionary& dict) {
			bool isExist = false;
			for ( auto& dic : mDictSet ) {
				if ( dic.getKey() == dict.getKey() ) {
					dic.setValue(dict.getValue<std::string>());
					isExist = true;
				}
			}
			if ( !isExist ) {
				mDictSet.push_back(dict);
			}
		}

		std::string getTitle() const { return mTitle; }
		template<typename T>
		T getValue(std::string_view key) const {
			for ( const auto& dict : mDictSet ) {
				if ( dict.getKey() == key ) {
					return dict.getValue<T>();
				}
			}
			throw std::runtime_error("Key not found");
		}
		template<typename T, int N>
		std::array<T, N> getValue(std::string_view key) const {
			for ( const auto& dict : mDictSet ) {
				if ( dict.getKey() == key ) {
					return dict.getValue<T, N>();
				}
			}
			throw std::runtime_error("Key not found");
		}
		bool hasKey(std::string_view key) const {
			for ( const auto& dict : mDictSet ) {
				if ( dict.getKey() == key ) {
					return true;
				}
			}
			return false;
		}
		TConfig getConfig(std::string_view title) const {
			for ( const auto& config : mConfigSet ) {
				if ( config.mTitle == title ) {
					return config;
				}
			}
			throw std::runtime_error("Config not found");
		}
		TDictionary getDictionary(std::string_view key) const {
			for ( const auto& dict : mDictSet ) {
				if ( dict.getKey() == key ) {
					return dict;
				}
			}
			throw std::runtime_error("Dictionary not found");
		}
		TConfig& modifyConfig(std::string_view title) {
			for ( auto& config : mConfigSet ) {
				if ( config.mTitle == title ) {
					return config;
				}
			}
			throw std::runtime_error("Config not found");
		}
	};

	class TConfigFile {
	private:
		std::filesystem::path mPath;

		std::vector<TConfig> mConfigSet;
	public:
		TConfigFile() = default;

		TConfigFile(std::filesystem::path path) {
			try {
				if ( !std::filesystem::exists(path) ) {
					throw std::runtime_error("File does not exist");
				} else if ( path.extension() != ".conf" ) {
					throw std::runtime_error("File is not a configuration file");
				} else {
					mPath = path;
					read();
				}
			} catch ( const std::exception& e ) {
				std::cerr << "Error: " << e.what() << std::endl;
				// Handle the error
			}
		}

		bool hasConfig(std::string_view title) const {
			for ( const auto& config : mConfigSet ) {
				if ( config.getTitle() == title ) {
					return true;
				}
			}
			return false;
		}

		void read() {
			std::ifstream mFile(mPath);
			if ( !mFile.is_open() ) {
				throw std::runtime_error("Failed to open file");
			}
			std::string line;
			std::vector<TConfig> subConfig;
			while ( std::getline(mFile, line) ) {
				removeFrontBlank(line);
				if ( isBlankOrComment(line) ) {
					continue;
				}
				if ( isTitle(line) ) {
					std::string title = line.substr(1, line.find(']') - 1);
					mConfigSet.push_back(TConfig(title));
					continue;
				}
				if ( line.find('=') != std::string::npos ) {
					std::string key = line.substr(0, line.find('='));
					removeBackBlank(key);
					std::string value = line.substr(line.find('=') + 1);
					removeFrontBlank(value);
					removeBackBlank(value);
					value = isEndComment(value);
					removeQuoteQue(value);
					if ( value.at(0) != '{' ) {
						if ( subConfig.size() > 0 ) {
							subConfig.back().addDictionary(TDictionary(key, value));
						} else {
							mConfigSet.back().addDictionary(TDictionary(key, value));
						}
					} else {
						subConfig.push_back(TConfig(key));
					}
				} else if ( line.at(0) == '}' ) {
					if ( subConfig.size() > 1 ) {
						(subConfig.end() - 2)->addConfig(subConfig.back());
						subConfig.pop_back();
					} else {
						mConfigSet.back().addConfig(subConfig[0]);
						subConfig.pop_back();
					}
				}

			}
		}

		TConfig getConfig(std::string_view title) const {
			for ( const auto& config : mConfigSet ) {
				if ( config.getTitle() == title ) {
					return config;
				}
			}
			throw std::runtime_error("Config not found");
		}

		TConfig& modifyConfig(std::string_view title) {
			for ( auto& config : mConfigSet ) {
				if ( config.getTitle() == title ) {
					return config;
				}
			}
			throw std::runtime_error("Config not found");
		}

		void removeFrontBlank(std::string& line) {
			auto it = line.begin();
			while ( it != line.end() && std::isspace(*it) ) {
				++it;
			}
			line.erase(line.begin(), it);
		}
		void removeBackBlank(std::string& line) {
			auto it = line.end();
			while ( it != line.begin() && std::isspace(*(it - 1)) ) {
				--it;
			}
			line.erase(it, line.end());
		}

		bool isBlankOrComment(std::string_view line) const {
			if ( line.empty() || line.at(0) == '#' ) {
				return true;
			} else {
				return false;
			}
		}

		void removeQuoteQue(std::string& line) {
			if ( line.at(0) == '"' ) {
				line.erase(line.begin());
			}
			if ( line.back() == '"' ) {
				line.pop_back();
			}
		}

		std::string isEndComment(std::string_view line) const {
			std::string temp = std::string(line);
			if ( temp.find("#") == std::string::npos ) {
				return temp;
			} else {
				int nQuote = 0;
				for ( int i = 0; i < temp.size(); i++ ) {
					if ( temp[i] == '"' ) {
						nQuote++;
					}
					if ( nQuote % 2 == 0 && temp[i] == '#' ) {
						return temp.substr(0, i - 1);
					}
				}
			}
			return temp;
		}

		/**
		 * @brief Method to test a line is title or not
		 *
		 * @param line
		 * @return true
		 * @return false
		*/
		bool isTitle(std::string_view line) const {
			if ( line.at(0) == '[' && line.at(line.size() - 1) == ']' ) {
				if ( std::count(line.begin(), line.end(), '[') > 1 || std::count(line.begin(), line.end(), ']') > 1 ) {
					std::cerr << "Invalide title format. Title should be in the format [TITLE]" << std::endl;
					return false;
				} else {
					return true;
				}
			} else {
				return false;
			}
		}
	};
}


#endif // __TCONFIG_H__