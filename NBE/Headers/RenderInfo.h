#pragma once
namespace NBE
{
	struct NBE_API RenderInfo
    {
        int width;
        int height;

        bool fullScreen;
        char type;

        wchar_t title[64];
        wchar_t className[64];
    };
}