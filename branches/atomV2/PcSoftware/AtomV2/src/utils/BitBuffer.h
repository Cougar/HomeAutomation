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

namespace atom {
namespace utils {

using namespace std;

class BitBuffer {
public:
	BitBuffer(unsigned char bytes[], unsigned int length);
	BitBuffer(vector<unsigned char> bytes);
	BitBuffer(string value);
	BitBuffer();
	virtual ~BitBuffer();

	long readInteger(unsigned int length);
	void writeInteger(unsigned int length, long value);

	unsigned long readUnsignedInteger(unsigned int length);
	void writeUnsignedInteger(unsigned int length, unsigned long value);

	double readDecimal(unsigned int length, unsigned int scaling);
	void writeDecimal(unsigned int length, unsigned int scaling, double value);

	bool readBoolean();
	void writeBoolean(bool value);

	vector<unsigned char> getAsBytes();
	void setFromBytes(vector<unsigned char> bytes);

	string getAsString();
	void setFromString(string value);

private:
	vector<unsigned char> myBuffer;
};

}
}

#endif /* BITBUFFER_H_ */
