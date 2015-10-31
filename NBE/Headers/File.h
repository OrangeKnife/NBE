#pragma once

//this is a file to read/write any kinds of files
#include "NBE.h"
#include <fstream>
#include "Exception.h"

using namespace std;
namespace NBE
{
	class FileIO
	{
	public:
		static ifstream* FileIO::readFile(const String& name )
		{
			ifstream* f= new ifstream(name,ios::binary);
			if (!f->is_open())
			{
				throw NException(ReadFileError, (name + String(TEXT(" failed"))) );
			}
			return f;
		}

		static void FileIO::readFileIntoStr(String& name, char*& str) 
		{
			ifstream* ifs = FileIO::readFile(name);		
			int count=0;

			if (ifs) {
				ifs->seekg(0,std::ios::end);
				count = (int)ifs->tellg();
				ifs->seekg(0,std::ios::beg);
				if (count > 0) 
				{
					str = new char[count+1];/*(char *)malloc(sizeof(char) * (count+1));*/
					ifs->read(str,count);
					str[count] = '\0';
				}
				delete ifs;
			}
		}

	};
	
}