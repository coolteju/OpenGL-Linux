#include  <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/XKBlib.h>
#include <X11/keysym.h>

#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glu.h>

#include "OGL.h"

using namespace std;

bool bFullscreen=false;

Display *gpDisplay=NULL;
XVisualInfo *gpXVisualInfo=NULL;

Colormap gColormap;

Window gWindow;

int giWinWidth=800;
int giWinHeight=600;

GLfloat Angle=0.0f;

bool bLight = false;
GLfloat LightAmbient[] = { 0.5f,0.5f,0.5f,1.0f };
GLfloat Lightdiffuse[] = { 1.0f,1.0f,1.0f,1.0f };
GLfloat LightPosition[] = { 0.0f,0.0f,2.0f,1.0f };


GLXContext gGLXContext;


int main(void)
{
	
	void CreateWindow(void);
	void initialize(void);
	void ToggleFullscreen(void);
	void display();
	void spin(void);
	void resize(int ,int);
	void uninitialize(void);


	int WinWidth=giWinWidth;
	int WinHeight=giWinHeight;

	bool bDone=false;

	CreateWindow();

	initialize();    

	XEvent event;
	KeySym keysym;


	while(bDone==false)
	{
		while(XPending(gpDisplay))
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
								bDone=true;

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
							}
							break;

							case XK_l:
							case XK_L:
								if(bLight==false)
								{
									bLight = true;
									glEnable(GL_LIGHTING);
								}

								else
								{
									bLight=false;
									glDisable(GL_LIGHTING);
								}

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
						resize(WinWidth,WinHeight);
						break;

					case Expose:
						break;

					case DestroyNotify:
						break;

					case 33:
						bDone=true;
						break;

					default:
						break;


			}

		}
		spin();

		display();

	}	
	uninitialize();

		return 0;
}



void CreateWindow(void)
{
	void uninitialize(void);

	XSetWindowAttributes winAttribs;

	int defaultScreen;
	int defaultDepth;
	int styleMask;

	static int frameBufferAttributes[]=
	{
		GLX_RGBA,
		GLX_DEPTH_SIZE,24,
		GLX_RED_SIZE,8,
		GLX_GREEN_SIZE ,8,
		GLX_BLUE_SIZE,8,
		GLX_DOUBLEBUFFER,
		None

	};
			/*static int frameBufferAttributes[] = 
				{ 
					GLX_RGBA, 
					GLX_DEPTH_SIZE, 24,
					GLX_DOUBLEBUFFER,
					None 
				};
*/

	gpDisplay=XOpenDisplay(NULL);

	if(gpDisplay==NULL)
	{
		printf("ERROR: Cannot Open Display,\nExiting");
		uninitialize();
		exit(1);
	}

	defaultScreen=XDefaultScreen(gpDisplay);

	gpXVisualInfo=glXChooseVisual(gpDisplay,defaultScreen,frameBufferAttributes);

	winAttribs.border_pixel=0;
	winAttribs.background_pixmap=0;
	winAttribs.colormap=XCreateColormap(gpDisplay,RootWindow(gpDisplay,gpXVisualInfo->screen),gpXVisualInfo->visual,AllocNone);

	gColormap=winAttribs.colormap;

	winAttribs.background_pixel=BlackPixel(gpDisplay,defaultScreen);

	winAttribs.event_mask=ExposureMask|VisibilityChangeMask|ButtonPressMask|KeyPressMask|PointerMotionMask|StructureNotifyMask;

	styleMask=CWBorderPixel|CWBackPixel|CWEventMask|CWColormap;

	gWindow=XCreateWindow(gpDisplay,RootWindow(gpDisplay,gpXVisualInfo->screen),0,0,giWinWidth,giWinHeight,0,gpXVisualInfo->depth,
							InputOutput,gpXVisualInfo->visual,styleMask,&winAttribs	);


	if(!gWindow)
	{
		printf("ERROR: failed to create new window..\nexiting...");
		uninitialize();
		exit(1);
	}

	XStoreName(gpDisplay,gWindow,"OpenGL Window");

	Atom windowManagerDelete=XInternAtom(gpDisplay,"WM_DELETE_WINDOW",True);

	XSetWMProtocols(gpDisplay,gWindow,&windowManagerDelete,1);

	XMapWindow(gpDisplay,gWindow);

}

void ToggleFullscreen(void)
{
	Atom wm_state;
	Atom fullscreen;
	XEvent xev={0};

	wm_state=XInternAtom(gpDisplay,"_NET_WM_STATE",False);
	memset(&xev,0,sizeof(xev));

	xev.type=ClientMessage;
	xev.xclient.window=gWindow;
	xev.xclient.message_type=wm_state;
	xev.xclient.format=32;
	xev.xclient.data.l[0]=bFullscreen? 0 :1;

	fullscreen=XInternAtom(gpDisplay,"_NET_WM_STATE_FULLSCREEN",False);

	xev.xclient.data.l[1]=fullscreen;

	XSendEvent(gpDisplay,RootWindow(gpDisplay,gpXVisualInfo->screen),false,StructureNotifyMask,&xev);
}

void initialize(void)
{

	void resize(int ,int);

	gGLXContext=glXCreateContext(gpDisplay,gpXVisualInfo,NULL,GL_TRUE);

	glXMakeCurrent(gpDisplay,gWindow,gGLXContext);

	glShadeModel(GL_SMOOTH);

	glClearColor(0.0f,0.0f,0.0f,0.0f);

	glClearDepth(1.0f);

	glEnable(GL_DEPTH_TEST);

	glDepthFunc(GL_LEQUAL);

	glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_NICEST);

	glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmbient);

	glLightfv(GL_LIGHT1, GL_DIFFUSE, Lightdiffuse);

	glLightfv(GL_LIGHT1, GL_POSITION, LightPosition);

	glEnable(GL_LIGHT1);

	resize(giWinWidth,giWinHeight);
}


void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();
	glTranslatef(0.0f, 0.0f, -1.5f);
	glRotatef(-70.0f, 1.0f, 0.0f, 0.0f);
	glRotatef(Angle, 0.0f, 0.0f, 1.0f);

	glBegin(GL_TRIANGLES);
	for (int i = 0; i < sizeof(face_indicies) / sizeof(face_indicies[0]); i++)
	{
		for (int j = 0; j < 3; j++)
		{
			int vi = face_indicies[i][j];
			int ni = face_indicies[i][j + 3];
			int ti = face_indicies[i][j + 6];
			glNormal3f(normals[ni][0], normals[ni][1], normals[ni][2]);

			glTexCoord2f(textures[ti][0], textures[ti][1]);
			glVertex3f(vertices[vi][0], vertices[vi][1], vertices[vi][2]);
		}
	}

	glEnd();
	glXSwapBuffers(gpDisplay,gWindow);
}

void resize(int width, int height)
{
	if(height==0)
		height=1;

	glViewport(0,0,(GLsizei)width,(GLsizei)height);

	glMatrixMode(GL_PROJECTION);

	glLoadIdentity();

	gluPerspective(45.0f,(GLfloat)width/(GLfloat)height,0.1f,100.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();


}

void spin(void)
{
	Angle=Angle+2.0f;
	if(Angle>=360.0f)
	{
		Angle=Angle - 360.0f;
	}

	


}

void uninitialize(void)
{
	GLXContext currentGLXContext;
	currentGLXContext=glXGetCurrentContext();

	if(currentGLXContext!=NULL && currentGLXContext==gGLXContext)
	{
		glXMakeCurrent(gpDisplay,0,0);
	}

	if(gGLXContext)
	{
		glXDestroyContext(gpDisplay,gGLXContext);
	}

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


