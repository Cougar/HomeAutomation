/*
 * file.cpp
 *
 *  Created on: Aug 04, 2009
 *      Author: Mattias Runge
 */

#include "file.h"

namespace atom {
namespace file {


string get_contents(string filename)
{
	ifstream srcfile(filename.c_str());

	if (!srcfile.is_open())
	{
		srcfile.close();
		return "";
	}

	string buffer = "";
	string line;

	while (!srcfile.eof())
	{
		getline(srcfile, line);

		if (srcfile.eof()) break;

		buffer += line + "\n";
	};

	srcfile.close();

	return buffer;
}

bool exists(string filename)
{
	ifstream file(filename.c_str());
	if (file.is_open())
	{
		file.close();
		return true;
	}

	return false;
}


}
}
