/*
 * Node.h
 *
 *  Created on: Apr 26, 2009
 *      Author: Mattias Runge
 */

#ifndef NODE_H_
#define NODE_H_

#include <string>
#include <map>
#include <stdexcept>
#include <utility>
#include "log/Logger.h"
#include "utils/file.h"
#include "utils/convert.h"
#include "Exception.hpp"
#include <boost/algorithm/string/trim.hpp>

namespace atom {
namespace xml {

using namespace std;
using namespace boost::algorithm;

class Node
{
public:
	typedef std::map<std::string, std::string> attributeList; // TODO Move these typedef:s out of the class
	typedef std::pair<std::string, std::string> attributePair;
	typedef std::vector<Node> nodeList;

	Node();
	Node(string filename);
	Node(string filename, string xmlData);

	void load(string filename);
	void parse(string filename, string xmlData);

	string tagName();
	attributeList & attributes();

	string operator [](string attributeName);

	Node selectFirst();
	Node selectFirst(string tagName);
	Node selectFirst(string tagName, attributeList attributes);
	Node selectFirst(string tagName, attributePair pair1);
	Node selectFirst(string tagName, attributePair pair1, attributePair pair2);
	Node selectFirst(string tagName, attributePair pair1, attributePair pair2, attributePair pair3);
	Node selectFirst(string tagName, attributePair pair1, attributePair pair2, attributePair pair3, attributePair pair4);
	Node selectFirst(string tagName, attributePair pair1, attributePair pair2, attributePair pair3, attributePair pair4, attributePair pair5);

	nodeList select();
	nodeList select(string tagName);
	nodeList select(string tagName, attributeList attributes);
	nodeList select(string tagName, attributePair pair1);
	nodeList select(string tagName, attributePair pair1, attributePair pair2);
	nodeList select(string tagName, attributePair pair1, attributePair pair2, attributePair pair3);
	nodeList select(string tagName, attributePair pair1, attributePair pair2, attributePair pair3, attributePair pair4);
	nodeList select(string tagName, attributePair pair1, attributePair pair2, attributePair pair3, attributePair pair4, attributePair pair5);

	string toString();

private:
	log::Logger LOG;

	bool myIsEmpty;
	string myTagName;
	attributeList myAttributes;
	nodeList myChildren;

	int parseTag(string filename, string xmlData, unsigned int position);
};

}
}

#endif /* NODE_H_ */
