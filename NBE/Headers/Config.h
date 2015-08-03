#pragma once
#include "NBE.h"
#include <unordered_map>

namespace NBE
{
    class NBE_API Config
    {
    public:
        typedef std::unordered_map<std::string,std::string> strstrmap;
        Config();
        Config(const String& iniName);
        ~Config();

        void loadConfig(const String& iniName);
        const std::string& getInfo(const std::string& strName);
        const std::string& getInfo(const char*);
    private:
        strstrmap m_infoMap;
    };
}