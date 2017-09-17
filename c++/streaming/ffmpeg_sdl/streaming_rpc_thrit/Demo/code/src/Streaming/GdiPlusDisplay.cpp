//#undef WIN32_LEAN_AND_MEAN
//#include "stdafx.h"
#include "GdiPlusDisplay.h"
#include "ImageConvert.h"
#include <stdio.h>
#include "windows.h"
#include "UsedDef.h"
#ifdef GDI_PLUS
#include "gdiplus.h"
#pragma comment(lib, "gdiplus.lib")
#endif
bool GDIPlusRenderYUV420P(void* pWnd,void* pBuffer,int bufSize,int width,int height)
{
	HWND hWnd = (HWND)pWnd;
	if(hWnd == NULL || width <= 0 || height <= 0 || bufSize <= 0)
	{
		return false;
	}
	if(bufSize > width * height * 3)
	{
		return false;
	}
	
	int screen_w  = 1920;
	int screen_h = 1080;
	
	RECT clientRect;
	GetClientRect(hWnd,&clientRect);
	screen_w = clientRect.right - clientRect.left + 1;
	screen_h = clientRect.bottom - clientRect.top + 1;
	if(screen_w < 0 || screen_h < 0)
	{
		return false;
	}
	HDC hDc=GetDC(hWnd);
	//Note:
	//Big Endian or Small Endian?
	//ARGB order:high bit -> low bit.
	//ARGB Format Big Endian (low address save high MSB, here is A) in memory : A|R|G|B
	//ARGB Format Little Endian (low address save low MSB, here is B) in memory : B|G|R|A
	
	//Microsoft Windows is Little Endian
	//So we must change the order

	//YUV Need to Convert to RGB first
	//YUV420P to RGB24
	unsigned char* pBuffer_convert = new unsigned char[width*height*3];
	if(pBuffer_convert == NULL)
	{
		return false;
	}
	Simpled_CONVERT_YUV420PtoRGB24((unsigned char*)pBuffer,pBuffer_convert,width,height);

	//Change to Little Endian
	CHANGE_ENDIAN_PIC(pBuffer_convert,width,height,24);
	//BMP Header
	BITMAPINFO m_bmphdr={0};
	DWORD dwBmpHdr = sizeof(BITMAPINFO);
	//24bit
	m_bmphdr.bmiHeader.biBitCount = 24;
	m_bmphdr.bmiHeader.biClrImportant = 0;
	m_bmphdr.bmiHeader.biSize = dwBmpHdr;
	m_bmphdr.bmiHeader.biSizeImage = 0;
	m_bmphdr.bmiHeader.biWidth = width;
	//Notice: BMP storage pixel data in opposite direction of Y-axis (from bottom to top).
	//So we must set reverse biHeight to show image correctly.
	m_bmphdr.bmiHeader.biHeight = -height;
	m_bmphdr.bmiHeader.biXPelsPerMeter = 0;
	m_bmphdr.bmiHeader.biYPelsPerMeter = 0;
	m_bmphdr.bmiHeader.biClrUsed = 0;
	m_bmphdr.bmiHeader.biPlanes = 1;
	m_bmphdr.bmiHeader.biCompression = BI_RGB;
	
	#ifdef GDI_PLUS
		Gdiplus::Bitmap bmpSrc(&m_bmphdr,pBuffer_convert);  
		Gdiplus::Graphics graphDst(hDc);  
		graphDst.SetInterpolationMode(Gdiplus::InterpolationModeDefault);  
		graphDst.DrawImage(&bmpSrc, Gdiplus::RectF(0,0,screen_w,screen_h), 0, 0, width,height, Gdiplus::UnitPixel);
	#endif

	ReleaseDC(hWnd,hDc);

	if(pBuffer_convert != NULL)
	{
		delete []pBuffer_convert;
	}
	return true;
}