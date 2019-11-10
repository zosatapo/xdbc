// http://msdn2.microsoft.com/en-us/library/ms686293.aspx
// To compile an application that uses SignalObjectAndWait, 
// define _WIN32_WINNT as 0x0400 or later.
#define _WIN32_WINNT   0x0502

// Prevent inclusion of winsock.h in windows.h 
#define WIN32_LEAN_AND_MEAN

// We've used macros to group together the deprecations. 
// All the insecure functions are marked _CRT_INSECURE_DEPRECATE in the headers. 
// To prevent the deprecation of these functions, you can set a special macro 
// at compile time¡ª_CRT_SECURE_NO_DEPRECATE¡ªand security deprecation warnings will stop.
#define _CRT_SECURE_NO_DEPRECATE

// Calling any one of the potentially unsafe methods in the Standard C++ Library 
// will result in Compiler Warning (level 3) C4996. To disable this warning, 
// define the macro _SCL_SECURE_NO_WARNINGS in your code:
#define _SCL_SECURE_NO_WARNINGS
#define _SCL_SECURE_NO_DEPRECATE

//================================

