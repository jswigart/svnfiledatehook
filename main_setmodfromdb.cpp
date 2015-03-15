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
	log.open( "file_setmods.log", std::ios_base::out | std::ios_base::trunc );

	if ( argc != 6 )
	{
		log << "Expected 7 arguments, got " << argc << std::endl;
		return 1;
	}
	
	std::string filePaths = argv[1];
	std::string fileDepth = argv[2];
	std::string	revision = argv[3];
	std::string	error = argv[4];
	std::string workingDir = argv[5];
	
	log << "File Paths " << filePaths << std::endl;
	log << "File Depth " << fileDepth << std::endl;
	log << "Revision " << revision << std::endl;
	log << "Error " << error << std::endl;
	log << "Working Directory " << workingDir << std::endl;

	std::replace( workingDir.begin(), workingDir.end(), '\\', '/' );

	sqlite3 * db = NULL;
	sqlite3_open_v2( dbFile, &db, SQLITE_OPEN_READONLY, NULL );
	if ( db == NULL )
	{
		log << "Error Opening Database " << dbFile << std::endl;
		return 1;
	}

	log << "Opened Database " << dbFile << std::endl;

	//sqlite3_exec( db, "CREATE TABLE files(file TEXT PRIMARY KEY, modified INTEGER)", NULL, NULL, NULL );

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

				sqlite3_stmt * stmt = NULL;
				if ( sqlite3_prepare( db, "SELECT * from files WHERE file=\"?\"", -1, &stmt, NULL ) == SQLITE_OK && 
					sqlite3_bind_text( stmt, 1, fileRelative, -1, NULL ) == SQLITE_OK )
				{
					if ( sqlite3_step( stmt ) == SQLITE_ROW )
					{
						const LONGLONG timestamp = sqlite3_column_int64(stmt, 0);
						SetSavedFileTime( filePathBuffer, timestamp );

						log << "Updating timestamp for " << fileRelative << " to " << timestamp << std::endl;
					}
					else
					{
						log << "No timestamp for " << fileRelative << std::endl;
					}
				}
			}
		}
	}
	
	return 0;
}
