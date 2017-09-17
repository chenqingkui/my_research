#ifndef _SIMPLE_IMAGE_CONVERT_H_
#define _SIMPLE_IMAGE_CONVERT_H_
//简单的将YUV420P转换为RGB24
int Simpled_CONVERT_YUV420PtoRGB24(unsigned char* yuv_src,unsigned char* rgb_dst,int nWidth,int nHeight);
//改变大小端
void CHANGE_ENDIAN_PIC(unsigned char *image,int w,int h,int bpp);
#endif
