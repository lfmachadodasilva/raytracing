/*
 *	Computa��o Gr�fica - Trabalho de Raytracing
 *	Professor Marcelo Gattass
 *
 *	@file main.c 
 *
 *	@author 
 *			- Maira Noronha
 *			- Thiago Bastos
 *
 *	@date
 *			�ltima Modifica��o:		10 de Fevereiro de 2003
 *
 *	@version 2.0
 *
 *	Este arquivo n�o precisa ser modificado.
 *	Veja o m�dulo raytracing.c
 */

#include <stdio.h>
#include <time.h>

/*- Inclusao das bibliotecas IUP e CD: ------------------------------------*/
#include <iup.h>                    /* interface das funcoes IUP */
#include <iupgl.h>                  /* interface da funcao do canvas do OpenGL no IUP */

#ifdef WIN32
#include <windows.h>                /* inclui as definicoes do windows para o OpenGL */
#include <gl/gl.h>                  /* prototypes do OpenGL */
#include <gl/glu.h>                 /* prototypes do OpenGL */
#else
#include <GL/gl.h>                  /* prototypes do OpenGL */
#include <GL/glu.h>                 /* prototypes do OpenGL */
#endif

#include <time.h>

#include "image.h"                  /* TAD para imagem */
#include "color.h"
#include "algebra.h"
#include "raytracing.h"

/* -- implemented in "iconlib.c" to load standard icon images into IUP */
void IconLibOpen(void);

/*- Contexto do Programa: -------------------------------------------------*/
Scene* scene;         /* cena corrente */
int yc=0;            /* y corrente para Ray Tracing incremetnal */
int width,height=-1; /* alrgura e altura corrente */
Image *image;        /* imagem que armazena o resultado at� agora do algoritmo */
double eye[VECTOR];
Camera* camera;

double duration;
int start_time; 
int finish_time;


Ihandle *canvas;      /* ponteiro IUP dos canvas */
Ihandle *label;       /* ponteiro IUP do label para colocar mensagens para usuario */

/*- Funcoes auxiliares ------------*/

/* Dialogo de selecao de arquivo  */
static char * get_file_name( void )
{
  Ihandle* getfile = IupFileDlg();
  char* filename = NULL;

  IupSetAttribute(getfile, IUP_TITLE, "Abertura de arquivo"  );
  IupSetAttribute(getfile, IUP_DIALOGTYPE, IUP_OPEN);
  IupSetAttribute(getfile, IUP_FILTER, "*.rt4");
  IupSetAttribute(getfile, IUP_FILTERINFO, "Arquivo de cena (*.rt4)");
  IupPopup(getfile, IUP_CENTER, IUP_CENTER);  /* o posicionamento nao esta sendo respeitado no Windows */

  filename = IupGetAttribute(getfile, IUP_VALUE);
  return filename;
}


/*------------------------------------------*/
/* Callbacks do IUP.                        */
/*------------------------------------------*/


/* - Callback de mudanca de tamanho no canvas (mesma para ambos os canvas) */
int resize_cb(Ihandle *self, int width, int height)
{
 IupGLMakeCurrent(self);  /* torna o foco do OpenGL para este canvas */

 /* define a area do canvas que deve receber as primitivas do OpenGL */
 glViewport(0,0,width,height);

 /* transformacao de instanciacao dos objetos no sistema de coordenadas da camera */
 glMatrixMode(GL_MODELVIEW);
 glLoadIdentity();           /* identidade,  ou seja nada */

 /* transformacao de projecao */
 glMatrixMode(GL_PROJECTION);
 glLoadIdentity();
 gluOrtho2D (0.0, (GLsizei)(width), 0.0, (GLsizei)(height));  /* ortografica no plano xy de [0,w]x[0,h] */
  
 glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
 glClear(GL_COLOR_BUFFER_BIT);

 return IUP_DEFAULT; /* retorna o controle para o gerenciador de eventos */
}


/* - Callback de repaint do canvas  */
int repaint_cb(Ihandle *self)
{
  int w,h;
  int x,y;
  float rgb[COLOR];

  if (yc==0){  /* e' a primeira vez: limpa o canvas */
	  IupGLMakeCurrent(self);
	  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	  glClear(GL_COLOR_BUFFER_BIT);
    IupGLSwapBuffers(self); /* change the back buffer with the front buffer */
	  glFlush();
  }

  if (yc!=height) { /* esta callback so'desenha depois que o algoritmo termina a imagem */
     return IUP_DEFAULT; 
  }

  imgGetWidth(image, &w);
  imgGetHeight(image, &h);
  IupGLMakeCurrent(self);
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  glBegin(GL_POINTS);
    for (y=0;y<h;y++) {
      for (x=0; x<w; x++) {
         imageGetPixel(image, x, y, rgb);
				 glColor3f((float)rgb[RED],(float)rgb[GREEN],(float)rgb[BLUE]);
         glVertex2i(x,y);
       }
    }
  glEnd();

  IupGLSwapBuffers(self); /* change the back buffer with the front buffer */
  glFlush();
  return IUP_DEFAULT; /* retorna o controle para o gerenciador de eventos */
} 

/* captura um nome para um arquivo */
static char * get_new_file_name( void )
{
  Ihandle* getfile = IupFileDlg();
  char* filename = NULL;

  IupSetAttribute(getfile, IUP_TITLE, "Salva arquivo"  );
  IupSetAttribute(getfile, IUP_DIALOGTYPE, IUP_SAVE);
  IupSetAttribute(getfile, IUP_FILTER, "*.bmp");
  IupSetAttribute(getfile, IUP_FILTERINFO, "Arquivo de imagem (*.bmp)");
  IupPopup(getfile, IUP_CENTER, IUP_CENTER);  /* o posicionamento nao esta sendo respeitado no Windows */

  filename = IupGetAttribute(getfile, IUP_VALUE);
  return filename;
}

/* salva a imagem gerada */
int save_cb(Ihandle *self)
{
  char* filename = get_new_file_name( );  /* chama o dialogo de abertura de arquivo */
  if (filename==NULL) return 0;
  imgWriteBMP(filename,image);
  //imgAdjust2eN(image);
  return IUP_DEFAULT;
}

/* calcula uma linha da imagem a cada camada de idle */
int idle_cb(void)
{
	int x;
	double ray[VECTOR];
	float pixel[COLOR];

  /* Faz uma linha de pixels por vez */
  if (yc < height) {

    IupGLMakeCurrent(canvas);
    glBegin(GL_POINTS);
    {
      for (x = 0; x < width; ++x) {

        camGetRay(camera, x, yc, ray);
        rayTrace(scene, eye, ray, 0, pixel);

        imageSetPixel(image, x, yc, pixel);
        glColor3f((float) pixel[RED], (float) pixel[GREEN], (float) pixel[BLUE]);
        glVertex2i(x, yc);

      }
    }
    glEnd();

    glFlush();

    IupGLSwapBuffers(canvas);
    
    yc++;
  } else {

		IupSetFunction (IUP_IDLE_ACTION, (Icallback) NULL); /* a imagem ja' esta' completa */
		finish_time = clock();
		duration = (double)(finish_time - start_time)/CLOCKS_PER_SEC;
		IupSetfAttribute(label, "TITLE", "tempo=%.3lf s", duration);

		glFlush();
	}


	return IUP_DEFAULT;
}


/* carrega uma nova cena */
int load_cb(void) {
  char* filename = get_file_name();  /* chama o dialogo de abertura de arquivo */
  char buffer[30];

  if (filename==NULL) return 0;

  /* Le a cena especificada */
  scene = sceLoad( filename );
  if( scene == NULL ) return IUP_DEFAULT;

  sceGetCamera( scene, &camera );
  camGetEye( camera, eye );
  camGetScreenWidth( camera, &width );
  camGetScreenHeight( camera, &height );
  yc=0;

  if (image) imgDestroy(image);
  image = imgCreate( width, height );
  IupSetfAttribute(label, "TITLE", "%3dx%3d", width, height);
  sprintf(buffer,"%3dx%3d", width, height);
  IupSetAttribute(canvas,IUP_RASTERSIZE,buffer);
  IupSetFunction (IUP_IDLE_ACTION, (Icallback) idle_cb);
  start_time  = clock(); 
  return IUP_DEFAULT;
}

/*-------------------------------------------------------------------------*/
/* Incializa o programa.                                                   */
/*-------------------------------------------------------------------------*/

int init(void)
{
  Ihandle *dialog, *statusbar,  *box;

  Ihandle *toolbar, *load, *save;

  /* creates the toolbar and its buttons */
  load = IupButton("Load", "load_cb");
  IupSetAttribute(load,"TIP","Carrega uma imagem.");
  //IupSetAttribute(load,"IMAGE","icon_lib_open");
  IupSetFunction("load_cb", (Icallback)load_cb);

  save = IupButton("Save", "save_cb");
  IupSetAttribute(save,"TIP","Salva no formato GIF.");
  //IupSetAttribute(save,"IMAGE","icon_lib_save");
  

  toolbar = IupHbox(
       load, 
       save,
	   IupFill(),
     NULL);

  IupSetAttribute(toolbar, "ALIGNMENT", "ACENTER");
 

  /* cria um canvas */
  canvas = IupGLCanvas("repaint_cb"); 
  IupSetAttribute(canvas,IUP_RASTERSIZE,"400x400");
  IupSetAttribute(canvas, "RESIZE_CB", "resize_cb");

  /* associa o evento de repaint a funccao repaint_cb */
  IupSetFunction("repaint_cb", (Icallback) repaint_cb);
  IupSetFunction("save_cb", (Icallback)save_cb);
  IupSetFunction("resize_cb", (Icallback) resize_cb);
  IupSetFunction (IUP_IDLE_ACTION, (Icallback) NULL);

  /* the status bar is just a label to put some usefull information in run time */
  label = IupLabel("status");
  IupSetAttribute(label, "EXPAND", "HORIZONTAL");
  IupSetAttribute(label, "FONT", "COURIER_NORMAL_10");
  statusbar = IupSetAttributes(IupHbox(
	  IupFrame(IupHbox(label, NULL)), 
                NULL), "MARGIN=5x5");

  /* this is the most external box that puts together
     the toolbar, the two canvas and the status bar */
  box = IupVbox(
          toolbar,
          canvas, 
          statusbar, 
          NULL);

  /* create the dialog and set its attributes */
  dialog = IupDialog(box);
  IupSetAttribute(dialog, "CLOSE_CB", "app_exit_cb");
  IupSetAttribute(dialog, "TITLE", "CG2004: Trab. 2");


  IupShowXY(dialog, IUP_CENTER, IUP_CENTER);

  return 1;
}

/*-------------------------------------------------------------------------*/
/* Rotina principal.                                                       */
/*-------------------------------------------------------------------------*/
void main(int argc, char* argv[]) {
    IupOpen(&argc,&argv);
    IupGLCanvasOpen();
    //IconLibOpen();
    if ( init() )
		IupMainLoop();
    IupClose();
}