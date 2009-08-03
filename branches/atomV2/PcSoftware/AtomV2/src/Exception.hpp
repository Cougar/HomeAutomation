/*
 * Exception.hpp
 *
 *  Created on: Apr 26, 2009
 *      Author: Mattias Runge
 */

#ifndef EXCEPTION_HPP_
#define	EXCEPTION_HPP_

#include <string>

namespace atom {

using namespace std;

class Exception
{
public:
	Exception(string description)
	{
		myDescription = description;
	}

	string getDescription()
	{
		return myDescription;
	}

private:
	string myDescription;
};

}

#endif /* _EXCEPTION_HPP */

