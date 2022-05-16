#include <Windows.h>
#include "key_config.h"
#include "inipp.h"
#include <fstream>
#include "stdafx.h"

LPCTSTR lpAppName;
LPCTSTR lpKeyName;
LPCTSTR lpDefault;
LPTSTR  lpReturnedString;
LPCTSTR lpFileName;

bool getKeyConfig()
{
	inipp::Ini<char> ini;
	std::ifstream is("config.ini");
	ini.parse(is);
	std::cout << "raw ini file:" << std::endl;
	ini.generate(std::cout);
	ini.strip_trailing_comments();
	ini.default_section(ini.sections["SYSTEM"]);
	ini.interpolate();
	std::cout << "ini file after default section and interpolation:" << std::endl;
	ini.generate(std::cout);
	int compression_level = -1;
	inipp::get_value(ini.sections["player1"], "UP", compression_level);
	std::cout << "bitbucket.org compression level: " << compression_level << std::endl;
	return 0;
}