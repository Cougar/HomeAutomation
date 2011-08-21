
/*
String.pad(length: Integer, [substring: String = " "], [type: Integer = 0]): String
      Returns the string with a substring padded on the left, right or both sides.
    length
        amount of characters that the string must have
    substring
        string that will be concatenated
    type
        specifies the side where the concatenation will happen, where: 0 = left, 1 = right and 2 = both sides
*/
String.prototype.pad = function(l, s, t)
{
  return  s || (s = " "), (l -= this.length) > 0 ? (s = new Array(Math.ceil(l / s.length)
    + 1).join(s)).substr(0, t = !t ? l : t == 1 ? 0 : Math.ceil(l / 2))
    + this + s.substr(0, l - t) : this;
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

String.prototype.html_entity_decode = function()
{
  var str = this.replace(/&#197;/g, "Ä");
  str = str.replace(/&Auml;/g, "Ä");
  
  str = str.replace(/&#196;/g, "Å");
  str = str.replace(/&Aring;/g, "Å");
  
  str = str.replace(/&#214;/g, "Ö");
  str = str.replace(/&Ouml;/g, "Ö");
  
  str = str.replace(/&#228;/g, "ä");
  str = str.replace(/&auml;/g, "ä");
  
  str = str.replace(/&#229;/g, "å");
  str = str.replace(/&aring;/g, "å");
  
  str = str.replace(/&#246;/g, "ö");
  str = str.replace(/&ouml;/g, "ö");
  
  //FIXME: Add more characters
  
  return str;
}

Date.prototype.getTimestamp = function()
{
  return this.getTime()/1000;
}

Date.prototype.getDateFormated = function()
{
  return this.getFullYear() + "-" + (this.getMonth()+1).toString().pad(2, "0", 0) + "-" + this.getDate().toString().pad(2, "0", 0);
}

Date.prototype.getTimeFormated = function()
{
  return this.getHours().toString().pad(2, "0", 0) + ":" + this.getMinutes().toString().pad(2, "0", 0) + ":" + this.getSeconds().toString().pad(2, "0", 0);
}

Date.prototype.getTimeShortFormated = function()
{
  return this.getHours().toString().pad(2, "0", 0) + ":" + this.getMinutes().toString().pad(2, "0", 0);
}

Date.prototype.getDateTimeFormated = function()
{
  return this.getDateFormated() + " " + this.getTimeFormated();
}

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

function array_remove_empty(array)
{
    var result = [];
    
    for (var n in array)
    {
        if (array[n] && (typeof array[n] == 'string' && array[n].length > 0))
        {
            result.push(array[n]);
        }
    }
    
    return result;
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

String.prototype.rpad = function(char, length)
{
    var result = this;
    
    while (result.length < length)
    {
        result += char;
    }
    
    return result;
}

function create_table(table_lines)
{
    var table_rows = [];
    
    if (table_lines.length == 0)
    {
        return table_rows;
    }
    
    for (var col = 0; col < table_lines[0].length; col++)
    {
        var len = 0;
        
        for (var row = 0; row < table_lines.length; row++)
        {
            if (table_lines[row][col].length > len)
            {
                len = table_lines[row][col].length;
            }
        }
        
        len += 6;
        
        for (var row = 0; row < table_lines.length; row++)
        {
            if (!table_rows[row])
            {
                table_rows[row] = "";
            }
            
            var str = table_lines[row][col] + '';
            str = str.rpad(" ", len);
            //Log("col=" + col + ", row=" + row + " len=" + len + ", str=\"" + str + "\", str.length=" + str.length + "\n");
            table_rows[row] += str;
        }
    }
    
    return table_rows;
}
