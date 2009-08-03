/*
 * Utils.cpp
 *
 *  Created on: Apr 26, 2009
 *      Author: Mattias Runge
 */

#include "Utils.h"

namespace atom {
namespace utils {

string bytes2string(byte_list bytes)
{
	string str = "";

	for (unsigned int n = 0; n < bytes.size(); n++)
	{
		str += bytes[n];
	}

	return str;
}

string niceTime()
{
	string result;
	struct tm tmStruct;
	time_t t = time(NULL);
	localtime_r(&t, &tmStruct);

	result += lpad(itos(tmStruct.tm_hour), 2, '0');
	result += ":";
	result += lpad(itos(tmStruct.tm_min), 2, '0');
	result += ":";
	result += lpad(itos(tmStruct.tm_sec), 2, '0');

	return result;
}

unsigned int hex2uint(string hex)
{
	return bin2uint(hex2bin(hex));
}

unsigned int bin2uint(string bin)
{
	unsigned int num = 0;

	for (int n = bin.length() - 1; n >= 0; n--)
	{
		if (bin[n] == '1') num += (unsigned int)pow(2.0f, (int)(bin.length()
				- 1 - n));
	}

	return num;
}

string invert(string bin)
{
	string inverted;

	for (unsigned int n = 0; n < bin.length(); n++)
		inverted += (bin[n] == '1' ? '0' : '1');

	return inverted;
}

float bin2float(string bin)
{
	float num = 0;

	if (bin.length() == 0) return 0.0f;

	bool isNegative = false;
	;

	if (bin[0] == '1')
	{
		bin = invert(bin);
		isNegative = true;
	}

	for (int n = bin.length() - 1; n > 0; n--)
	{
		if (bin[n] == '1') num += pow(2.0f, (int)(bin.length() - 1 - n));
	}

	num /= 64.0f;

	if (isNegative) num = -num;

	return num;
}

string bin2hex(string bin)
{
	string hex;

	while (bin.size() > 0)
	{
		string part;

		try
		{
			part = bin.substr(0, 4);
		}
		catch (std::out_of_range& e)
		{
			cout << "DEBUG: bin2hex exception: " << e.what() << "\n";
			cout << "DEBUG: A bin=" << bin << endl;
			continue;
		}

		bin.erase(0, part.size());

		while (part.size() < 4)
			part = "0" + part;

		if (part == "0000")
			hex += '0';
		else if (part == "0001")
			hex += '1';
		else if (part == "0010")
			hex += '2';
		else if (part == "0011")
			hex += '3';
		else if (part == "0100")
			hex += '4';
		else if (part == "0101")
			hex += '5';
		else if (part == "0110")
			hex += '6';
		else if (part == "0111")
			hex += '7';
		else if (part == "1000")
			hex += '8';
		else if (part == "1001")
			hex += '9';
		else if (part == "1010")
			hex += 'a';
		else if (part == "1011")
			hex += 'b';
		else if (part == "1100")
			hex += 'c';
		else if (part == "1101")
			hex += 'd';
		else if (part == "1110")
			hex += 'e';
		else if (part == "1111") hex += 'f';
	}

	return hex;
}

string float2bin(float num, unsigned int length)
{
	string bin;

	///FIXME

	while (bin.size() < length)
		bin = "0" + bin;

	return bin;
}

string hex2bin(string hex)
{
	hex = strtolower(hex);

	string bin;

	for (unsigned int n = 0; n < hex.length(); n++)
	{
		switch (hex[n])
		{
		case '0':
			bin += "0000";
			break;
		case '1':
			bin += "0001";
			break;
		case '2':
			bin += "0010";
			break;
		case '3':
			bin += "0011";
			break;
		case '4':
			bin += "0100";
			break;
		case '5':
			bin += "0101";
			break;
		case '6':
			bin += "0110";
			break;
		case '7':
			bin += "0111";
			break;
		case '8':
			bin += "1000";
			break;
		case '9':
			bin += "1001";
			break;
		case 'a':
			bin += "1010";
			break;
		case 'b':
			bin += "1011";
			break;
		case 'c':
			bin += "1100";
			break;
		case 'd':
			bin += "1101";
			break;
		case 'e':
			bin += "1110";
			break;
		case 'f':
			bin += "1111";
			break;
		}
	}

	return bin;
}

string uint2bin(unsigned int num, unsigned int length)
{
	string bin;

	while (num)
	{
		bin = char((num & 1) + '0') + bin;
		num >>= 1;
	}

	while (bin.size() < length)
		bin = "0" + bin;

	return bin;
}

string uint2hex(unsigned int num, unsigned int length)
{
	return bin2hex(uint2bin(num, length));
}

vector<string> explode(string delimiter, string str)
{
	vector<string> parts;
	string::size_type startPos = 0;
	string::size_type endPos = 0;

	while ((endPos = str.find(delimiter, startPos)) != string::npos)
	{
		try
		{
			parts.push_back(str.substr(startPos, endPos - startPos));
		}
		catch (std::out_of_range& e)
		{
			cout << "DEBUG: explode exception: " << e.what() << "\n";
			cout << "DEBUG: A str=" << str << " :: startPos=" << startPos
					<< " :: endPos=" << endPos << endl;
			continue;
		}

		startPos = endPos + delimiter.length();
	}

	if (startPos < str.length())
	{
		try
		{
			parts.push_back(str.substr(startPos, str.length() - startPos));
		}
		catch (std::out_of_range& e)
		{
			cout << "DEBUG: explode exception: " << e.what() << "\n";
			cout << "DEBUG: B str=" << str << " :: startPos=" << startPos
					<< " :: endPos=" << endPos << endl;
		}
	}

	return parts;
}

string strtoupper(string s)
{
	for (unsigned int n = 0; n < s.size(); n++)
		s[n] = (char)toupper(s[n]);

	return s;
}

string strtolower(string s)
{
	for (unsigned int n = 0; n < s.size(); n++)
		s[n] = (char)tolower(s[n]);

	return s;
}

string trim(string s)
{
	return trim(s, ' ');
}

string trim(const string s, char c)
{
	string result = s;
	while (result[0] == c)
		result.erase(0, 1);

	while (result[result.length() - 1] == c)
		result.erase(result.length() - 1, 1);

	return result;
}

int stoi(const string s)
{
	istringstream in(s);
	int i;
	in >> i;
	return i;
}

string itos(int i)
{
	ostringstream out;
	out << i;
	return out.str();
}

unsigned int stou(const string s)
{
	istringstream in(s);
	unsigned int i;
	in >> i;
	return i;
}

string utos(unsigned int i)
{
	ostringstream out;
	out << i;
	return out.str();
}

float stof(const string s)
{
	istringstream in(s);
	float f;
	in >> f;
	return f;
}

string ftos(float f)
{
	ostringstream out;
	out << f;
	return out.str();
}

string str_replace(string search, string replace, string str)
{
	vector<string> parts = explode(search, str);

	string result;
	for (unsigned int n = 0; n < parts.size(); n++)
	{
		result += parts[n];

		if (n < parts.size() - 1) result += replace;
	}

	return result;
}

string file_get_contents(string filename)
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

bool file_exists(string filename)
{
	ifstream file(filename.c_str());
	if (file.is_open())
	{
		file.close();
		return true;
	}

	return false;
}

string escape(string in)
{
	string out;

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

	return out;
}

string rpad(string in, unsigned int length, char c)
{
	while (in.length() < length)
	{
		in += c;
	}

	return in;
}

string lpad(string in, unsigned int length, char c)
{
	while (in.length() < length)
	{
		in = c + in;
	}

	return in;
}

}
}
