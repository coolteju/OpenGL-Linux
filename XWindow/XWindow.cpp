#include<iostream>
#include<stdio.h>
#include<stdlib.h>
#include<memory.h>

#include<X11/Xlib.h>
#include<X11/Xutil.h>
#include<X11/XKBlib.h>
#include<X11/keysym.h>

using namespace std;

bool bFullscreen=false;
Display *gpDisplay=NULL;
XVisualInfo *gpXVisualInfo=NULL;
Colormap gColormap;
Window gWindow;
int giWindowWidth=800;
int giWindowHeight=600;

int main(void)
{

	void CreateWindow(void);
	void ToggleFullscreen(void);
	void uninitialize(void);

	int WinWidth=giWindowWidth;
	int WinHeight=giWindowHeight;

	CreateWindow();

	XEvent event;
	KeySym keysym;

	while(1)
	{
		XNextEvent(gpDisplay,&event);
		switch(event.type)
		{
			case MapNotify:
				break;
			
			case KeyPress:
			keysym=XkbKeycodeToKeysym(gpDisplay,event.xkey.keycode,0,0);

				switch(keysym)
				{
					case XK_Escape:
					uninitialize();
					exit(0);

					case XK_F:
					case XK_f:
					if(bFullscreen==false)
					{
						ToggleFullscreen();
						bFullscreen=true;
					}
					else
					{
						ToggleFullscreen();
						bFullscreen=false;

					}break;

					default:
					break;

				}
				break;

				case ButtonPress:
				switch(event.xbutton.button)
				{
					case 1:
					break;
					case 2:
					 break;
					 case 3:
					 break;

				}
				break;

				case MotionNotify:
				break;

				case ConfigureNotify:
				WinWidth=event.xconfigure.width;
				WinHeight=event.xconfigure.height;
				break;

				case Expose:
				break;

				case DestroyNotify:
				break;

				case 33:
				uninitialize();
				exit(0);

				default :
				break;
		}
	}
	return 0;
}


void CreateWindow(void)
{
	void uninitialize(void);

	XSetWindowAttributes WinAttribs;

	int defaultScreen;
	int defaultDepth;
	int styleMask;

	gpDisplay=XOpenDisplay(NULL);

	if(gpDisplay==NULL)
	{
		printf("ERROR : Unable to Open X Display");
		uninitialize();
		exit(1);
	}
	defaultScreen=XDefaultScreen(gpDisplay);
	
	defaultDepth=DefaultDepth(gpDisplay,defaultScreen);

	gpXVisualInfo=(XVisualInfo *)malloc(sizeof(XVisualInfo));

	if(gpXVisualInfo==NULL)
	{
		printf("ERROR: Allocate memory for visual info\n Exiting now");
		exit(1);
	}

	if(XMatchVisualInfo(gpDisplay,defaultScreen,defaultDepth,TrueColor,gpXVisualInfo)==0)
	{
		printf("ERROR: Unable to get a Visual..\nExiting Now...\n ");
		exit(1);
	}

	WinAttribs.border_pixel=0;
	WinAttribs.background_pixmap=0;
	WinAttribs.colormap=XCreateColormap(gpDisplay,RootWindow(gpDisplay,gpXVisualInfo->screen),gpXVisualInfo->visual,AllocNone);

	gColormap=WinAttribs.colormap;

	WinAttribs.background_pixel=BlackPixel(gpDisplay,defaultScreen);

	WinAttribs.event_mask=ExposureMask|VisibilityChangeMask|ButtonPressMask|KeyPressMask|PointerMotionMask|StructureNotifyMask;

	styleMask=CWBorderPixel|CWBackPixel|CWEventMask|CWColormap;

	gWindow=XCreateWindow(gpDisplay,RootWindow(gpDisplay,gpXVisualInfo->screen),0,0,giWindowWidth,giWindowHeight,0,gpXVisualInfo->depth,
							InputOutput,gpXVisualInfo->visual,styleMask,&WinAttribs);

	if(!gWindow)
	{
		printf("ERROR: Unable to create Window \n exiting");
		exit(1);
	}

	XStoreName(gpDisplay,gWindow,"First Window");

	Atom WindowManagerDelete=XInternAtom(gpDisplay,"WM_DELETE_WINDOW",True);

	XSetWMProtocols(gpDisplay,gWindow,&WindowManagerDelete,1);

	XMapWindow(gpDisplay,gWindow);
}

void ToggleFullscreen(void)
{
	Atom wm_state;
	Atom Fullscreen;
	XEvent xev={0};

	wm_state=XInternAtom(gpDisplay,"_NET_WM_STATE",False);
	memset(&xev,0,sizeof(xev));

	xev.type=ClientMessage;
	xev.xclient.window=gWindow;
	xev.xclient.message_type=wm_state;
	xev.xclient.format=32;
	xev.xclient.data.l[0]=bFullscreen ? 0: 1;
	 Fullscreen=XInternAtom(gpDisplay,"_NET_WM_STATE_FULLSCREEN",False);

	 xev.xclient.data.l[1]=Fullscreen;

	 XSendEvent(gpDisplay,RootWindow(gpDisplay,gpXVisualInfo->screen),False,StructureNotifyMask,&xev);


}


void uninitialize(void)
{
	if(gWindow)
	{
		XDestroyWindow(gpDisplay,gWindow);
	}

	if(gColormap)
	{
		XFreeColormap(gpDisplay,gColormap);
	}

	if(gpXVisualInfo)
	{
		free(gpXVisualInfo);
		gpXVisualInfo=NULL;

	}

	if(gpDisplay)
	{
		XCloseDisplay(gpDisplay);
		gpDisplay=NULL;
	}
}