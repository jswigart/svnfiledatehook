#include <iostream>
#include <fstream>
#include <stdio.h>
#include <algorithm>

#include "Shared.h"
#include "sqlite3.h"

int main(int argc, char**argv)
{
	const char * dbFile = "file_mods.db";

	std::fstream log;
	log.open( "file_getmods.log", std::ios_base::out | std::ios_base::trunc );

	if ( argc != 5 )
	{
		log << "Expected 5 arguments, got " << argc << std::endl;
		return 1;
	}
	
	std::string filePaths = argv[1];
	std::string fileDepth = argv[2];
	std::string fileMessage = argv[3];
	std::string workingDir = argv[4];
	
	log << "File Paths " << filePaths << std::endl;
	log << "File Depth " << fileDepth << std::endl;
	log << "File Msg " << fileMessage << std::endl;
	log << "Working Directory " << workingDir << std::endl;

	std::replace( workingDir.begin(), workingDir.end(), '\\', '/' );

	sqlite3 * db = NULL;
	sqlite3_open_v2( dbFile, &db, SQLITE_OPEN_READWRITE|SQLITE_OPEN_CREATE, NULL );
	if ( db == NULL )
	{
		log << "Error Opening Database " << dbFile << std::endl;
		return 1;
	}

	log << "Opened Database " << dbFile << std::endl;

	sqlite3_exec( db, "CREATE TABLE files(file TEXT PRIMARY KEY, modified INTEGER)", NULL, NULL, NULL );

	std::fstream fileList;
	fileList.open( filePaths, std::ios_base::in );
	if ( fileList.is_open() )
	{
		while ( !fileList.eof() )
		{
			enum { BufferSz=2048 };
			char filePathBuffer[BufferSz] = {};
			fileList.getline( filePathBuffer, BufferSz );
			
			log << "FilePath: " << filePathBuffer << std::endl;

			const char * fileRelative = filePathBuffer;
			if ( !_strnicmp( filePathBuffer, workingDir.c_str(), workingDir.length() ))
				fileRelative += workingDir.length();

			if ( fileRelative[ 0 ] != NULL )
			{
				log << "FilePath Relative: " << fileRelative << std::endl;
				
				const LONGLONG timestamp = GetSavedFileTime( filePathBuffer );
				
				char sqlBuffer[BufferSz] = {};
				_snprintf_s( sqlBuffer, BufferSz, "INSERT OR IGNORE INTO files VALUES (\"%s\", %lld)", fileRelative, timestamp );
				sqlite3_exec( db, sqlBuffer, NULL, NULL, NULL );
				
				_snprintf_s( sqlBuffer, BufferSz, "UPDATE files SET file=\"%s\", modified=%lld WHERE file=\"%s\"", fileRelative, timestamp, fileRelative );
				sqlite3_exec( db, sqlBuffer, NULL, NULL, NULL );
			}
		}
	}

	return 0;
}
