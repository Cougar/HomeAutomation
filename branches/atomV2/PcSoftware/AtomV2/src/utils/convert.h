/*
 * convert.h
 *
 *  Created on: Aug 04, 2009
 *      Author: Mattias Runge
 */

#ifndef CONVERT_H_
#define CONVERT_H_

#include <string>
#include <boost/algorithm/string/case_conv.hpp>
#include "types.h"
#include <fstream>
#include <sstream>
#include <cmath>

namespace atom {
namespace convert {

using namespace std;
using namespace boost::algorithm;

bool string2bool(const string s);
int string2int(const string s);
string int2string(int i);
string uint2string(unsigned int i);
unsigned int string2uint(const string s);
float string2float(const string s);
string float2string(float f);
string bytes2string(byte_list bytes);
unsigned int hex2uint(string hex);
string hex2bin(string hex);
string bin2hex(string bin);
float bin2float(string bin);
string float2bin(float num, unsigned int length);
unsigned int bin2uint(string bin);
string uint2bin(unsigned int num, unsigned int length);
string uint2hex(unsigned int num, unsigned int length);
string bin_invert(string bin);

unsigned int stateCount2bitCount(unsigned int stateCount);

}
}

#endif // CONVERT_H_
