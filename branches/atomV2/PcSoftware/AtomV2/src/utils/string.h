/*
 * string.h
 *
 *  Created on: Aug 04, 2009
 *      Author: Mattias Runge
 */

#ifndef STRING_H_
#define STRING_H_

#include <string>

namespace atom {
namespace string_ {

using namespace std;

void escape(string & in);
void pad_right(string & in, unsigned int length, char c);
void pad_left(string & in, unsigned int length, char c);

string escape_copy(string in);
string pad_right_copy(string in, unsigned int length, char c);
string pad_left_copy(string in, unsigned int length, char c);


}
}

#endif // STRING_H_
