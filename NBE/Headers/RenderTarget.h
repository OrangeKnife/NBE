#pragma once
#define uint unsigned int
namespace NBE
{
	struct RenderTarget
	{
		RenderTarget(int _w,int _h, int rt = -1,int tex = -1):
			rtIdx(rt),texIdx(tex),w(_w),h(_h)
		{}

		int rtIdx;
		int texIdx;//color
		int w,h;
	};
}