/*
 * Header.h
 *
 *  Created on: Jul 23, 2009
 *      Author: Mattias Runge
 */

#ifndef HEADER_H_
#define HEADER_H_

#include <map>
#include "Variable.h"
#include "../utils/BitBuffer.h"

namespace atom {
namespace message {

using namespace atom::utils;

class Header
{
	typedef std::map<std::string, Variable> variableMap;

public:
	Header();
	virtual ~Header();

	Variable & getVariable(string name);

	void readBits(BitBuffer & buffer);

private:
	variableMap myVariables;
};

}
}

#endif /* HEADER_H_ */
