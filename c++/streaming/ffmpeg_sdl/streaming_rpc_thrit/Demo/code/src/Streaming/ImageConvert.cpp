#include "ImageConvert.h"
/**
 * 最简单的GDI播放视频的例子（GDI播放RGB/YUV）
 * Simplest Video Play GDI (GDI play RGB/YUV) 
 *
 * 雷霄骅 Lei Xiaohua
 * leixiaohua1020@126.com
 * 中国传媒大学/数字电视技术
 * Communication University of China / Digital TV Technology
 * http://blog.csdn.net/leixiaohua1020
 *
 * 本程序使用GDI播放RGB/YUV视频像素数据。GDI实际上只能直接播放RGB数据。
 * 因此如果输入数据为YUV420P的话，需要先转换为RGB数据之后再进行播放。
 * 
 * 函数调用步骤如下：
 * GetDC()：获得显示设备的句柄。
 * 像素数据格式的转换（如果需要的话）
 * 设置BMP文件头...
 * StretchDIBits()：指定BMP文件头，以及像素数据，绘制。
 * ReleaseDC()：释放显示设备的句柄。
 *
 * 在该示例程序中，包含了像素转换的几个工具函数，以及“大端”，
 * “小端”（字节顺序）相互转换的函数。
 *
 * This software plays RGB/YUV raw video data using GDI.
 * In fact GDI only can draw RGB data. So If input data is
 * YUV420P, it need to be convert to RGB first.
 * It's the simplest GDI tutorial (About video playback).
 *
 * The process is shown as follows:
 *
 * GetDC()：retrieves a handle to a device context (DC).
 * Convert pixel data format(if needed).
 * Set BMP Header...
 * StretchDIBits()：Set pixel data and BMP data and begin to draw.
 * ReleaseDC()：release the handle.
 *
 * In this program there are some functions about conversion
 * between pixel format and conversion between "Big Endian" and 
 * "Little Endian".
 */
//#include "stdafx.h"
#include <stdio.h>
#include <string.h>
//#include <tchar.h>
//#include <Windows.h>
#include <stdlib.h>

//Not Efficient, Just an example
//change endian of a pixel (32bit)
void CHANGE_ENDIAN_32(unsigned char *data){
	char temp3,temp2;
	temp3=data[3];
	temp2=data[2];
	data[3]=data[0];
	data[2]=data[1];
	data[0]=temp3;
	data[1]=temp2;
}
//change endian of a pixel (24bit)
void CHANGE_ENDIAN_24(unsigned char *data){
	char temp2=data[2];
	data[2]=data[0];
	data[0]=temp2;
}

//RGBA to RGB24 (or BGRA to BGR24)
void CONVERT_RGBA32toRGB24(unsigned char *image,int w,int h){
	for(int i =0;i<h;i++)
		for(int j=0;j<w;j++){
			memcpy(image+(i*w+j)*3,image+(i*w+j)*4,3);
		}
}
//RGB24 to BGR24
void CONVERT_RGB24toBGR24(unsigned char *image,int w,int h){
	for(int i =0;i<h;i++)
		for(int j=0;j<w;j++){
			char temp2;
			temp2=image[(i*w+j)*3+2];
			image[(i*w+j)*3+2]=image[(i*w+j)*3+0];
			image[(i*w+j)*3+0]=temp2;
		}
}

//Change endian of a picture
void CHANGE_ENDIAN_PIC(unsigned char *image,int w,int h,int bpp){
	unsigned char *pixeldata=NULL;
	for(int i =0;i<h;i++)
		for(int j=0;j<w;j++){
			pixeldata=image+(i*w+j)*bpp/8;
			if(bpp==32){
				CHANGE_ENDIAN_32(pixeldata);
			}else if(bpp==24){
				CHANGE_ENDIAN_24(pixeldata);
			}
		}
}

inline unsigned char CONVERT_ADJUST(double tmp)
{
	return (unsigned char)((tmp >= 0 && tmp <= 255)?tmp:(tmp < 0 ? 0 : 255));
}
//YUV420P to RGB24
int Simpled_CONVERT_YUV420PtoRGB24(unsigned char* yuv_src,unsigned char* rgb_dst,int nWidth,int nHeight)
{
	if(yuv_src == NULL || rgb_dst == NULL)
	{
		return -1;
	}
	unsigned char *tmpbuf=(unsigned char *)malloc(nWidth*nHeight*3);
	unsigned char Y,U,V,R,G,B;
	unsigned char* y_planar,*u_planar,*v_planar;
	int rgb_width , u_width;
	rgb_width = nWidth * 3;
	u_width = (nWidth >> 1);
	int ypSize = nWidth * nHeight;
	int upSize = (ypSize>>2);
	int offSet = 0;

	y_planar = yuv_src;
	u_planar = yuv_src + ypSize;
	v_planar = u_planar + upSize;

	for(int i = 0; i < nHeight; i++)
	{
		for(int j = 0; j < nWidth; j ++)
		{
			// Get the Y value from the y planar
			Y = *(y_planar + nWidth * i + j);
			// Get the V value from the u planar
			offSet = (i>>1) * (u_width) + (j>>1);
#if 1
			V = *(v_planar + offSet);
			// Get the U value from the v planar
			U = *(u_planar + offSet);
#else
			V = *(u_planar + offSet);
			// Get the U value from the v planar
			U = *(v_planar + offSet);
#endif
			// Cacular the R,G,B values
			// Method 1
			//R' = 1.164*(Y’-16) + 1.596*(Cr'-128)
			//G' = 1.164*(Y’-16) - 0.813*(Cr'-128) - 0.392*(Cb'-128)
			//B' = 1.164*(Y’-16) + 2.017*(Cb'-128)
#if 0
			R = CONVERT_ADJUST(1.164*(Y-16)+1.596*(V-128));
			G = CONVERT_ADJUST(1.164*(Y-16)-0.813*(V-128)-0.392*(U-128));
			B = CONVERT_ADJUST(1.164*(Y-16)+2.017*(U-128));
#else
			R = CONVERT_ADJUST((Y + (1.4075 * (V - 128))));
			G = CONVERT_ADJUST((Y - (0.3455 * (U - 128) - 0.7169 * (V - 128))));
			B = CONVERT_ADJUST((Y + (1.7790 * (U - 128))));
#endif
			/*
			// The following formulas are from MicroSoft' MSDN
			int C,D,E;
			// Method 2
			C = Y - 16;
			D = U - 128;
			E = V - 128;
			R = CONVERT_ADJUST(( 298 * C + 409 * E + 128) >> 8);
			G = CONVERT_ADJUST(( 298 * C - 100 * D - 208 * E + 128) >> 8);
			B = CONVERT_ADJUST(( 298 * C + 516 * D + 128) >> 8);
			R = ((R - 128) * .6 + 128 )>255?255:(R - 128) * .6 + 128; 
			G = ((G - 128) * .6 + 128 )>255?255:(G - 128) * .6 + 128; 
			B = ((B - 128) * .6 + 128 )>255?255:(B - 128) * .6 + 128; 
			*/

			offSet = rgb_width * i + j * 3;
#if 1
			rgb_dst[offSet] = R;
			rgb_dst[offSet + 1] = G;
			rgb_dst[offSet + 2] = B;
#else
			rgb_dst[offSet] = B;
			rgb_dst[offSet + 1] = G;
			rgb_dst[offSet + 2] = R;
#endif
		}
	}
	free(tmpbuf);
	return 0;
}
