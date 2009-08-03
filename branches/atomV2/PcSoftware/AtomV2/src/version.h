#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Date Version Types
	static const char DATE[] = "04";
	static const char MONTH[] = "08";
	static const char YEAR[] = "2009";
	static const double UBUNTU_VERSION_STYLE = 9.08;
	
	//Software Status
	static const char STATUS[] = "Alpha";
	static const char STATUS_SHORT[] = "a";
	
	//Standard Version Type
	static const long MAJOR = 2;
	static const long MINOR = 0;
	static const long BUILD = 26;
	static const long REVISION = 168;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT = 28;
	#define RC_FILEVERSION 2,0,26,168
	#define RC_FILEVERSION_STRING "2, 0, 26, 168\0"
	static const char FULLVERSION_STRING[] = "2.0.26.168";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY = 26;
	

}
#endif //VERSION_h
