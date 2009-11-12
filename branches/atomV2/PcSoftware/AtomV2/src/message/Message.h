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

namespace atom {

//using namespace boost::algorithm;

class Message
{
public:
	typedef boost::shared_ptr<Message> Pointer;

	Message(const void *origin);
	Message();
	virtual ~Message();

	const void *GetOrigin() const;

	Header & getHeader();
	Variable & getVariable(string name);
	variableMap & getVariables();
	responseList getResponses();

	string toString();

private:
	typedef std::map<std::string, Variable> variableMap;
	typedef std::vector<std::string> responseList;


	log::Logger LOG;

	const void *myOrigin;

	Header myHeader;
	variableMap myVariables;
	responseList myResponses;
};

} // namespace atom

#endif /* MESSAGE_H_ */
