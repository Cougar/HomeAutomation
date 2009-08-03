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
#include "../utils/Logger.h"
#include <map>
#include "Variable.h"

using namespace std;
using namespace atom::utils;

namespace atom {
namespace message {

class Message
{
	typedef std::map<std::string, Variable> variableMap;
	typedef std::vector<std::string> responseList;

public:
	typedef boost::shared_ptr<Message> pointer;

	Message(string moduletype, unsigned int moduleId, string type);
	Message();
	virtual ~Message();

	void setOrigin(const void *origin);
	bool isOrigin(const void *origin);

	bool isFromModule();
	string getModuletype();
	unsigned int getModuleId();
	string getType();
	Variable & getVariable(string name);
	responseList getResponses();

	void readBits(BitBuffer & buffer);

private:
	Logger LOG;

	const void *myOrigin;

	bool myFromModule;
	string myModuletype;
	unsigned int myModuleId;
	string myType;
	variableMap myVariables;
	responseList myResponses;
};

}
}

#endif /* MESSAGE_H_ */
