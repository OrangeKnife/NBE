#include "NBE.h"
#include "Config.h"
#include "TypeCast.h"
#include <fstream>
namespace NBE
{
	Config::Config()
	{}

	Config::Config(const String& iniName)
	{
		//file stream read strings
		loadConfig(iniName);
	}

	Config::~Config()
	{}

	void Config::loadConfig(const String& iniName)
	{
		std::shared_ptr<std::ifstream> f( new std::ifstream(iniName,std::ios::binary) );
		if (!f->is_open())
		{
			throw NException(ReadFileError, String(L"Fail to read ") + iniName);
		}
		
		//lambda
		auto parseFunc =
			[&](std::string& line)
		{
			if (line.c_str()[0] == '[' || line.c_str()[0] == ';')
			{
				//string tag = line.substr(1,line.find_last_not_of(']'));
				return;
			}
			else if (line.length()>1)
			{
				size_t q = line.find_first_of('=');
				size_t l = line.find_last_of('\r');
				std::string item = line.substr(0,q);
				std::string val = line.substr(q+1,l>q+1?l-(q+1):line.length()-(q+1));
				m_infoMap[item] = val;
			}
		};

		std::shared_ptr<char> buffer(new char[128]);//line length
		while (!f->eof())
		{
			f->getline(buffer.get(),128);
			parseFunc( std::string(buffer.get()));
		}
		f->close();

	}

	const std::string& Config::getInfo(const char* strName)
	{
		if ( m_infoMap.find(strName) != m_infoMap.end() )
		{
			return m_infoMap[std::string(strName)];
		}

		throw NException(CantFindConfigItem,String(L"Fail to find config item: ") + String(TypeCast::charToWchar(strName)));
	}

	const std::string& Config::getInfo(const std::string& strName)
	{
		if ( m_infoMap.find(strName) != m_infoMap.end() )
		{
			return m_infoMap[strName];
		}

		throw NException(CantFindConfigItem,String(L"Fail to find config item: ") + String(TypeCast::charToWchar(strName.c_str())));
	}
}