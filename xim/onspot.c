#include <stdio.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <X11/Xlocale.h>

#define DEFAULT_FONT    \
"8x16,\
-tlc-song-medium-r-normal--16-150-75-75-c-160-gbk-0"

//-tlc-song-medium-r-normal--16-*-*-*-*-*-big5-0"

#define F_SIZE 		16
#define COL    		30
#define ROW    		10

#define W_WIDTH  	(F_SIZE * COL)
#define W_HEIGHT 	(F_SIZE * ROW)

/* Callback functions */
int	P_StartCB(XIC, XPointer, XPointer);
int	P_DoneCB(XIC, XPointer, XPointer);
int	P_DrawCB(XIC, XPointer, XIMPreeditDrawCallbackStruct*);
int	P_CaretCB(XIC, XPointer, XIMPreeditCaretCallbackStruct*);

int	S_StartCB(XIC, XPointer, XPointer);
int	S_DoneCB(XIC, XPointer, XPointer);
int	S_DrawCB(XIC, XPointer, XIMStatusDrawCallbackStruct*);

/* Global Variables */
Display		*display;
int		screen;
GC		gc, gc_reverse;
Window		window;
XFontSet	fs;
int		dec;
int		row = 0, col = 0;
char		status_tab[30];
char		preedit_tab[128];
char		Feedback[128];
void		DrawPreeditString(char *, char *);

main(int argc, char *argv[])
{
  XGCValues	values;
  XWMHints	hints;
  Atom		protocols;
  char		**missing_list;
  int		missing_count;
  char		*def_string;
  XFontSetExtents *fs_ext;
  XIM		im;
  XIC		ic;
  XVaNestedList	preedit_attr, status_attr;
  XEvent	event;
  unsigned long mask, fevent;
  int		len = 128;
  char		string[128], s_tab[ROW][128];
  KeySym	keysym;
  int		i, count = 0;
  Status	status;

  XIMCallback	P_StartCB_Str;
  XIMCallback	P_DoneCB_Str;
  XIMCallback	P_DrawCB_Str;
  XIMCallback	P_CaretCB_Str;
  XIMCallback	S_StartCB_Str;
  XIMCallback	S_DoneCB_Str;
  XIMCallback	S_DrawCB_Str;

  display = XOpenDisplay("");
  screen = DefaultScreen(display);
  gc = DefaultGC(display, screen);
  values.foreground = WhitePixel(display, screen);
  values.foreground = BlackPixel(display, screen);
  gc_reverse = XCreateGC(display, DefaultRootWindow(display),
			 (GCForeground | GCBackground), &values);

  if (setlocale(LC_CTYPE, "") == NULL) {
    fprintf(stderr, "Error : setlocale() !\n");
    exit(0);
  }

  if (! XSupportsLocale() ) {
    fprintf(stderr, "Error : XSupportsLocale() !\n");
    exit(0);
  }

  fs = XCreateFontSet(display, DEFAULT_FONT, &missing_list,
		      &missing_count, &def_string);
  fs_ext = XExtentsOfFontSet(fs);
  dec = fs_ext->max_logical_extent.height + fs_ext->max_logical_extent.y;

  window = XCreateSimpleWindow(display, RootWindow(display, screen),
			       0, 0, W_WIDTH, W_HEIGHT + dec, 2,
			       BlackPixel(display, screen),
			       WhitePixel(display, screen));
  XStoreName(display, window, "XIM Demo - OnTheSpot");

  hints.flags = InputHint;
  hints.input = True;
  XSetWMHints(display, window, &hints);
  protocols = XInternAtom(display, "WM_DELETE_WINDOW", True);
  XSetWMProtocols(display, window, &protocols, 1);

  if ((im = XOpenIM(display, NULL, NULL, NULL)) == NULL) {
    printf("Error : XOpenIM !\n");
    exit(0);
  }

  P_StartCB_Str.client_data = NULL;
  P_StartCB_Str.callback = (XIMProc)P_StartCB;
  P_DoneCB_Str.client_data = NULL;
  P_DoneCB_Str.callback = (XIMProc)P_DoneCB;
  P_DrawCB_Str.client_data = NULL;
  P_DrawCB_Str.callback = (XIMProc)P_DrawCB;
  P_CaretCB_Str.client_data = NULL;
  P_CaretCB_Str.callback = (XIMProc)P_StartCB;
  preedit_attr = XVaCreateNestedList(0,
				     XNPreeditStartCallback, &P_StartCB_Str,
				     XNPreeditDoneCallback, &P_DoneCB_Str,
				     XNPreeditDrawCallback, &P_DrawCB_Str,
				     XNPreeditCaretCallback, &P_CaretCB_Str,
				     NULL);

  S_StartCB_Str.client_data = NULL;
  S_StartCB_Str.callback = (XIMProc)S_StartCB;
  S_DoneCB_Str.client_data = NULL;
  S_DoneCB_Str.callback = (XIMProc)S_DoneCB;
  S_DrawCB_Str.client_data = NULL;
  S_DrawCB_Str.callback = (XIMProc)S_DrawCB;
  status_attr = XVaCreateNestedList(0,
				    XNStatusStartCallback, &S_StartCB_Str,
				    XNStatusDoneCallback, &S_DoneCB_Str,
				    XNStatusDrawCallback, &S_DrawCB_Str,
				    NULL);

  ic = XCreateIC(im,
		 XNInputStyle, XIMPreeditCallbacks | XIMStatusCallbacks,
		 XNClientWindow, window,
		 XNPreeditAttributes, preedit_attr,
		 //XNStatusAttributes, status_attr,
		 NULL);

  if (ic == NULL) {
    printf("Error : XCreateIC() ! \n");
    XCloseIM(im);
    exit(0);
  }
  XFree(preedit_attr);
  XFree(status_attr);

  XGetICValues(ic, XNFilterEvents, &fevent, NULL);
  mask = ExposureMask | KeyPressMask | FocusChangeMask;
  XSelectInput(display, window, mask | fevent);

  for(i = 0; i < ROW; i++)
    s_tab[i][0] = 0;

  XMapWindow(display, window);

  printf("This program is too simple to flush the preedit area.\n");

  for(;;) {
    XNextEvent(display, &event);
    if (XFilterEvent(&event, None) == True)
      continue;

    switch(event.type)
    {
    case FocusIn:
      XSetICFocus(ic);
      break;
    case FocusOut:
      XUnsetICFocus(ic);
      break;
    case Expose:
      for ( i=0; i < ROW; i++)
	XmbDrawString(display, window, fs, gc, 0,
		      F_SIZE * (i + 1), s_tab[i], strlen(s_tab[i]));
      DrawPreeditString(preedit_tab, Feedback);
      XmbDrawString(display, window, fs, gc, 20, W_HEIGHT,
		    status_tab, strlen(status_tab));
      break;
    case KeyPress:
      count = XmbLookupString(ic, (XKeyPressedEvent *) &event,
			      string, len, &keysym, &status);
      /* string[count] = 0; */
      if (status == XLookupBoth && keysym == XK_Return)
      {
	row = (++row) % ROW;
	col = 0;
	s_tab[row][0] = 0;
	preedit_tab[0] = 0;
	Feedback[0] = 0;
	XClearArea(display, window, 0, F_SIZE * row + dec,
		   W_WIDTH, F_SIZE, False);
      }
      else if (status = XLookupChars || status == XLookupBoth)
      {
	XmbDrawString(display, window, fs, gc,
		      F_SIZE / 2 * col, F_SIZE * (row + 1),
		      string, count);
	for (i = 0; i < count; i++, col++)
	  s_tab[row][col] = 0;
	s_tab[row][col] = 0;
	preedit_tab[0] = 0;
	Feedback[0] = 0;
      }
      break;

    case ClientMessage:
      if (event.xclient.data.l[0] == protocols)
      {
	XDestroyIC(ic);
	XCloseIM(im);
	XDestroyWindow(display, window);
	XCloseDisplay(display);
	exit(0);
      }
      break;
    defaults:
      break;
    }
  }
}

int P_StartCB(XIC ic, XPointer client_data, XPointer P_S)
{
  printf(" PreeditStartCallback\n");
  return(127);
}

int P_DoneCB(XIC ic, XPointer client_data, XPointer P_DN)
{
  printf(" PreeditDoneCallback\n");
  preedit_tab[0] = 0;
  Feedback[0] = 0;
}

int P_DrawCB(XIC ic, XPointer client_data,
	      XIMPreeditDrawCallbackStruct *P_DR)
{
  int i, j, lcol;
  char preedit_string[128], emp_string[1];

  lcol = col;
  emp_string[0] = ' ' ;

/*
printf("caret chg_length and strlen:%d %d %d\n",
	P_DR->caret,
	P_DR->chg_length,
	P_DR->text->length);
*/
  for (i = 0; i <(P_DR->chg_length*2); i++, lcol++)
    XmbDrawImageString(display, window, fs, gc,
		       F_SIZE / 2 * (lcol + 2 * P_DR->chg_first),
		       F_SIZE * (row + 1),
		       emp_string, 1);

  if (P_DR->text) {
    if (P_DR->text->encoding_is_wchar)
      wctomb(preedit_string, P_DR->text->string.wide_char);
    else
      strcpy(preedit_string, P_DR->text->string.multi_byte);
    for (i = P_DR->chg_first * 2, j = 0; preedit_string[j] != 0;
	 i++, j++)
      preedit_tab[j] = preedit_string[j];
    preedit_tab[i] = 0;
    for (i = P_DR->chg_first, j = 0;
	 (unsigned int)i < P_DR->chg_first + P_DR->text->length;
	 i++, j++)
      Feedback[i] = P_DR->text->feedback[j];
    DrawPreeditString(preedit_tab, Feedback);
  }
}

int P_CaretCB(XIC ic, XPointer client_data,
	      XIMPreeditCaretCallbackStruct *P_C)
{  printf(" PreeditCaretCalback\n");
}

int S_StartCB(XIC ic, XPointer client_data, XPointer S_S)
{
  printf(" StatusStartCallback\n");
}

int S_DoneCB(XIC ic, XPointer client_data, XPointer S_S)
{
  printf(" StartusDoneCallback\n");
}

int S_DrawCB(XIC ic, XPointer client_data,
	      XIMStatusDrawCallbackStruct *S_DR)
{
  if (S_DR->type != XIMBitmapType) {
    if (S_DR->data.text->encoding_is_wchar)
      wctomb(status_tab, S_DR->data.text->string.wide_char);
    else
      strcpy(status_tab, S_DR->data.text->string.multi_byte);
    XClearArea(display, window, 0, W_HEIGHT - F_SIZE + dec,
		W_WIDTH, F_SIZE, False);
    XmbDrawImageString(display, window, fs, gc,
		  20, W_HEIGHT, status_tab, strlen(status_tab));
  }
}

void DrawPreeditString(char *p, char *fp)
{
  int i;
  int lcol = col;

  dec = 2;
  for (i = 0; i < strlen(fp); i++) {

    if (fp[i] == XIMReverse)
      XmbDrawImageString(display, window, fs, gc_reverse,
			 F_SIZE / 2 * lcol, F_SIZE * (row + 1),
			 p, mblen(p, 3));
    //else if (fp[i] == XIMUnderline) {
    else {
      XmbDrawImageString(display, window, fs, gc_reverse,
			 F_SIZE / 2 * lcol, F_SIZE * (row + 1),
			 p, mblen(p, 3));
      XDrawLine(display, window, gc,
		F_SIZE / 2 * lcol, F_SIZE * (row + 1),
		F_SIZE / 2 * (lcol + mblen(p, 3)) - (dec - 1),
		F_SIZE * (row + 1) + 1);
		//F_SIZE / 2 * F_SIZE * (row + 1) + 1);

    }
    lcol += mblen(p, 3);
    p += mblen(p, 3);
  }
}
