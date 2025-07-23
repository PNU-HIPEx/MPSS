#include "cppargs.hpp"
// HelpMessage class
HelpMessage::HelpMessage(std::string prog, std::string description) {
	std::cout << description << std::endl << std::endl;
}

void HelpMessage::print(std::vector<Argument>& Pos_args, std::vector<Argument>& Opt_args) {
	int sentense_size = 0;
	if ( Pos_args.size() != 0 ) {
		std::cout << "positional arguments:" << std::endl;
		for ( Argument& arg : Pos_args ) {
			std::cout << "  " << arg.getArgDenoteOut();
			sentense_size += arg.getArgDenoteOut().size() + 2;
			if ( arg.getArgDomain().size() != 0 ) {
				int rest = 0;
				std::cout << " {";
				sentense_size += 2;
				for ( std::string& domain : arg.getArgDomain() ) {
					if ( rest < arg.getArgDomain().size() - 1 ) {
						std::cout << domain << ",";
						sentense_size += domain.size() + 1;
					} else {
						std::cout << domain;
						sentense_size += domain.size();
					}
					rest++;
				}
				std::cout << "}";
				sentense_size += 1;
			}
			if ( arg.getArgMinorOpt().size() != 0 ) {
				for ( std::string& minorOpt : arg.getArgMinorOpt() ) {
					std::cout << ", " << minorOpt;
					sentense_size += 2;
				}
				if ( arg.getArgDomain().size() != 0 ) {
					int rest = 0;
					std::cout << " {";
					sentense_size += 2;
					for ( std::string& domain : arg.getArgDomain() ) {
						if ( rest < arg.getArgDomain().size() - 1 ) {
							std::cout << domain << ",";
							sentense_size += domain.size() + 1;
						} else {
							std::cout << domain;
							sentense_size += domain.size();
						}
						rest++;
					}
					std::cout << "}";
					sentense_size += 1;
				}
			}
			if ( sentense_size < 9 ) {
				std::cout << "\t\t\t";
			} else if ( sentense_size < 17 ) {
				std::cout << "\t\t";
			} else if ( sentense_size < 25 ) {
				std::cout << "\t";
			} else {
				std::cout << std::endl << "\t\t\t";
			}
			std::cout << arg.getArgDescription();
			std::cout << std::endl;
			sentense_size = 0;
		}
	}
	std::cout << std::endl;
	std::cout << "optional arguments:" << std::endl;
	std::cout << "  -h, --help\t\tshow this help message and exit" << std::endl;
	if ( Opt_args.size() != 0 ) {
		for ( Argument& arg : Opt_args ) {
			std::cout << "  " << arg.getArgDenoteOut();
			sentense_size += 2 + arg.getArgDenoteOut().size();
			if ( arg.getArgDomain().size() != 0 ) {
				int rest = 0;
				std::cout << " {";
				sentense_size += 2;
				for ( std::string& domain : arg.getArgDomain() ) {
					if ( rest < arg.getArgDomain().size() - 1 ) {
						std::cout << domain << ",";
						sentense_size += domain.size() + 1;
					} else {
						std::cout << domain;
						sentense_size += domain.size();
					}
					rest++;
				}
				std::cout << "}";
				sentense_size += 1;
			}
			if ( arg.getArgMinorOpt().size() != 0 ) {
				for ( std::string minorOpt : arg.getArgMinorOpt() ) {
					std::cout << ", " << minorOpt;
					sentense_size += 2;
				}
				if ( arg.getArgDomain().size() != 0 ) {
					int rest = 0;
					std::cout << " {";
					sentense_size += 2;
					for ( std::string& domain : arg.getArgDomain() ) {
						if ( rest < arg.getArgDomain().size() - 1 ) {
							std::cout << domain << ",";
							sentense_size += domain.size() + 1;
						} else {
							std::cout << domain;
							sentense_size += domain.size();
						}
						rest++;
					}
					rest = 0;
					std::cout << "}";
					sentense_size += 1;
				}
			}
			if ( sentense_size < 8 ) {
				std::cout << "\t\t\t";
			} else if ( sentense_size < 16 ) {
				std::cout << "\t\t";
			} else if ( sentense_size < 24 ) {
				std::cout << "\t";
			} else {
				std::cout << std::endl << "\t\t\t";
			}
			std::cout << arg.getArgDescription();
			std::cout << std::endl;
			sentense_size = 0;
		}
	}
}

// Argument class
Argument::Argument(std::string str) : _argOpt(str) { }

void Argument::setArgType(ARGTYPES typ) {
	_argType = typ;
}

void Argument::setArgValue(std::string str) {
	_argValueList.push_back(str);
}

void Argument::setArgValueList(std::vector<std::string> strList) {
	_argValueList.reserve(_argValueList.size() + strList.size());
	_argValueList.insert(_argValueList.end(), strList.begin(), strList.end());
}

void Argument::replaceArgValueList(std::vector<std::string> strList) {
	_argValueList.clear();
	_argValueList.reserve(strList.size());
	_argValueList.assign(strList.begin(), strList.end());
}

void Argument::setArgDomain(std::string str) {
	_argDomain.push_back(str);
}

void Argument::setArgDomain(std::vector<std::string> strList) {
	_argDomain.reserve(_argDomain.size() + strList.size());
	_argDomain.insert(_argDomain.end(), strList.begin(), strList.end());
}

void Argument::setArgName(std::string str) {
	_argName = str;
}

void Argument::setArgOpt(std::string str) {
	_argOpt = str;
}

void Argument::setArgMinorOpt(std::string str) {
	_argMinorOpt.push_back(str);
}

void Argument::setArgMinorOpt(std::vector<std::string> strList) {
	_argMinorOpt.reserve(_argMinorOpt.size() + strList.size());
	_argMinorOpt.insert(_argMinorOpt.end(), strList.begin(), strList.end());
}

void Argument::setArgDenoteOut(std::string str) {
	_argDenoteOut = str;
}

void Argument::setArgDenoteIn(std::string str) {
	_argDenoteIn = str;
}

void Argument::setArgDescription(std::string description) {
	_description = description;
}

void Argument::isArgMulti() {
	_isArgMulti = true;
}

void Argument::notArgMulti() {
	_isArgMulti = false;
}

void Argument::isArgConst() {
	_isArgConst = true;
}

void Argument::notArgConst() {
	_isArgConst = false;
}

ARGTYPES Argument::getArgType() const {
	return _argType;
}

std::string Argument::getArgValue(const int order) const {
	return _argValueList[order];
}

std::vector<std::string> Argument::getArgValueList() const {
	return _argValueList;
}

std::string Argument::getArgDomain(const int order) const {
	return _argDomain[order];
}

std::vector<std::string> Argument::getArgDomain() const {
	return _argDomain;
}

std::string Argument::getArgName() const {
	return _argName;
}

std::string Argument::getArgOpt() const {
	return _argOpt;
}

std::vector<std::string> Argument::getArgMinorOpt() const {
	return _argMinorOpt;
}

std::string Argument::getArgDenoteOut() const {
	return _argDenoteOut;
}

std::string Argument::getArgDenoteIn() const {
	return _argDenoteIn;
}

std::string Argument::getArgDescription() const {
	return _description;
}

bool Argument::getArgMulti() const {
	return _isArgMulti;
}

bool Argument::getArgConst() const {
	return _isArgConst;
}

// ArgumentParser class
ArgumentParser::ArgumentParser(int _argc, char** _argv) {
	for ( int arg = 0; arg < _argc; arg++ ) {
		argv.push_back(_argv[arg]);
	}
	std::string key = "Positional";
	std::vector<std::string> temp_value;

	bool onlyPositional = true;

	for ( std::string& arg : argv ) {
		if ( arg == "--help" || arg == "-h" ) {
			needHelp = true;
		}
		if ( arg[0] != '-' && arg[0] != '.' ) {
			temp_value.push_back(arg);
		}
		if ( arg[0] == '-' ) {
			argv_init.insert({key, temp_value});
			key = arg;
			temp_value.clear();
		}
		if ( arg == argv.back() && onlyPositional ) {
			argv_init.insert({key, temp_value});
		}
	}
}

ArgumentParser::~ArgumentParser() { }

ArgumentParser& ArgumentParser::add_argument(const std::string& opts) {
	args_temp = new Argument(opts);
	std::string name;
	for ( int i = 0; i < opts.length(); i++ ) {
		if ( opts[i] != '-' ) {
			name = opts.substr(i, opts.length());
			break;
		}
	}
	args_temp->setArgName(name);
	args_temp->setArgDenoteIn(name);
	args_temp->setArgDenoteOut(opts);
	return *this;
}

ArgumentParser& ArgumentParser::add_minor_argument(const std::string& opts) {
	args_temp->setArgMinorOpt(opts);
	return *this;
}

ArgumentParser& ArgumentParser::add_domain(const std::vector<std::string>& opts) {
	args_temp->setArgDomain(opts);
	return *this;
}

ArgumentParser& ArgumentParser::dest(const std::string& str) {
	args_temp->setArgDenoteIn(str);
	return *this;
}

ArgumentParser& ArgumentParser::metavar(const std::string& str) {
	args_temp->setArgDenoteOut(str);
	return *this;
}

ArgumentParser& ArgumentParser::set_const() {
	args_temp->isArgConst();
	return *this;
}

ArgumentParser& ArgumentParser::nargs() {
	args_temp->isArgMulti();
	return *this;
}

ArgumentParser& ArgumentParser::type(std::string typeOpt) {
	ARGTYPES argType;
	if ( typeOpt == "int" ) {
		argType = ARGTYPES::INT;
	} else if ( typeOpt == "double" ) {
		argType = ARGTYPES::DOUBLE;
	} else if ( typeOpt == "bool" ) {
		argType = ARGTYPES::BOOL;
	} else if ( typeOpt == "string" ) {
		argType = ARGTYPES::STRING;
	} else {
		argType = ARGTYPES::NONE;
		std::cout << "NO TYPE WARNING (SET TO ARBITRARY TYPE)" << std::endl;
	}
	args_temp->setArgType(argType);
	return *this;
}

ArgumentParser& ArgumentParser::help(std::string message) {
	args_temp->setArgDescription(message);
	return *this;
}

ArgumentParser& ArgumentParser::set_default(std::string value) {
	args_temp->setArgValue(value);
	return *this;
}

ARGTYPES ArgumentParser::detType(const std::string& str) {
	std::istringstream iss(str);
	int intVal;
	float floatVal;
	if ( iss >> intVal && iss.eof() ) {
		return ARGTYPES::INT;
	} else if ( iss.clear(), iss.seekg(0), iss >> floatVal && iss.eof() ) {
		return ARGTYPES::DOUBLE;
	} else if ( str == "true" || str == "false" ) {
		return ARGTYPES::BOOL;
	} else {
		return ARGTYPES::STRING;
	}
}

void ArgumentParser::add_finish() {
	Argument* args_add = args_temp;
	args_add->getArgOpt()[0] == '-' ? Opt_args.push_back(*args_add) : Pos_args.push_back(*args_add);
	args_temp = nullptr;
}

void ArgumentParser::parse_args() {
	if ( needHelp ) {
		HelpMessage* message = new HelpMessage(prog, description);
		message->print(Pos_args, Opt_args);
		exit(0);
	}

	if ( Pos_args.size() > argv_init["Positional"].size() ) {
		std::cout << "ERROR: Not enough required arguments." << std::endl;
		exit(0);
	}

	int numOfMulti = 0;
	for ( Argument& pos : Pos_args ) {
		if ( pos.getArgMulti() ) {
			numOfMulti++;
		}
	}
	if ( numOfMulti > 2 || (numOfMulti == 1 && !Pos_args.back().getArgMulti()) ) {
		std::cout << "Cannot classify that the arguments are belong to group" << std::endl;
		exit(0);
	}

	int numArg = 0;
	for ( Argument& opt : Pos_args ) {
		if ( numArg < Pos_args.size() - 1 ) {
			opt.replaceArgValueList({(argv_init.find("Positional")->second)[0]});
			argv_init.find("Positional")->second.erase(argv_init.find("Positional")->second.begin());
		} else {
			opt.replaceArgValueList(argv_init.find("Positional")->second);
		}
		numArg++;
	}

	for ( Argument& opt : Pos_args ) {
		if ( opt.getArgType() == ARGTYPES::NONE ) {
			opt.setArgType(detType(opt.getArgValueList()[0]));
		} else {
			if ( opt.getArgValueList().size() != 0 && opt.getArgType() != detType(opt.getArgValueList()[0]) ) {
				std::cout << "Not proper type variable" << std::endl;
				exit(0);
			}
		}
		if ( !opt.getArgMulti() && opt.getArgValueList().size() > 1 ) {
			std::cout << "Cannot enter more than 2 variables into this variable" << std::endl;
			exit(0);
		}
	}

	for ( std::pair<std::string, std::vector<std::string>> argv : argv_init ) {
		if ( argv.first == "Positional" ) continue;
		bool isArguExist = false;
		for ( Argument& opt : Opt_args ) {
			if ( opt.getArgOpt() == argv.first ) {
				isArguExist = true;
				opt.replaceArgValueList(argv.second);
				break;
			}
		}
		if ( !isArguExist ) {
			std::cout << "Undefined argument" << std::endl;
			exit(0);
		}
	}
	for ( Argument& opt : Opt_args ) {
		if ( opt.getArgType() == ARGTYPES::NONE ) {
			opt.setArgType(detType(opt.getArgValueList()[0]));
		} else {
			if ( opt.getArgValueList().size() != 0 && opt.getArgType() != detType(opt.getArgValueList()[0]) ) {
				std::cout << "Not proper type variable" << std::endl;
				exit(0);
			}
		}
		if ( !opt.getArgMulti() && opt.getArgValueList().size() > 1 ) {
			std::cout << "Cannot enter more than 2 variables into this variable" << std::endl;
			exit(0);
		}
	}
}

template<typename T>
T ArgumentParser::get_value(const std::string& valueName) const {

}
template<>
int ArgumentParser::get_value<int>(const std::string& valueName) const {
	bool isexist = false;
	ARGTYPES returnType = ARGTYPES::NONE;
	int returnValue;
	for ( Argument pos : Pos_args ) {
		if ( pos.getArgName() == valueName ) {
			returnValue = stoi(pos.getArgValueList()[0]);
			returnType = pos.getArgType();
			isexist = true;
			break;
		}
	}
	for ( Argument opt : Opt_args ) {
		if ( opt.getArgName() == valueName ) {
			returnValue = stoi(opt.getArgValueList()[0]);
			returnType = opt.getArgType();
			isexist = true;
			break;
		}
	}
	if ( !isexist ) {
		std::cout << "No proper argument" << std::endl;
		exit(0);
	}
	if ( returnType != ARGTYPES::INT ) {
		std::cout << "Not proper type" << std::endl;
		exit(0);
	}
	return returnValue;
}

template<>
double ArgumentParser::get_value<double>(const std::string& valueName) const {
	bool isexist = false;
	ARGTYPES returnType = ARGTYPES::NONE;
	double returnValue;
	for ( Argument pos : Pos_args ) {
		if ( pos.getArgName() == valueName ) {
			returnValue = stod(pos.getArgValueList()[0]);
			returnType = pos.getArgType();
			isexist = true;
			break;
		}
	}
	for ( Argument opt : Opt_args ) {
		if ( opt.getArgName() == valueName ) {
			returnValue = stod(opt.getArgValueList()[0]);
			returnType = opt.getArgType();
			isexist = true;
			break;
		}
	}
	if ( !isexist ) {
		std::cout << "No proper argument" << std::endl;
		exit(0);
	}
	if ( returnType != ARGTYPES::DOUBLE ) {
		std::cout << "Not proper type" << std::endl;
		exit(0);
	}
	return returnValue;
}

template<>
bool ArgumentParser::get_value<bool>(const std::string& valueName) const {
	bool isexist = false;
	ARGTYPES returnType = ARGTYPES::NONE;
	bool returnValue;
	for ( Argument pos : Pos_args ) {
		if ( pos.getArgName() == valueName ) {
			returnValue = pos.getArgValueList()[0] == "true" ? true : false;
			returnType = pos.getArgType();
			isexist = true;
			break;
		}
	}
	for ( Argument opt : Opt_args ) {
		if ( opt.getArgName() == valueName ) {
			returnValue = opt.getArgValueList()[0] == "true" ? true : false;
			returnType = opt.getArgType();
			isexist = true;
			break;
		}
	}
	if ( !isexist ) {
		std::cout << "No proper argument" << std::endl;
		exit(0);
	}
	if ( returnType != ARGTYPES::BOOL ) {
		std::cout << "Not proper type" << std::endl;
		exit(0);
	}
	return returnValue;
}
template<>
std::string ArgumentParser::get_value<std::string>(const std::string& valueName) const {
	bool isexist = false;
	ARGTYPES returnType = ARGTYPES::NONE;
	std::string returnValue;
	for ( Argument pos : Pos_args ) {
		if ( pos.getArgName() == valueName ) {
			returnValue = pos.getArgValueList()[0];
			returnType = pos.getArgType();
			isexist = true;
			break;
		}
	}
	for ( Argument opt : Opt_args ) {
		if ( opt.getArgName() == valueName ) {
			returnValue = opt.getArgValueList()[0];
			returnType = opt.getArgType();
			isexist = true;
			break;
		}
	}
	if ( !isexist ) {
		std::cout << "No proper argument" << std::endl;
		exit(0);
	}
	if ( returnType != ARGTYPES::STRING ) {
		std::cout << "Not proper type" << std::endl;
		exit(0);
	}
	return returnValue;
}

template<>
std::vector<int> ArgumentParser::get_value<std::vector<int>>(const std::string& valueName) const {
	bool isexist = false;
	ARGTYPES returnType = ARGTYPES::NONE;
	std::vector<int> returnValue;
	for ( Argument pos : Pos_args ) {
		if ( pos.getArgName() == valueName ) {
			for ( std::string val : pos.getArgValueList() ) {
				returnValue.push_back(stoi(val));
			}
			returnType = pos.getArgType();
			isexist = true;
			break;
		}
	}
	for ( Argument opt : Opt_args ) {
		if ( opt.getArgName() == valueName ) {
			for ( std::string val : opt.getArgValueList() ) {
				returnValue.push_back(stoi(val));
			}
			returnType = opt.getArgType();
			isexist = true;
			break;
		}
	}
	if ( !isexist ) {
		std::cout << "No proper argument" << std::endl;
		exit(0);
	}
	if ( returnType != ARGTYPES::INT ) {
		std::cout << "Not proper type" << std::endl;
		exit(0);
	}

	return returnValue;
}

template<>
std::vector<double> ArgumentParser::get_value<std::vector<double>>(const std::string& valueName) const {
	bool isexist = false;
	ARGTYPES returnType = ARGTYPES::NONE;
	std::vector<double> returnValue;
	for ( Argument pos : Pos_args ) {
		if ( pos.getArgName() == valueName ) {
			for ( std::string val : pos.getArgValueList() ) {
				returnValue.push_back(stod(val));
			}
			returnType = pos.getArgType();
			isexist = true;
			break;
		}
	}
	for ( Argument opt : Opt_args ) {
		if ( opt.getArgName() == valueName ) {
			for ( std::string val : opt.getArgValueList() ) {
				returnValue.push_back(stod(val));
			}
			returnType = opt.getArgType();
			isexist = true;
			break;
		}
	}
	if ( !isexist ) {
		std::cout << "No proper argument" << std::endl;
		exit(0);
	}
	if ( returnType != ARGTYPES::DOUBLE ) {
		std::cout << "Not proper type" << std::endl;
		exit(0);
	}
	return returnValue;
}

template<>
std::vector<bool> ArgumentParser::get_value<std::vector<bool>>(const std::string& valueName) const {
	bool isexist = false;
	ARGTYPES returnType = ARGTYPES::NONE;
	std::vector<bool> returnValue;
	for ( Argument pos : Pos_args ) {
		if ( pos.getArgName() == valueName ) {
			std::transform(pos.getArgValueList().begin(), pos.getArgValueList().end(), std::back_inserter(returnValue), [ ](const std::string& str) { return str == "true" ? true : false; });
			returnType = pos.getArgType();
			isexist = true;
			break;
		}
	}
	for ( Argument opt : Opt_args ) {
		if ( opt.getArgName() == valueName ) {
			std::transform(opt.getArgValueList().begin(), opt.getArgValueList().end(), std::back_inserter(returnValue), [ ](const std::string& str) { return str == "true" ? true : false; });
			returnType = opt.getArgType();
			isexist = true;
			break;
		}
	}
	if ( !isexist ) {
		std::cout << "No proper argument" << std::endl;
		exit(0);
	}
	if ( returnType != ARGTYPES::BOOL ) {
		std::cout << "Not proper type" << std::endl;
		exit(0);
	}
	return returnValue;
}
template<>
std::vector<std::string> ArgumentParser::get_value<std::vector<std::string>>(const std::string& valueName) const {
	bool isexist = false;
	ARGTYPES returnType = ARGTYPES::NONE;
	std::vector<std::string> returnValue;
	for ( Argument pos : Pos_args ) {
		if ( pos.getArgName() == valueName ) {
			returnValue = pos.getArgValueList();
			returnType = pos.getArgType();
			isexist = true;
			break;
		}
	}
	for ( Argument opt : Opt_args ) {
		if ( opt.getArgName() == valueName ) {
			returnValue = opt.getArgValueList();
			returnType = opt.getArgType();
			isexist = true;
			break;
		}
	}
	if ( !isexist ) {
		std::cout << "No proper argument" << std::endl;
		exit(0);
	}
	if ( returnType != ARGTYPES::STRING ) {
		std::cout << "Not proper type" << std::endl;
		exit(0);
	}
	return returnValue;
}