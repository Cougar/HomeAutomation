/*
 * string.cpp
 *
 *  Created on: Aug 04, 2009
 *      Author: Mattias Runge
 */

#include "string.h"

namespace atom {
namespace string_ {

void escape(string & in)
{
	string out;

	// TODO Use replace functions instead maybe?
	for (unsigned int n = 0; n < in.length(); n++)
	{
		switch (in[n])
		{
		case '\r':
			break;

		case '\n':
			out += "\\n";
			break;

		case '\'':
			out += "\\'";
			break;

		case '"':
			out += "\\\"";
			break;

		default:
			out += in[n];
			break;
		}
	}

	in = out;
}

void pad_right(string & in, unsigned int length, char c)
{
	while (in.length() < length)
	{
		in += c;
	}
}

void pad_left(string & in, unsigned int length, char c)
{
	while (in.length() < length)
	{
		in = c + in;
	}
}

string escape_copy(string in)
{
	escape(in);
	return in;
}

string pad_right_copy(string in, unsigned int length, char c)
{
	pad_right(in, length, c);
	return in;
}

string pad_left_copy(string in, unsigned int length, char c)
{
	pad_left(in, length, c);
	return in;
}

}
}
