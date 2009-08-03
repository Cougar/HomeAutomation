/*
 * Utils.h
 *
 *  Created on: Apr 26, 2009
 *      Author: Mattias Runge
 */

#ifndef UTILS_H_
#define UTILS_H_

#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <iostream>
#include <cmath>
#include <sys/types.h>
#include <dirent.h>
#include "../types.h"

using namespace std;

namespace atom {
namespace utils {

string bytes2string(byte_list bytes);
string niceTime();
unsigned int hex2uint(string hex);
string hex2bin(string hex);
string bin2hex(string bin);
float bin2float(string bin);
string float2bin(float num, unsigned int length);
unsigned int bin2uint(string bin);
string uint2bin(unsigned int num, unsigned int length);
string uint2hex(unsigned int num, unsigned int length);
vector<string> explode(string delimiter, string str);
string strtoupper(string s);
string strtolower(string s);
string trim(string s);
string trim(const string s, char c);
bool stob(const string s);
int stoi(const string s);
string itos(int i);
string utos(unsigned int i);
unsigned int stou(const string s);
float stof(const string s);
string ftos(float f);
string str_replace(string search, string replace, string str);
string file_get_contents(string filename);
bool file_exists(string filename);
string escape(string in);
string rpad(string in, unsigned int length, char c);
string lpad(string in, unsigned int length, char c);

}
}

#endif /* UTILS_H_ */
