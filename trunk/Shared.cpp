#include "Shared.h"

LONGLONG GetSavedFileTime( const char * filepath )
{
	DWORD dwAccess = GENERIC_READ;
	DWORD dwShare = FILE_SHARE_READ;
	DWORD dwCreate = OPEN_EXISTING;
	DWORD dwFlags = FILE_ATTRIBUTE_NORMAL;

	HANDLE fp = CreateFile( filepath, dwAccess, dwShare, NULL, dwCreate, dwFlags, NULL );
	if( fp == INVALID_HANDLE_VALUE )
		return 0;

	FILETIME writeTime;
	GetFileTime( fp, NULL, NULL, &writeTime );

	//SYSTEMTIME base_st = {
	//	1970,   // wYear
	//	1,      // wMonth
	//	0,      // wDayOfWeek
	//	1,      // wDay
	//	0,      // wHour
	//	0,      // wMinute
	//	0,      // wSecond
	//	0       // wMilliseconds
	//};

	//FILETIME base_ft;
	//SystemTimeToFileTime( &base_st, &base_ft );

	//LARGE_INTEGER itime;
	//itime.QuadPart = reinterpret_cast<LARGE_INTEGER&>( writeTime ).QuadPart;
	//itime.QuadPart -= reinterpret_cast<LARGE_INTEGER&>( base_ft ).QuadPart;
	//itime.QuadPart /= 10000000LL;

	CloseHandle( fp );

	//return itime.QuadPart;

	ULARGE_INTEGER large;
	large.HighPart = writeTime.dwHighDateTime;
	large.LowPart = writeTime.dwLowDateTime;
	return large.QuadPart;
}

void SetSavedFileTime( const char * filepath, LONGLONG timestamp )
{
	DWORD dwAccess = GENERIC_READ | GENERIC_WRITE;
	DWORD dwShare = FILE_SHARE_READ;
	DWORD dwCreate = OPEN_EXISTING;
	DWORD dwFlags = FILE_ATTRIBUTE_NORMAL;

	HANDLE fp = CreateFile( filepath, dwAccess, dwShare, NULL, dwCreate, dwFlags, NULL );
	if( fp == INVALID_HANDLE_VALUE )
		return;

	//SYSTEMTIME base_st = {
	//	1970,   // wYear
	//	1,      // wMonth
	//	0,      // wDayOfWeek
	//	1,      // wDay
	//	0,      // wHour
	//	0,      // wMinute
	//	0,      // wSecond
	//	0       // wMilliseconds
	//};

	//FILETIME base_ft;
	//SystemTimeToFileTime( &base_st, &base_ft );

	//LARGE_INTEGER itime;
	//itime.QuadPart = timestamp;
	//itime.QuadPart *= 10000000LL;
	//itime.QuadPart += reinterpret_cast<LARGE_INTEGER&>( base_ft ).QuadPart;
	////itime.QuadPart = reinterpret_cast<LARGE_INTEGER&>( writeTime ).QuadPart;

	ULARGE_INTEGER large;
	large.QuadPart = timestamp;

	FILETIME filetime;
	filetime.dwHighDateTime = large.HighPart;
	filetime.dwLowDateTime = large.LowPart;
	SetFileTime( fp, NULL, NULL, &filetime );

	CloseHandle( fp );
}
