#pragma once
#define CHAR_TO_WCHAR_SIZE_MAX 512
#include "NBE.h"
#include "Exception.h"

#include <memory>
#include <windows.h>
namespace NBE
{
    class TypeCast
    {
    public:
        static char* intToChar(int i)
        {
            char* intChar = new char[11];
            sprintf_s(intChar,10,"%d",i);
            return intChar;
        }

        static int stringToInt(const std::string& obj)
        {
            return atoi(obj.c_str());
        }
        static bool charToBool(const char* obj)
        {
            return memcmp("true",obj,4) == 0;
        }
		static TCHAR* charToTCHAR(const char* obj)
		{
#ifdef UNICODE
			return charToWchar(obj);
#else
			return obj;
#endif
		}

        static wchar_t* charToWchar(const char* obj)//create a new wchar_t pointer, take care
        {
            int len = (int)strlen(obj) + 1;//'\0'
            if (len > CHAR_TO_WCHAR_SIZE_MAX)
            {
                throw NException(CharToWcharOverFlow, String(TEXT("Type cast error: char to wchar over flow")));
            }
            wchar_t* strWchar(new wchar_t[CHAR_TO_WCHAR_SIZE_MAX]);
            int w_Len = MultiByteToWideChar(CP_ACP, 0, obj, len, strWchar, CHAR_TO_WCHAR_SIZE_MAX);
            return strWchar;
        }

		static char* wcharToChar(const wchar_t* obj)
		{
			size_t len = wcslen(obj);

			char* strChar = new char[len + 1];
			size_t converted;
			wcstombs_s(&converted, strChar, len + 1, obj, len + 1);
			return strChar;
		}

		static String stringToString(std::string& str)
		{
#ifdef UNICODE
			wchar_t* tempPointer = charToWchar(str.c_str());
#else
			const char* tempPointer = str.c_str();
#endif
			String rt(tempPointer);
#ifdef UNICODE
			delete tempPointer;
#endif
			return rt;
		}


		
    private:
        TypeCast();
        ~TypeCast();
    };
}