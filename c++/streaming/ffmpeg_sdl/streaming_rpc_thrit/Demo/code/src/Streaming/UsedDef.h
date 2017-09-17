#ifndef _VSDK_USE_DEF_H_
#define _VSDK_USE_DEF_H_

//user define 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//ע�⿪��GDI_PLUS������GDI PlUS��ʾ,GDI Plus�ֱ��ʴ�ʱ��ʾ�ӳٽϴ�
//ע�⿪��SDL_SHOW������SDL�ķ�ʽ���Ե������ڵķ�ʽ��ʾ��Ƶ��
//ע����SDL_SHOWģʽ�¿���SDL_USE_HWND�ķ�ʽ������ʾǶ��MFC������ʾ��Ƶ��
//ע����SDL_SHOWģʽ�¿���SDL_USE_BUTTON���ڵ���������ʾ��Ƶ���ĵ׶���ʾ��Ƶ�����ź���ͣ�İ�ť
//#define GDI_PLUS    

#define SIMPLIED_DECODE
#define SIMPLIED_STREAM 

#ifndef GDI_PLUS
	#define SDL_SHOW
	//#define SDL_USE_HWND 
	#ifndef SDL_USE_HWND
		#define SDL_USE_BUTTON
	#endif
#endif
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef GDI_PLUS
	#define SHOW_WINDOW_WIDTH  720
	#define SHOW_WINDOW_HEIGHT 560
#else
	#ifdef SDL_SHOW
		#define SHOW_WINDOW_WIDTH  720
		#define SHOW_WINDOW_HEIGHT 576
	#else
		#define SHOW_WINDOW_WIDTH  1920
		#define SHOW_WINDOW_HEIGHT 1080
	#endif
#endif
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
#endif