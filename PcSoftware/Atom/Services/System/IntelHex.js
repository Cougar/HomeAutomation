
function IntelHex(hexLines)
{
//print("constructor of IntelHex\n");
	this.myValid = false;
	this.myHexLength = 0;
	this.myAddrLower = 0x7FFFFFFF;
	this.myAddrUpper = 0;
	this.myHexData = new Array(524288);
	for (var i = 0; i < 524288; i++) this.myHexData[i] = 0xFF;

	var lineDataCount = 0;
	var lineAddrHigh = 0;
	var lineAddr = 0;
	var lineCode = 0;
	var fullAddr = 0;

	for (var n = 0; n < hexLines.length; n++)
	{
		if (hexLines[n].length>=11 && hexLines[n].substr(0,1)==":") {
			lineDataCount = hex2uint(hexLines[n].substr(1,2));
			lineAddr =  hex2uint(hexLines[n].substr(3,4)) & 65535;	 
			lineCode = hex2uint(hexLines[n].substr(7,2));   
			fullAddr = (lineAddrHigh * 65536) + lineAddr;

			switch(lineCode) 
			{
				case 0:
					this.myHexLength +=lineDataCount;
					if ((fullAddr + lineDataCount - 1) > this.myAddrUpper) this.myAddrUpper = fullAddr + lineDataCount - 1;
					if (lineCode == 0 && fullAddr < this.myAddrLower) this.myAddrLower = fullAddr;
					for (var j = 0; j < lineDataCount; j++) {
						this.myHexData[fullAddr] = hex2uint(hexLines[n].substr(j * 2 + 9, 2));
						//this.myHexData[fullAddr] &= 255;
						fullAddr++;
					}
//print("found a record of type 0, fulladdr: "+fullAddr+"\nhexLines.length: "+hexLines.length+"\n"+hexLines[n]+"\n");
					break;
				case 4:
					lineAddrHigh = hex2uint(hexLines[n].Substring(9, 4)) & 65535;
					break;
				case 1: 
//print("found a record of type 1");
					this.myValid = true;
					break; 
				
			}
		}
	}
}

IntelHex.prototype.myHexData = null;
IntelHex.prototype.myHexLength = null;
IntelHex.prototype.myAddrLower = null;
IntelHex.prototype.myAddrUpper = null;
IntelHex.prototype.myValid = null;

IntelHex.prototype.getLength = function()
{
	return this.myHexLength;
}

IntelHex.prototype.isValid = function()
{
	return this.myValid;
}

IntelHex.prototype.getAddrLower = function()
{
	return this.myAddrLower;
}

IntelHex.prototype.getByte = function(address)
{
	var byte = 0xff;
	if (address < 524288 && this.myValid) {
		byte = this.myHexData[address]; 
	}

	return byte;
}
