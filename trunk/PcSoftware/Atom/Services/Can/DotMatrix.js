
function DotMatrix(type, name, id)
{
	this.CanService(type, name, id);
	this.myLastValue = new Array();
}

extend(DotMatrix, CanService);


DotMatrix.prototype.canMessageHandler = function(canMessage)
{
	var self = this;
	if (canMessage.getDirectionFlag() == "From_Owner")
	{
		switch (canMessage.getCommandName())
		{

		case "Pwm":
			break;

		}
	}
	
	this.CanService.prototype.canMessageHandler.call(this, canMessage);
}

/**
pwmValue in % with 0.01 resolution (Example: 55.25%)
*/
DotMatrix.prototype.setBrightness = function(value)
{
	var canMessage = new CanMessage("act", "To_Owner", this.myName, this.myId, "Pwm");
	canMessage.setData("Id", 0);
	canMessage.setData("Value", value*100);
	sendMessage(canMessage);
}

DotMatrix.prototype.setData = function(moduleid, row1, row2, row3, row4, row5, row6, row7, row8)
{
	var frameName = "ImageDataModule1";
	switch (moduleid)
	{
		case 1:
			frameName = "ImageDataModule1";
			break;
		case 2:
			frameName = "ImageDataModule2";
			break;
		case 3:
			frameName = "ImageDataModule3";
			break;
		case 4:
			frameName = "ImageDataModule4";
			break;
		
	}
	var canMessage = new CanMessage("act", "To_Owner", this.myName, this.myId, frameName);
	canMessage.setData("Row1", row1);
	canMessage.setData("Row2", row2);
	canMessage.setData("Row3", row3);
	canMessage.setData("Row4", row4);
	canMessage.setData("Row5", row5);
	canMessage.setData("Row6", row6);
	canMessage.setData("Row7", row7);
	canMessage.setData("Row8", row8);
	sendMessage(canMessage);
}

