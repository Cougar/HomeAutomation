/*
 * Header.h
 *
 *  Created on: Jul 23, 2009
 *      Author: Mattias Runge
 */

#ifndef HEADER_H_
#define HEADER_H_

#include <map>
#include <boost/algorithm/string/trim.hpp>
#include "message/Variable.h"
#include "message/BitBuffer.h"
#include "protocol/Protocol.h"
#include "utils/convert.h"
#include "Exception.hpp"
#include "log/Logger.h"

namespace atom {
namespace message {

using namespace std;
using namespace boost::algorithm;

class Header
{
	typedef std::map<std::string, Variable> variableMap;

public:
	Header();
	Header(BitBuffer & buffer);
	Header(unsigned long moduleId, string moduleType, string messageType);
	virtual ~Header();

	unsigned long getModuleId();
	string getModuleType();
	string getMessageType();

	void readBits(BitBuffer & buffer);
	void writeBits(BitBuffer & buffer);

	string toString();

private:
	log::Logger LOG;

	variableMap myVariables;

	void loadVariables();
};

}
}

#endif /* HEADER_H_ */
