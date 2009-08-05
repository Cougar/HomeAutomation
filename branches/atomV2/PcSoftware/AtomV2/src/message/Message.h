/*
 * Message.h
 *
 *  Created on: Apr 25, 2009
 *      Author: Mattias Runge
 */

#ifndef MESSAGE_H_
#define MESSAGE_H_

#include <string>
#include <vector>
#include <map>
#include <boost/algorithm/string/trim.hpp>
#include "log/Logger.h"
#include "message/Variable.h"
#include "message/Header.h"
#include "protocol/Protocol.h"
#include "utils/convert.h"

using namespace std;

namespace atom {
namespace message {

using namespace boost::algorithm;

class Message
{
	typedef std::map<std::string, Variable> variableMap;
	typedef std::vector<std::string> responseList;

public:
	typedef boost::shared_ptr<Message> pointer;

	Message(Header header);
	Message();
	virtual ~Message();

	void setOrigin(const void *origin);
	bool isOrigin(const void *origin);

	Header & getHeader();
	Variable & getVariable(string name);
	variableMap & getVariables();
	responseList getResponses();

	void readBits(BitBuffer & buffer);
	void writeBits(BitBuffer & buffeer);

	string toString();

private:
	log::Logger LOG;

	const void *myOrigin;

	Header myHeader;
	variableMap myVariables;
	responseList myResponses;
};

}
}

#endif /* MESSAGE_H_ */
