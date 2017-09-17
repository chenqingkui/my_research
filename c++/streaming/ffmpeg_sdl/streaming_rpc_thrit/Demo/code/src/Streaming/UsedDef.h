#ifndef _VSDK_USE_DEF_H_
#define _VSDK_USE_DEF_H_

//user define 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//注意开启GDI_PLUS将采用GDI PlUS显示,GDI Plus分辨率大时显示延迟较大
//注意开启SDL_SHOW将采用SDL的方式则将以弹出窗口的方式显示视频流
//注意在SDL_SHOW模式下开启SDL_USE_HWND的方式将把显示嵌入MFC界面显示视频流
//注意在SDL_SHOW模式下开启SDL_USE_BUTTON会在弹出窗口显示视频流的底端显示视频流播放和暂停的按钮
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