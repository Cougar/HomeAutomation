/*
 * BitBuffer.h
 *
 *  Created on: Jul 17, 2009
 *      Author: Mattias Runge
 */

#ifndef BITBUFFER_H_
#define BITBUFFER_H_

#include <vector>
#include <string>
#include "types.h"

namespace atom {
namespace message {

using namespace std;

class BitBuffer {
public:
	BitBuffer(unsigned char bytes[], unsigned int length);
	BitBuffer(byte_list bytes);
	BitBuffer();
	virtual ~BitBuffer();

	byte_list getAsBytes();
	void setFromBytes(vector<unsigned char> bytes);

	void incrementPointer(unsigned int steps);
	void clear();

	void read(unsigned int length, long & value);
	void read(unsigned int length, unsigned long & value);
	void read(unsigned int length, bool & value);

	void write(unsigned int length, long value);
	void write(unsigned int length, unsigned long value);
	void write(unsigned int length, bool value);

private:
	byte_list myBuffer;
};

}
}

#endif /* BITBUFFER_H_ */
