#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Date Version Types
	static const char DATE[] = "05";
	static const char MONTH[] = "08";
	static const char YEAR[] = "2009";
	static const double UBUNTU_VERSION_STYLE = 9.08;
	
	//Software Status
	static const char STATUS[] = "Alpha";
	static const char STATUS_SHORT[] = "a";
	
	//Standard Version Type
	static const long MAJOR = 2;
	static const long MINOR = 1;
	static const long BUILD = 129;
	static const long REVISION = 727;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT = 170;
	#define RC_FILEVERSION 2,1,129,727
	#define RC_FILEVERSION_STRING "2, 1, 129, 727\0"
	static const char FULLVERSION_STRING[] = "2.1.129.727";
	
	//SVN Version
	static const char SVN_REVISION[] = "1310";
	static const char SVN_DATE[] = "2009-08-03T22:18:47.169739Z";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY = 3;
	

}
#endif //VERSION_h
