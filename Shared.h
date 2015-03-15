#include <windows.h>
#include <sys/utime.h>
#include <string>

LONGLONG GetSavedFileTime( const char * filepath );
void SetSavedFileTime( const char * filepath, LONGLONG timestamp );