
function in_array(array, item)
{
	for (var n in array)
	{
		if (array[n] == item)
		{
			return true;
		}
	}
	
	return false;
}

function key_in_array(array, key)
{
	for (var n in array)
	{
		if (n == key)
		{
			return true;
		}
	}
	
	return false;
}

String.prototype.ltrim = function(charlist)
{
	charlist = !charlist ? ' \s\xA0' : charlist.replace(/([\[\]\(\)\.\?\/\*\{\}\+\$\^\:])/g, '\$1');
	var re = new RegExp('^[' + charlist + ']+', 'g');
	return this.replace(re, '');
}

String.prototype.rtrim = function(charlist)
{
	charlist = !charlist ? ' \s\xA0' : charlist.replace(/([\[\]\(\)\.\?\/\*\{\}\+\$\^\:])/g, '\$1');
	var re = new RegExp('[' + charlist + ']+$', 'g');
	return this.replace(re, '');
}

String.prototype.trim = function(charlist)
{
	var str = this.ltrim(charlist);
	return str.rtrim(charlist);
}
