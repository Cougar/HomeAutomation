/*
 * file.h
 *
 *  Created on: Aug 04, 2009
 *      Author: Mattias Runge
 */

#ifndef FILE_H_
#define FILE_H_

#include <fstream>
#include <string>

namespace atom {
namespace file {

using namespace std;

string get_contents(string filename);
bool exists(string filename);

}
}

#endif // FILE_H_
