
const MAX_SIZE = 524288;

/* Call constructor with the contents of an Intel Hex file (rows as arrays) 
   http://en.wikipedia.org/wiki/.hex
*/
function IntelHex(hexLines)
{
	this.myValid = false;
	this.myAddrLower = 0x7FFFFFFF;
	this.myAddrUpper = 0;
	this.myHexData = new Array(MAX_SIZE);
	for (var i = 0; i < MAX_SIZE; i++) this.myHexData[i] = 0xFF;

	var lineDataCount = 0;
	var lineAddrHigh = 0;
	var lineAddr = 0;
	var lineCode = 0;
	var fullAddr = 0;
	
	/* Parse hexfile to binary (array of bytes) */
	for (var n = 0; n < hexLines.length; n++)
	{
		if (hexLines[n].length>=11 && hexLines[n].substr(0,1)==":") {
			lineDataCount = hex2uint(hexLines[n].substr(1,2));		/* Byte count */
			lineAddr =  hex2uint(hexLines[n].substr(3,4)) & 65535;	/* Address */
			lineCode = hex2uint(hexLines[n].substr(7,2));			/* Record type */
			fullAddr = (lineAddrHigh * 65536) + lineAddr;

			switch(lineCode)
			{
				case 0:	/* If Data record */
					if ((fullAddr + lineDataCount - 1) > this.myAddrUpper) this.myAddrUpper = fullAddr + lineDataCount - 1;
					if (lineCode == 0 && fullAddr < this.myAddrLower) this.myAddrLower = fullAddr;
					for (var j = 0; j < lineDataCount; j++) {
						this.myHexData[fullAddr] = hex2uint(hexLines[n].substr(j * 2 + 9, 2));
						fullAddr++;
					}
					break;
				case 4:	/* If Extended Linear Address Record */
					lineAddrHigh = hex2uint(hexLines[n].substr(9, 4)) & 65535;
					break;
				case 1:	/* If End Of File record */
					this.myValid = true;
					break; 
				
			}
		}
	}
}

IntelHex.prototype.myHexData = null;
IntelHex.prototype.myAddrLower = null;
IntelHex.prototype.myAddrUpper = null;
IntelHex.prototype.myValid = null;

/* Get the CRC of data, includes "holes" in the middle */
IntelHex.prototype.getCRC16 = function()
{
	var crc=0;
	for (var i = this.myAddrLower; i <= this.myAddrUpper; i++)
	{
		crc = this.crc16_update(crc, this.myHexData[i]);
	}
	return crc&0xffff;
}

IntelHex.prototype.crc16_update = function(crc, byte)
{
	crc ^= byte;
	for (var i = 0; i<8; ++i)
	{
		if ((crc & 1) != 0)
		{
			crc = (crc >> 1) ^ 0xA001;
		}
		else
		{
			crc = (crc >> 1);
		}
	}
	return crc;
}

/* Get the length of the data */
IntelHex.prototype.getLength = function()
{
	if (this.myAddrUpper+1>this.myAddrLower)
	{
		return (this.myAddrUpper - this.myAddrLower + 1);
	}
	else
	{
		return 0;
	}
}

/* Get the validity of the data (check if end of file record was found) */
IntelHex.prototype.isValid = function()
{
	return this.myValid;
}

/* Get the lowest address of the data */
IntelHex.prototype.getAddrLower = function()
{
	return this.myAddrLower;
}

/* Get the highest address of the data */
IntelHex.prototype.getAddrUpper = function()
{
	return this.myAddrUpper;
}

/* Get a byte at a certain address */
IntelHex.prototype.getByte = function(address)
{
	var byte = 0xff;
	if (address < MAX_SIZE && this.myValid) {
		byte = this.myHexData[address]; 
	}

	return byte;
}
