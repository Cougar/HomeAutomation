
TextTable = function()
{

}

TextTable.create = function(tableLines)
{
	if (tableLines.length == 0)
	{
                return "";
	}
	
	var firstrow = tableLines[0];

	var tableRows = new Array();

	for (var n = 0; n < tableLines.length; n++)
	{
		tableRows[n] = "";
	}
	
        for (var col = 0; col < firstrow.length; col++)
        {
        	var len = 0;

                for (var row = 0; row < tableLines.length; row++)
                {
                	if (tableLines[row][col].length > len)
			{
                        	len = tableLines[row][col].length;
			}
                }

                len += 6;

                for (var row = 0; row < tableLines.length; row++)
                {
                	if (firstrow.length-1 == col)
			{
                        	tableRows[row] += tableLines[row][col] + '';
                        }
			else
			{
				var str = tableLines[row][col] + '';
				str = str.pad(len, ' ', 1);
                        	tableRows[row] += str;
			}
                }
	}
 
        return tableRows;
}
