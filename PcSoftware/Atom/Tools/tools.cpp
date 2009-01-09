
#include <string>

/***************************************************************************
 *   Copyright (C) 2004 by Mattias Runge                                   *
 *   mattias@mrunge.se                                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "tools.h"

string niceTime()
{
	string result;
	struct tm tmStruct;
	time_t t = time(NULL);
	localtime_r(&t, &tmStruct);

	result += lpad(itos(tmStruct.tm_hour), 2, '0');
	result += ":";
	result += lpad(itos(tmStruct.tm_min), 2, '0');
	result += ":";
	result += lpad(itos(tmStruct.tm_sec), 2, '0');

	return result;
}

unsigned int hex2uint(string hex)
{
	return bin2uint(hex2bin(hex));
}

unsigned int bin2uint(string bin)
{
	unsigned int num = 0;

	for (int n = bin.length()-1; n >= 0; n--)
	{
		if (bin[n] == '1')
			num += (unsigned int)pow(2.0f, (int)(bin.length()-1-n));
	}

	return num;
}

string invert(string bin)
{
	string inverted;

	for (int n = 0; n < bin.length(); n++)
		inverted += (bin[n] == '1' ? '0' : '1');

	return inverted;
}

float bin2float(string bin)
{
	float num = 0;

	if (bin.length() == 0)
		return 0.0f;

	bool isNegative = false;;

	if (bin[0] == '1')
	{
		bin = invert(bin);
		isNegative = true;
	}

	for (int n = bin.length()-1; n > 0; n--)
	{
		if (bin[n] == '1')
			num += pow(2.0f, (int)(bin.length()-1-n));
	}

	num /= 64.0f;

	if (isNegative)
		num = -num;

	return num;
}

string bin2hex(string bin)
{
	string hex;

	while (bin.size() > 0)
	{
		string part;

		try
		{
			part = bin.substr(0, 4);
		}
		catch (std::out_of_range& e)
		{
			cout << "DEBUG: bin2hex exception: " << e.what() << "\n";
			cout << "DEBUG: A bin=" << bin << endl;
			continue;
		}
		
		bin.erase(0, part.size());

		while (part.size() < 4)
			part = "0" + part;

		if (part == "0000")
			hex += '0';
		else if (part == "0001")
			hex += '1';
		else if (part == "0010")
			hex += '2';
		else if (part == "0011")
			hex += '3';
		else if (part == "0100")
			hex += '4';
		else if (part == "0101")
			hex += '5';
		else if (part == "0110")
			hex += '6';
		else if (part == "0111")
			hex += '7';
		else if (part == "1000")
			hex += '8';
		else if (part == "1001")
			hex += '9';
		else if (part == "1010")
			hex += 'a';
		else if (part == "1011")
			hex += 'b';
		else if (part == "1100")
			hex += 'c';
		else if (part == "1101")
			hex += 'd';
		else if (part == "1110")
			hex += 'e';
		else if (part == "1111")
			hex += 'f';
	}

	return hex;
}

string float2bin(float num, int length)
{
	string bin;

	///FIXME

	while (bin.size() < length)
		bin = "0" + bin;

	return bin;
}

string hex2bin(string hex)
{
	hex = strtolower(hex);

	string bin;

	for (int n = 0; n < hex.length(); n++)
	{
		switch (hex[n])
		{
			case '0': bin += "0000"; break;
			case '1': bin += "0001"; break;
			case '2': bin += "0010"; break;
			case '3': bin += "0011"; break;
			case '4': bin += "0100"; break;
			case '5': bin += "0101"; break;
			case '6': bin += "0110"; break;
			case '7': bin += "0111"; break;
			case '8': bin += "1000"; break;
			case '9': bin += "1001"; break;
			case 'a': bin += "1010"; break;
			case 'b': bin += "1011"; break;
			case 'c': bin += "1100"; break;
			case 'd': bin += "1101"; break;
			case 'e': bin += "1110"; break;
			case 'f': bin += "1111"; break;
		}
	}

	return bin;
}

string uint2bin(unsigned int num, int length)
{
	string bin;

	while (num)
	{
		bin = char((num&1)+'0') + bin;
		num >>= 1;
	}

	while (bin.size() < length)
		bin = "0" + bin;

	return bin;
}

string uint2hex(unsigned int num, int length)
{
	return bin2hex(uint2bin(num, length));
}

vector<string> explode(string delimiter, string str)
{
	vector<string> parts;
	string::size_type startPos = 0;
	string::size_type endPos = 0;

	while ((endPos = str.find(delimiter, startPos)) != string::npos)
	{
		try
		{
			parts.push_back(str.substr(startPos, endPos-startPos));
		}
		catch (std::out_of_range& e)
		{
			cout << "DEBUG: explode exception: " << e.what() << "\n";
			cout << "DEBUG: A str=" << str << " :: startPos=" << startPos << " :: endPos=" << endPos << endl;
			continue;
		}
		
		startPos = endPos+delimiter.length();
	}

	if (startPos < str.length())
	{
		try
		{
			parts.push_back(str.substr(startPos, str.length()-startPos));
		}
		catch (std::out_of_range& e)
		{
			cout << "DEBUG: explode exception: " << e.what() << "\n";
			cout << "DEBUG: B str=" << str << " :: startPos=" << startPos << " :: endPos=" << endPos << endl;
		}
	}

	return parts;
}

string strtoupper(string s)
{
	for (unsigned int n = 0; n < s.size(); n++)
		s[n] = (char)toupper(s[n]);

	return s;
}

string strtolower(string s)
{
	for (unsigned int n = 0; n < s.size(); n++)
		s[n] = (char)tolower(s[n]);

	return s;
}

string trim(string s)
{
	return trim(s, ' ');
}

string trim(const string s, char c)
{
	string result = s;
	while (result[0] == c)
		result.erase(0, 1);

	while (result[result.length()-1] == c)
		result.erase(result.length()-1, 1);

	return result;
}

int stoi(const string s)
{
	istringstream in(s);
	int i;
	in >> i;
	return i;
}

string itos(int i)
{
	ostringstream out;
	out << i;
	return out.str();
}

unsigned int stou(const string s)
{
	istringstream in(s);
	unsigned int i;
	in >> i;
	return i;
}

string utos(unsigned int i)
{
	ostringstream out;
	out << i;
	return out.str();
}

float stof(const string s)
{
	istringstream in(s);
	float f;
	in >> f;
	return f;
}

string ftos(float f)
{
	ostringstream out;
	out << f;
	return out.str();
}

string str_replace(string search, string replace, string str)
{
	vector<string> parts = explode(search, str);

	string result;
	for (int n = 0; n < parts.size(); n++)
	{
		result += parts[n];

		if (n < parts.size()-1)
			result += replace;
	}

	return result;
}

string file_get_contents(string filename)
{
	ifstream srcfile(filename.c_str());

	if (!srcfile.is_open())
	{
		srcfile.close();
		return "";
	}

	string buffer = "";
	string line;

	while (!srcfile.eof())
	{
		getline(srcfile, line);

		if (srcfile.eof())
			break;
		
		buffer += line + "\n";
	};

	srcfile.close();

	return buffer;
}

bool file_exists(string filename)
{
	ifstream file(filename.c_str());
	if (file.is_open())
	{
		file.close();
		return true;
	}

	return false;
}

string escape(string in)
{
	string out;

	for (int n = 0; n < in.length(); n++)
	{
		switch (in[n])
		{
			case '\r':
				break;

			case '\n':
				out += "\\n";
				break;

			case '\'':
				out += "\\'";
				break;

			case '"':
				out += "\\\"";
				break;

			default:
				out += in[n];
				break;
		}
	}

	return out;
}

string rpad(string in, int length, char c)
{
	while (in.length() < length)
	{
		in += c;
	}

	return in;
}

string lpad(string in, int length, char c)
{
	while (in.length() < length)
	{
		in = c + in;
	}

	return in;
}


// FileTools

vector<string> FileTools::GetDirList(string path)
{
	DIR *dir = opendir(path.c_str());
	vector<string> list;

	if (!dir)
		return list;

	dirent *d;
	while ((d = readdir(dir)))
	{
		if (d->d_name[0] == '.')
			continue;

		list.push_back(d->d_name);
	}
	closedir(dir);

	return list;
}

string FileTools::GetUniqeFilename(string path, string prefix, string postfix)
{
	time_t now;
	time(&now);

	string filename = path + prefix + itos(now);

	int index = 0;
	while (1)
	{
		string file = filename + itos(index) + postfix;
		if (!Exist(file))
			break;
		index++;
	}

	filename += itos(index) + postfix;

	return filename;
}

bool FileTools::SaveFile(string filepath, string data)
{
	ofstream destfile(filepath.c_str());
	if (!destfile.is_open())
	{
		destfile.close();
		return false;
	}

	destfile << data;

	destfile.close();
	return true;
}

string FileTools::Get(string src)
{
	ifstream srcfile(src.c_str());
	if (!srcfile.is_open())
	{
		srcfile.close();
		return "";
	}

	string buffer = "";
	string line;
	while (!srcfile.eof())
	{
		getline(srcfile, line);
		if (srcfile.eof())
			break;
		buffer += line + "\n";
	};

	srcfile.close();

	return buffer;
}

bool FileTools::Copy(string src, string dest)
{
	ifstream srcfile(src.c_str());
	if (!srcfile.is_open())
	{
		srcfile.close();
		return false;
	}

	ofstream destfile(dest.c_str());
	if (!destfile.is_open())
	{
		destfile.close();
		srcfile.close();
		return false;
	}

	string line;
	while (!srcfile.eof())
	{
		getline(srcfile, line);
		if (srcfile.eof())
			break;
		destfile << line << endl;
	};

	destfile.close();
	srcfile.close();

	return true;
}

bool FileTools::Exist(string filename)
{
	ifstream file(filename.c_str());
	if (file.is_open())
	{
		file.close();
		return true;
	}

	return false;
}





/*
void bd()
{
         int n,b=0,a[6],i=0,t=0;
         clrscr();
         printf("Conversion from Binary to Decimal\n");
         printf("Enter Binary Number\n");
         scanf("%d",&n);
         while(n!=0)
         {
          a[i]=n%10;
          n=n/10;
          if(a[i]!=1 && a[i]!=0)
           t=1;
          i++;
         }
         a[i]=2;
         n=1;
         for(i=0;a[i]!=2;i++)
         {
          b=b+a[i]*n;
          n=n*2;
         }
         if(t==0)
          printf("Decimal Equivalent=%d",B);
         else if(t==1)
          printf("Entered number is not binary.");

}

void db()
{
         int dec,bin,n,i=0,a[10];
         clrscr();
         printf("Conversion from Decimal to Binary\n");
         printf("Input decimal no.");
         scanf("%d",&dec);
         do
         {
         a[i]=dec%2;
         dec=dec/2;
         i++;
         }while(dec!=0);
         for(n=i-1;n>=0;n--)
         printf("%d",a[n]);
}

void doc()
{
         int n,i,a[10];
         clrscr();
         printf("Conversion from Decimal to Octal\n");
         printf("Enter a Decimal number\n");
         scanf("%d",&n);
         i=0;
         printf("Octal equavalent of %d is ",n);
         while(n!=0)
         {
          a[i]=n%8;
          n=n/8;
          i++;
         }
         i--;
         for(;i>=0;i--)
          printf("%d",a[i]);
}

void dh(long n)
{
         long i;
         if(n>0)
         {
          i=n%16;
          n=n/16;
          dh(n);
          if(i>=10)
          {
           switch(i)
           {
            case 10:
             printf("A");
             break;
            case 11:
             printf("B");
             break;
            case 12:
             printf("C");
             break;
            case 13:
             printf("D");
             break;
            case 14:
             printf("E");
             break;
            case 15:
             printf("F");
             break;
           }
          }
          else
           printf("%ld",i);
         }
}

void od()
{
         unsigned long n,i=0,a,p=1,t=0;
         clrscr();
         printf("Conversion from Octal to Decimal\n");
         printf("Enter a Octal number\n");
         scanf("%ld",&n);
         i=0;
         printf("Decimal equavalent of %ld",n);
         while(n!=0)
         {
          a=n%10;
          if(a>7)
           t=1;
          n=n/10;
          i=i+a*p;
          p=p*8;
         }
         if(t==0)
          printf("= %ld",i);
         else if(t==1)
          printf(" can't be calculated because it is not an Octal Number.\n");
}

void ob()
{
         int n,a[6],i=0,t=0;
         clrscr();
         printf("Convertion from Octal to Binary.\n");
         printf("Enter an Octal Number.\n");
         scanf("%d",&n);
         while(n!=0)
         {
          a[i]=n%10;
          n=n/10;
          if(a[i]>7)
           t=1;
          i++;
         }
         i--;
         if(t==0)
          for(;i>=0;i--)
          {
           switch(a[i])
           {
            case 0:
             printf("000");
             break;
            case 1:
             printf("001");
             break;
            case 2:
             printf("010");
             break;
            case 3:
             printf("011");
             break;
            case 4:
             printf("100");
             break;
            case 5:
             printf("101");
             break;
            case 6:
             printf("110");
             break;
            case 7:
             printf("111");
             break;
           }
          }
         if(t==1)
          printf("Not a Octal number\n");
}

void bo()
{
         int i=0,a[5],t=0;
         long int n;
         clrscr();
         printf("Convertion From Binary to Octal\n");
         printf("Enter a Binary number\n");
         scanf("%ld",&n);
         while(n!=0)
         {
          a[i]=n%1000;
          n=n/1000;
          if(a[i]>111)
           t=1;
          i++;
         }
         i--;
         if(t==0)
          for(;i>=0;i--)
          {
           switch(a[i])
           {
            case 0:
             printf("0");
             break;
            case 1:
             printf("1");
             break;
            case 10:
             printf("2");
             break;
            case 11:
             printf("3");
             break;
            case 100:
             printf("4");
             break;
            case 101:
             printf("5");
             break;
            case 110:
             printf("6");
             break;
            case 111:
             printf("7");
             break;
            default:
   printf("\nEntered number is not binary.\nPrinted value is not correct.\n");
             break;
           }
          }
         if(t==1)
          printf("Number is not Binary\n");
}

void bh()
{
         int i=0,a[5],t=0;
         long int n;
         clrscr();
         printf("Convertion from Binary to Hexadecimal\n");
         printf("Enter a Binary number\n");
         scanf("%ld",&n);
         while(n!=0)
         {
          a[i]=n%10000;
          n=n/10000;
          if(a[i]>1111)
           t=1;
          i++;
         }
         i--;
         if(t==0)
          for(;i>=0;i--)
          {
           switch(a[i])
           {
            case 0:
             printf("0");
             break;
            case 1:
             printf("1");
             break;
            case 10:
             printf("2");
             break;
            case 11:
             printf("3");
             break;
            case 100:
             printf("4");
             break;
            case 101:
             printf("5");
             break;
            case 110:
             printf("6");
             break;
            case 111:
             printf("7");
             break;
            case 1000:
             printf("8");
             break;
            case 1001:
             printf("9");
             break;
            case 1010:
             printf("A");
             break;
            case 1011:
             printf("B");
             break;
            case 1100:
             printf("C");
             break;
            case 1101:
             printf("D");
             break;
            case 1110:
             printf("E");
             break;
            case 1111:
             printf("F");
             break;
            default:
     printf("\nEntered number is not binary.\nPrinted value is not correct.\n");
             break;
           }
          }
         if(t==1)
          printf("Number is not Binary\n");
}

void hb()
{
         int i;
         char s[20];
         clrscr();
         printf("Convertion from Hexadecimal to Binary\n");
         printf("Enter a Hexadecimal number\n");
         scanf("%s",s);
         //gets(s);
         printf("Binary Equivalent=");
         for(i=0;s[i]!=NULL;i++)
         {
          switch(s[i])
           {
            case '0':
             printf("0000");
             break;
            case '1':
             printf("0001");
             break;
            case '2':
             printf("0010");
             break;
            case '3':
             printf("0011");
             break;
            case '4':
             printf("0100");
             break;
            case '5':
             printf("0101");
             break;
            case '6':
             printf("0110");
             break;
            case '7':
             printf("0111");
             break;
            case '8':
             printf("1000");
             break;
            case '9':
             printf("1001");
             break;
            case 'a':
            case 'A':
             printf("1010");
             break;
            case 'b':
            case 'B':
             printf("1011");
             break;
            case 'c':
            case 'C':
             printf("1100");
             break;
            case 'd':
            case 'D':
             printf("1101");
             break;
            case 'e':
            case 'E':
             printf("1110");
             break;
            case 'f':
            case 'F':
             printf("1111");
             break;
            default:
       printf("\nEntered number is not Hexadecimal.\nPrinted value is not correct.\n");
             break;
           }
          }
}

void hd()
{
         int i,a[20];
         unsigned long int h=0,m=1;
         char s[20];
         clrscr();
         printf("Convertion from Hexadecimal to Decimal\n");
         printf("Enter a Hexadecimal number\n");
         scanf("%s",s);
         printf("Decimal Equivalent=");
         for(i=0;s[i]!=NULL;i++)
         {
          switch(s[i])
           {
            case '0':
             a[i]=0;
             break;
            case '1':
             a[i]=1;
             break;
            case '2':
             a[i]=2;
             break;
            case '3':
             a[i]=3;
             break;
            case '4':
             a[i]=4;
             break;
            case '5':
             a[i]=5;
             break;
            case '6':
             a[i]=6;
             break;
            case '7':
             a[i]=7;
             break;
            case '8':
             a[i]=8;
             break;
            case '9':
             a[i]=9;
             break;
            case 'a':
            case 'A':
             a[i]=10;
             break;
            case 'b':
            case 'B':
             a[i]=11;
             break;
            case 'c':
            case 'C':
             a[i]=12;
             break;
            case 'd':
            case 'D':
             a[i]=13;
             break;
            case 'e':
            case 'E':
             a[i]=14;
             break;
            case 'f':
            case 'F':
             a[i]=15;
             break;
            default:
    printf("\nEntered number is not Hexadecimal.\nPrinted value is not correct.\n");
             break;
           }
         }
         i--;
         for(;i>=0;i--)
         {
          h=h+a[i]*m;
          m=m*16;
         }
          printf("%ld ",h);
}

void oh(long n)
{
         long i;
         if(n>0)
         {
          i=n%16;
          n=n/16;
          oh(n);
          if(i>=10)
          {
           switch(i)
           {
            case 10:
             printf("A");
             break;
            case 11:
             printf("B");
             break;
            case 12:
             printf("C");
             break;
            case 13:
             printf("D");
             break;
            case 14:
             printf("E");
             break;
            case 15:
             printf("F");
             break;
           }
          }
          else
           printf("%ld",i);
         }
}

void ho()
{
         int i,a[20];
         unsigned long int h=0,m=1;
         char s[20];
         clrscr();
         printf("Convertion from Hexadecimal to Octal\n");
         printf("Enter a Hexadecimal number\n");
         scanf("%s",s);
         // Converting hex to dec first
         for(i=0;s[i]!=NULL;i++)
         {
          switch(s[i])
           {
            case '0':
             a[i]=0;
             break;
            case '1':
             a[i]=1;
             break;
            case '2':
             a[i]=2;
             break;
            case '3':
             a[i]=3;
             break;
            case '4':
             a[i]=4;
             break;
            case '5':
             a[i]=5;
             break;
            case '6':
             a[i]=6;
             break;
            case '7':
             a[i]=7;
             break;
            case '8':
             a[i]=8;
             break;
            case '9':
             a[i]=9;
             break;
            case 'a':
            case 'A':
             a[i]=10;
             break;
            case 'b':
            case 'B':
             a[i]=11;
             break;
            case 'c':
            case 'C':
             a[i]=12;
             break;
            case 'd':
            case 'D':
             a[i]=13;
             break;
            case 'e':
            case 'E':
             a[i]=14;
             break;
            case 'f':
            case 'F':
             a[i]=15;
             break;
            default:
      printf("\nEntered number is not Hexadecimal.\nPrinted value is not correct.\n");
             break;
           }
         }
         i--;
         for(;i>=0;i--)
         {
          h=h+a[i]*m;
          m=m*16;
         }
         // Now convering from decimal to octal (h)
         i=0;
         printf("Octal equavalent=");
         while(h!=0)
         {
          a[i]=h%8;
          h=h/8;
          i++;
         }
         i--;
         for(;i>=0;i--)
          printf("%d",a[i]);
}*/
