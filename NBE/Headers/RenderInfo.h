#pragma once
namespace NBE
{
	struct NBE_API RenderInfo
    {
        int width;
        int height;

        bool fullScreen;
        char type;

		TCHAR  title[64];
		TCHAR  className[64];
    };
}