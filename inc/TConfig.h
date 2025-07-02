
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
#include<type_traits>
#include<exception>
#include<algorithm>
#include<unordered_map>


namespace KEI {
	class TDictionary {
	public:
		TDictionary(std::string_view key, std::string_view value) : mKey(key), mValue(value) { }
	private:
		std::string mKey;
		std::string mValue;
	public:
		void setValue(std::string_view value) {
			mValue = value;
		}
		std::string getKey() const { return mKey; }
		template<typename T>
		T getValue() const {
			if constexpr ( std::is_same_v<T, int> || std::is_same_v<T, signed> || std::is_same_v < T, signed int> || std::is_same_v<T, int32_t> ) {
				return std::stoi(mValue);
			} else if constexpr ( std::is_same_v<T, unsigned int> || std::is_same_v<T, unsigned> || std::is_same_v<T, uint32_t> ) {
				return static_cast<unsigned int>(std::stoul(mValue));
			} else if constexpr ( std::is_same_v<T, int8_t> || std::is_same_v<T, signed char> ) {
				return static_cast<int8_t>(mValue.at(0));
			} else if constexpr ( std::is_same_v<T, uint8_t> || std::is_same_v<T, unsigned char> ) {
				return static_cast<uint8_t>(mValue.at(0));
			} else if constexpr ( std::is_same_v<T, int16_t> || std::is_same_v<T, short> || std::is_same_v<T, short int> || std::is_same_v<T, signed short> || std::is_same_v<T, signed short int> ) {
				return static_cast<int16_t>(std::stoi(mValue));
			} else if constexpr ( std::is_same_v<T, uint16_t> || std::is_same_v<T, unsigned short> || std::is_same_v<T, unsigned short int> ) {
				return static_cast<uint16_t>(std::stoul(mValue));
			} else if constexpr ( std::is_same_v<T, long> || std::is_same_v<T, signed long> || std::is_same_v<T, signed long int> ) {
				return static_cast<long>(std::stol(mValue));
			} else if constexpr ( std::is_same_v<T, unsigned long> || std::is_same_v<T, unsigned long int> ) {
				return static_cast<unsigned long>(std::stoul(mValue));
			} else if constexpr ( std::is_same_v<T, long long> || std::is_same_v<T, signed long long> || std::is_same_v<T, signed long long int> || std::is_same_v<T, int64_t> ) {
				return static_cast<int64_t>(std::stoll(mValue));
			} else if constexpr ( std::is_same_v<T, unsigned long long> || std::is_same_v<T, unsigned long long int> || std::is_same_v<T, uint64_t> ) {
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
				if ( mValue.at(0) == '"' && mValue.at(mValue.size() - 1) == '"' ) {
					return mValue.substr(1, mValue.size() - 2);
				} else {
					return mValue;
				}
			} else {
				static_assert(!std::is_same_v<T, T>, "Unsupported type");
			}
			return T();
		}
		template<typename T, int N>
		std::array<T, N> getValue() const {
			static_assert(N > 0, "N must be greater than 0");
			std::array<T, N> arr;
			std::istringstream valueList(mValue);
			std::string value;

			int n = 0;
			while ( valueList >> value ) {
				if constexpr ( std::is_same_v<T, int> ) {
					arr[n] = std::stoi(value);
				} else if constexpr ( std::is_same_v<T, unsigned int> ) {
					arr[n] = static_cast<unsigned int>(std::stoul(value));
				} else if constexpr ( std::is_same_v<T, float> ) {
					arr[n] = static_cast<float>(std::stof(value));
				} else if constexpr ( std::is_same_v<T, double> ) {
					arr[n] = static_cast<double>(std::stod(value));
				} else if constexpr ( std::is_same_v<T, long double> ) {
					arr[n] = static_cast<long double>(std::stold(value));
				} else if constexpr ( std::is_same_v<T, char> ) {
					arr[n] = value.at(0);
				} else if constexpr ( std::is_same_v<T, std::string> ) {
					arr[n] = value;
				} else {
					static_assert(!std::is_same_v<T, T>, "Unsupported type");
				}
				n++;
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
		TConfig(const TConfig& config) : mTitle(config.mTitle) {
			for ( const auto& config : config.mConfigSet ) {
				mConfigSet.push_back(config);
			}
			for ( const auto& dict : config.mDictSet ) {
				mDictSet.push_back(dict);
			}
		};
	private:
		std::string mTitle;
		std::vector<TConfig> mConfigSet;
		std::vector<TDictionary> mDictSet;
	public:
		void addConfig(const TConfig& config) {
			mConfigSet.push_back(config);
		}
		void addDictionary(const TDictionary& dict) {
			if ( hasKey(dict.getKey()) ) {
				for ( auto& existingDict : mDictSet ) {
					if ( existingDict.getKey() == dict.getKey() ) {
						existingDict.setValue(dict.getValue<std::string>());
						return;
					}
				}
			} else {
				mDictSet.push_back(dict);
			}
		}
		void addDictionary(std::string_view key, std::string_view value) {
			if ( hasKey(key) ) {
				for ( auto& existingDict : mDictSet ) {
					if ( existingDict.getKey() == key ) {
						existingDict.setValue(value);
						return;
					}
				}
			} else {
				TDictionary dict(key, value);
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
		std::vector<std::string> getValueList() const {
			std::vector<std::string> valueList;
			for ( const auto& dict : mDictSet ) {
				valueList.push_back(dict.getValue<std::string>());
				return valueList;
			}
			throw std::runtime_error("Value list not found");
		}
		bool hasKey(std::string_view key) const {
			for ( const auto& dict : mDictSet ) {
				if ( dict.getKey() == key ) {
					return true;
				}
			}
			for ( const auto& config : mConfigSet ) {
				if ( config.getTitle() == key ) {
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
		TConfig modifyConfig(std::string_view title) {
			for ( auto& config : mConfigSet ) {
				if ( config.mTitle == title ) {
					return config;
				}
			}
			throw std::runtime_error("Config not found");
		}
		std::vector<TConfig> getSubConfigSet() const {
			return mConfigSet;
		}
		std::vector<std::string> getConfigNameSet() const {
			std::vector<std::string> subConfigNames;
			for ( const auto& config : mConfigSet ) {
				subConfigNames.push_back(config.getTitle());
			}
			return subConfigNames;
		}
		std::string getConfigName() const {
			return mTitle;
		}
		std::unordered_map<std::string, TConfig> getConfigSetWithName() const {
			std::unordered_map<std::string, TConfig> configMap;
			for ( const auto& config : mConfigSet ) {
				configMap.insert_or_assign(config.getTitle(), config);
			}
			return configMap;
		}
	};

	class TConfigFile {
	private:
		std::filesystem::path mPath;
		std::ifstream mFile;

		std::vector<TConfig> mConfigSet;
	public:
		TConfigFile() = default;
		TConfigFile(const TConfigFile& configFile) : mPath(configFile.mPath), mConfigSet(configFile.mConfigSet) { }
		~TConfigFile() = default;

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

		void addConfig(const TConfig& config) {
			mConfigSet.push_back(config);
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
			mFile.open(mPath);
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
		TConfig& modifyConfig(std::string_view title) {
			for ( TConfig& config : mConfigSet ) {
				if ( config.getTitle() == title ) {
					return config;
				}
			}
			throw std::runtime_error("Config not found");
		}

		const TConfig& getConfig(std::string_view title) const {
			for ( const auto& config : mConfigSet ) {
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

		std::vector<TConfig> getConfigSet() const {
			return mConfigSet;
		}
		std::vector<std::string> getConfigNameSet() const {
			std::vector<std::string> configNameSet;
			for ( const auto& config : mConfigSet ) {
				configNameSet.push_back(config.getTitle());
			}
			return configNameSet;
		}
	};
}


#endif // __TCONFIG_H__