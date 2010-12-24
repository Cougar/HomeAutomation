
function get_time()
{
	return Math.round(new Date().getTime() / 1000);
}

function to_array(obj)
{
	var result = [];
	
	for (var n = 0; n < obj.length; n++)
	{
		result.push(obj[n]);
	}
	
	return result;
}

function array_length(array)
{
    var count = 0;

    for (var n in array)
    {
        count++;
    }

    return count;
}

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

function get_keys(array)
{
	var result = [];
	
	for (var name in array)
	{
		result.push(name);
	}
	
	return result;
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
