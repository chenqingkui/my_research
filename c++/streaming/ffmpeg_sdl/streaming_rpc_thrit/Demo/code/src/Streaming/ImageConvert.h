#ifndef _SIMPLE_IMAGE_CONVERT_H_
#define _SIMPLE_IMAGE_CONVERT_H_
//�򵥵Ľ�YUV420Pת��ΪRGB24
int Simpled_CONVERT_YUV420PtoRGB24(unsigned char* yuv_src,unsigned char* rgb_dst,int nWidth,int nHeight);
//�ı��С��
void CHANGE_ENDIAN_PIC(unsigned char *image,int w,int h,int bpp);
#endif
