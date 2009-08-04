/*
 * BitBuffer.cpp
 *
 *  Created on: Jul 17, 2009
 *      Author: Mattias Runge
 */

#include "BitBuffer.h"

namespace atom {
namespace message {

BitBuffer::BitBuffer(unsigned char bytes[], unsigned int length)
{
	for (unsigned int n = 0; n < length; n++)
	{
		this->myBuffer.push_back(bytes[n]);
	}
}

BitBuffer::BitBuffer(vector<unsigned char> bytes) {
	this->setFromBytes(bytes);
}

BitBuffer::BitBuffer(string value) {
	this->setFromString(value);
}


BitBuffer::BitBuffer() {
}

BitBuffer::~BitBuffer() {
}


double BitBuffer::readDecimal(unsigned int length, unsigned int scaling)
{
}

unsigned long BitBuffer::readUnsignedInteger(unsigned int length)
{
}

bool BitBuffer::readBoolean()
{
}

long BitBuffer::readInteger(unsigned int length)
{
}

void BitBuffer::writeDecimal(unsigned int length, unsigned int scaling, double value)
{
}

void BitBuffer::writeUnsignedInteger(unsigned int length, unsigned long  value)
{
}

void BitBuffer::writeInteger(unsigned int length, long  value)
{
}

void BitBuffer::writeBoolean(bool value)
{
}

vector<unsigned char> BitBuffer::getAsBytes()
{
	return this->myBuffer;
}

void BitBuffer::setFromBytes(vector<unsigned char> bytes)
{
	this->myBuffer = bytes;
}

string BitBuffer::getAsString()
{
	string str = "";

	for (unsigned int n = 0; n < this->myBuffer.size(); n++)
	{
		str += this->myBuffer[n];
	}

	return str;
}

void BitBuffer::setFromString(string value)
{
	this->myBuffer.clear();

	for (unsigned int n = 0; n < value.length(); n++)
	{
		this->myBuffer.push_back((unsigned char) value[n]);
	}
}

}
}
