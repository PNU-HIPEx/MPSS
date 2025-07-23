#ifndef __CPPARGS_H__
#define __CPPARGS_H__

#include <iostream>
#include <algorithm>
#include <vector>
#include <cstdarg>
#include <typeinfo>
#include <unordered_map>
#include <sstream>

enum ActionType {
	ACTION_NONE,
	ACTION_STORE,
	ACTION_STORE_CONST,
	ACTION_STORE_TRUE,
	ACTION_STORE_FALSE,
	ACTION_APPEND,
	ACTION_APPEND_CONST,
	ACTION_COUNT,
	ACTION_HELP,
	ACTION_VERSION,
	ACTION_PARSERS,
	ACTION_EXTEND
};

class Argument;

class HelpMessage {
private:
	std::string usage;
public:
	HelpMessage(std::string prog, std::string description);
	void print(std::vector<Argument>& Pos_args, std::vector<Argument>& Opt_args);
};

enum ARGTYPES {
	INT,
	DOUBLE,
	BOOL,
	STRING,
	NONE
};

class Argument {
private:
	ARGTYPES _argType = ARGTYPES::NONE;
	std::vector<std::string> _argValueList;
	std::vector<std::string> _argDomain;

	std::string _argName = "";
	std::string _argOpt = "";
	std::vector<std::string> _argMinorOpt;

	std::string _argDenoteOut = "";
	std::string _argDenoteIn = "";

	std::string _description = "";

	bool _isArgMulti = false;
	bool _isArgConst = false;

public:
	Argument(std::string str);

	// Setter
	void setArgType(ARGTYPES typ);

	void setArgValue(std::string str);
	void setArgValueList(std::vector<std::string> strList);
	void replaceArgValueList(std::vector<std::string> strList);
	void setArgDomain(std::string str);
	void setArgDomain(std::vector<std::string> strList);

	void setArgName(std::string str);
	void setArgOpt(std::string str);
	void setArgMinorOpt(std::string str);
	void setArgMinorOpt(std::vector<std::string> strList);

	void setArgDenoteOut(std::string str);
	void setArgDenoteIn(std::string str);

	void setArgDescription(std::string description);

	void isArgMulti();
	void notArgMulti();
	void isArgConst();
	void notArgConst();

	// Getter
	ARGTYPES getArgType() const;
	std::string getArgValue(const int order) const;
	std::vector<std::string> getArgValueList() const;
	std::string getArgDomain(const int order) const;
	std::vector<std::string> getArgDomain() const;

	std::string getArgName() const;
	std::string getArgOpt() const;
	std::vector<std::string> getArgMinorOpt() const;

	std::string getArgDenoteOut() const;
	std::string getArgDenoteIn() const;

	std::string getArgDescription() const;

	bool getArgMulti() const;
	bool getArgConst() const;
};

class ArgumentParser {
private:
	std::vector<std::string> argv;                                //
	std::string prog = "";                      // The name of the program (default: argv[0])   
	std::string description = "";               // A description of what the program does
	std::vector<Argument> Pos_args;
	std::vector<Argument> Opt_args;
	Argument* args_temp;
	std::unordered_map<std::string, std::vector<std::string>> argv_init;
	bool needHelp = false;
public:
	ArgumentParser(int _argc, char** _argv);
	~ArgumentParser();

	ArgumentParser& setDescription(std::string _description) { description = _description; return *this; }
	ArgumentParser& add_argument(const std::string& opts);
	ArgumentParser& add_minor_argument(const std::string& opts);
	ArgumentParser& add_domain(const std::vector<std::string>& opts);
	ArgumentParser& dest(const std::string& str);
	ArgumentParser& metavar(const std::string& str);
	ArgumentParser& set_const();
	ArgumentParser& nargs();
	ArgumentParser& type(std::string typeOpt); // You can set the type in int, double, bool, string.
	ArgumentParser& help(std::string message);
	ArgumentParser& set_default(std::string value);

	ARGTYPES detType(const std::string& str);
	void add_finish();
	void parse_args();
	template<typename T>
	T get_value(const std::string& valueName) const;
};

#endif