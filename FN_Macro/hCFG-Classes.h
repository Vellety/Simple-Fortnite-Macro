#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <map>

void exitWithError(const std::string &error)
{
	std::cout << error;
	std::cin.ignore();
	std::cin.get();

	exit(EXIT_FAILURE);
}

class Convert
{
public:
	// Convert T to string
	template <typename T>
	static std::string T_to_string(T const &val)
	{
		std::ostringstream ostr;
		ostr << val;

		return ostr.str();
	}

	// Convert string to T
	template <typename T>
	static T string_to_T(std::string const &val)
	{
		std::istringstream istr(val);
		T returnVal;
		if (!(istr >> returnVal))
			exitWithError("[-] CFG file: Not a valid " + (std::string)typeid(T).name() + " received!\n");

		return returnVal;
	}

	template <>
	static std::string string_to_T(std::string const &val)
	{
		return val;
	}
};

class cfgFile
{
private:
	std::map<std::string, std::string> mContent;
	std::string sName;

	void removeComment(std::string &line) const
	{
		if (line.find(';') != line.npos)
			line.erase(line.find(';'));
	}

	bool onlyWhitespace(const std::string &line) const
	{
		return (line.find_first_not_of(' ') == line.npos);
	}

	bool validLine(const std::string &line) const
	{
		std::string temp = line;
		temp.erase(0, temp.find_first_not_of("\t "));
		if (temp[0] == '=')
			return false;

		for (size_t i = temp.find('=') + 1; i < temp.length(); i++)
			if (temp[i] != ' ')
				return true;

		return false;
	}

	// Extract key (key=)
	void extractKey(std::string &key, size_t const &sepPos, const std::string &line) const
	{
		key = line.substr(0, sepPos);
		if (key.find('\t') != line.npos || key.find(' ') != line.npos)
			key.erase(key.find_first_of("\t "));
	}

	// Extract value (=value)
	void extractValue(std::string &value, size_t const &sepPos, const std::string &line) const
	{
		value = line.substr(sepPos + 1);
		value.erase(0, value.find_first_not_of("\t "));
		value.erase(value.find_last_not_of("\t ") + 1);
	}

	void extractContent(const std::string &line)
	{
		std::string temp = line;
		// Erase leading whitespace from the line
		temp.erase(0, temp.find_first_not_of("\t "));
		size_t sepPos = temp.find('=');

		std::string key, value;
		extractKey(key, sepPos, temp);
		extractValue(value, sepPos, temp);

		if (!keyExists(key))
			mContent.insert(std::pair<std::string, std::string>(key, value));
		else
			exitWithError("[-] CFG file has a duplicate error. Please correct it :)\n");
	}

	// lineNo = the current line number in the file
	// line = the current line, with comments removed
	void parseLine(const std::string &line, size_t const lineNo)
	{
		if (line.find('=') == line.npos)
			exitWithError("[-] CFG file is missing one or more separators. Check line: " + Convert::T_to_string(lineNo) + "\n");

		if (!validLine(line))
			exitWithError("[-] CFG file is missing one or more user values. Check line: " + Convert::T_to_string(lineNo) + "\n");

		extractContent(line);
	}

	void ExtractKeys()
	{
		std::ifstream file;
		file.open(sName.c_str());
		if (!file)
			exitWithError("[-] '" + sName + "' was not found in directory.\n");

		std::string line;
		size_t lineNo = 0;
		while (std::getline(file, line))
		{
			lineNo++;
			std::string temp = line;

			if (temp.empty())
				continue;

			removeComment(temp);
			if (onlyWhitespace(temp))
				continue;

			parseLine(temp, lineNo);
		}

		file.close();
	}
public:
	cfgFile(const std::string &sName)
	{
		this->sName = sName;
		ExtractKeys();
	}

	bool keyExists(const std::string &key) const
	{
		return mContent.find(key) != mContent.end();
	}

	template <typename ValueType>
	ValueType getValueOfKey(const std::string &key, ValueType const &defaultValue = ValueType()) const
	{
		if (!keyExists(key))
			return defaultValue;

		return Convert::string_to_T<ValueType>(mContent.find(key)->second);
	}
};