#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>

struct _Ecore_Input_Context {
     XIC xim_input_context;
     XIM xim_input_method;
     XIMStyle xim_input_style;
};

typedef struct _Ecore_Input_Context Ecore_Input_Context;

struct _Private_Data {
     Display *display;
     XFontSet fontset;
     Window win;
     GC gc;
     Ecore_Input_Context *eic;
};

typedef struct _Private_Data Private_Data;

void init_locale(void);
void xim_get_position(XPoint * pos);
void xim_set_status_position(const Ecore_Input_Context *eic);

static void _xim_destroy_cb(XIM xim, XPointer client_data, XPointer call_data);
static void _xim_instantiate_cb(Display * display, XPointer client_data, XPointer call_data);
static int _xim_real_init(Ecore_Input_Context * eic, const char *input_method, const char *preedit_type);
static void _xim_send_spot(const Ecore_Input_Context * eic);
static void _xim_get_area(XRectangle * preedit_rect, XRectangle * status_rect, XRectangle * needed_rect);
static void _xim_set_size(XRectangle * size);
static void _xim_set_color(unsigned long *fg, unsigned long *bg);

static Private_Data *data = NULL;

void
init_locale(void)
{
   char *locale = NULL;
   
   if (data == NULL) return;

   locale = setlocale(LC_ALL, "");
   XSetLocaleModifiers("");
   data->fontset = (XFontSet) 0;
   data->eic = NULL;

   if ((locale == NULL) || (!XSupportsLocale())) 
     {
	fprintf(stderr, "Locale not supported; defaulting to portable \"C\" locale.\n");
        locale = setlocale(LC_ALL, "C");
        XSetLocaleModifiers("");
        if (!locale) return;
        if (!XSupportsLocale()) return;
     } 
   else 
     {
	const char * font;
	char **missing_charsets;
	int missing_charset_count;

	data->eic = (Ecore_Input_Context*) calloc(1, sizeof(Ecore_Input_Context));
	font = "-misc-fixed-medium-r-normal--7-*-*-*-c-*-iso10646-,"
	       "-misc-fixed-*-r-*-*-*-120-*-*-*-*-*-*,"
	       "*";
	
	data->fontset = XCreateFontSet(data->display, font, 
	      &missing_charsets, &missing_charset_count, NULL );

	if ((data->fontset == (XFontSet) 0) || (_xim_real_init(data->eic, NULL, NULL) != -1)) 
	  {
            return;
	  }
        XRegisterIMInstantiateCallback(data->display, NULL, NULL, NULL, _xim_instantiate_cb, (XPointer) data->eic);
    }
}

void
xim_get_position(XPoint * pos)
{
   pos->x = 20;
   pos->y = 20;
}

void
xim_set_status_position(const Ecore_Input_Context *eic)
{
   XRectangle preedit_rect, status_rect, *needed_rect, rect;
   XVaNestedList preedit_attr, status_attr;
   XPoint spot;

   if (eic->xim_input_context == NULL) return;

   if (eic->xim_input_style & XIMPreeditPosition) 
     {
	_xim_set_size(&rect);
        xim_get_position(&spot);

        preedit_attr = XVaCreateNestedList(0, XNArea, &rect, XNSpotLocation, &spot, NULL);
        XSetICValues(eic->xim_input_context, XNPreeditAttributes, preedit_attr, NULL);
        XFree(preedit_attr);
     } 
   else if (eic->xim_input_style & XIMPreeditArea) 
     {
	/* Getting the necessary width of preedit area */
        status_attr = XVaCreateNestedList(0, XNAreaNeeded, &needed_rect, NULL);
        XGetICValues(eic->xim_input_context, XNStatusAttributes, status_attr, NULL);
        XFree(status_attr);

        _xim_get_area(&preedit_rect, &status_rect, needed_rect);

        preedit_attr = XVaCreateNestedList(0, XNArea, &preedit_rect, NULL);
        status_attr = XVaCreateNestedList(0, XNArea, &status_rect, NULL);
        XSetICValues(eic->xim_input_context, XNPreeditAttributes, preedit_attr, XNStatusAttributes, status_attr, NULL);
        XFree(preedit_attr);
        XFree(status_attr);
     }
}

static void
_xim_destroy_cb(XIM xim, XPointer client_data, XPointer call_data)
{
   Ecore_Input_Context *eic;

   eic = (Ecore_Input_Context*) client_data;

   eic->xim_input_context = NULL;
   eic->xim_input_method = NULL;
   XRegisterIMInstantiateCallback(data->display, NULL, NULL, NULL, _xim_instantiate_cb, (XPointer) eic);
   xim = NULL;
   client_data = call_data = (XPointer) 0;
}

static void
_xim_instantiate_cb(Display * display, XPointer client_data, XPointer call_data)
{
   Ecore_Input_Context *eic;

   eic = (Ecore_Input_Context*) client_data;

   _xim_real_init(eic, NULL, NULL);
   if (eic->xim_input_context) 
     {
	XUnregisterIMInstantiateCallback(data->display, NULL, NULL, NULL, _xim_instantiate_cb, (XPointer) eic);
     }
   display = NULL;
   client_data = call_data = (XPointer) 0;
}

static int
_xim_real_init(Ecore_Input_Context * eic, const char *input_method, const char *preedit_type)
{
   char *p, *s, buf[64], tmp[1024];
   char *end, *next_s;
   XIMStyles *xim_styles = NULL;
   int found;
   XPoint spot;
   XRectangle rect, status_rect, needed_rect;
   unsigned long fg, bg;
   XVaNestedList preedit_attr = NULL;
   XVaNestedList status_attr = NULL;
   
   if (eic->xim_input_context != NULL) return -1;
   eic->xim_input_style = 0;

   if (input_method && input_method[0]) 
     {
	strncpy(tmp, input_method, sizeof(tmp) - 1);
        for (s = tmp; s[0]; s = next_s + 1) 
	  {
	     while (s[0] && isspace(s[0])) s++;
	     if (!s[0]) break;
	     
	     for (end = s; (end[0] && (end[0] != ',')); end++);
	     for (next_s = end--; ((end >= s) && isspace(end[0])); end--);

             end[1] = '\0';
	     if (s[0]) 
	       {
		  snprintf(buf, sizeof(buf), "@im=%s", s);
		  if (((p = XSetLocaleModifiers(buf)) != NULL) && (*p)
                    && ((eic->xim_input_method = XOpenIM(data->display, NULL, NULL, NULL)) != NULL)) 
		    {
		       break;
		    }
	       }
	     if (!next_s[0]) break;
	  }
     }

    /* try with XMODIFIERS env. var. */
    if (eic->xim_input_method == NULL && getenv("XMODIFIERS") && (p = XSetLocaleModifiers("")) != NULL && *p) 
      {
	 eic->xim_input_method = XOpenIM(data->display, NULL, NULL, NULL);
      }

    /* try with no modifiers base */
    if (eic->xim_input_method == NULL && (p = XSetLocaleModifiers("@im=none")) != NULL && *p) 
      {
         eic->xim_input_method = XOpenIM(data->display, NULL, NULL, NULL);
      }

    if (eic->xim_input_method == NULL) 
      {
         eic->xim_input_method = XOpenIM(data->display, NULL, NULL, NULL);
      }

    if (eic->xim_input_method == NULL) return -1;

    // Setup input method destory callbacks
    {
        XIMCallback destroy_cb;

        destroy_cb.callback = _xim_destroy_cb;
        destroy_cb.client_data = NULL;
        if (XSetIMValues(eic->xim_input_method, XNDestroyCallback, &destroy_cb, NULL)) 
	  {
	     fprintf(stderr, "Could not set destroy callback to IM\n");
          }
    }

    // Get list of InputSyles supported by the XIM server
    if ((XGetIMValues(eic->xim_input_method, XNQueryInputStyle, &xim_styles, NULL)) || (!xim_styles)) 
      {
	 fprintf(stderr, "input method doesn't support any style\n");
	 XCloseIM(eic->xim_input_method);
	 return -1;
      }

    //Figure out the style we want to use, priority can be adjusted
    strncpy(tmp, (preedit_type ? preedit_type : "OverTheSpot,OffTheSpot,Root"), sizeof(tmp) - 1);

    preedit_type = "Root";
    // set xim_input_style
    for (found = 0, s = tmp; s[0] && !found; s = next_s + 1) 
      {
	 unsigned short i;
	 
	 for (; s[0] && isspace(s[0]); s++);
         if (!s[0]) break;
        
	 for (end = s; (end[0] && (end[0] != ',')); end++);
	 for (next_s = end--; ((end >= s) && isspace(end[0])); end--);
	 end[1] = '\0';

	 // Try one of our 3 choices
	 if (!strcmp(s, "OverTheSpot")) 
	   {
	      eic->xim_input_style = (XIMPreeditPosition | XIMStatusNothing);
	   } 
	 else if (!strcmp(s, "OffTheSpot")) 
	   {
	      eic->xim_input_style = (XIMPreeditArea | XIMStatusArea);
	   } 
	 else if (!strcmp(s, "Root")) 
	   {
	      eic->xim_input_style = (XIMPreeditNothing | XIMStatusNothing);
	   }

	 printf("trying %s\n", s);

	 // If our requested style is supported we set found and we are done
	 for (i = 0; i < xim_styles->count_styles; i++) 
	   {
	      if (eic->xim_input_style == xim_styles->supported_styles[i]) 
		{
		   printf("it works\n");
		   found = 1;
		   break;
		}
	   }
      }
    XFree(xim_styles);

    if (found == 0) 
      {
	 fprintf(stderr, "input method doesn't support my preedit type\n");
         XCloseIM(eic->xim_input_method);
	 eic->xim_input_method = NULL;
         return -1;
      }

    if ((eic->xim_input_style != (XIMPreeditNothing | XIMStatusNothing))
        && (eic->xim_input_style != (XIMPreeditArea | XIMStatusArea))
        && (eic->xim_input_style != (XIMPreeditPosition | XIMStatusNothing))) 
      {
	 fprintf(stderr, "This program does not support the preedit type\n");
         XCloseIM(eic->xim_input_method);
	 eic->xim_input_method = NULL;
         return -1;
      }

    if (eic->xim_input_style & XIMPreeditPosition) 
      {
	 _xim_set_size(&rect);
         xim_get_position(&spot);
         _xim_set_color(&fg, &bg);
	 preedit_attr = XVaCreateNestedList(0, 
		XNArea, &rect, 
		XNSpotLocation, &spot, 
		XNForeground, fg, /* Color */
		XNBackground, bg, /* Color */
		XNFontSet, data->fontset, 
		NULL);    
      } 
    else if (eic->xim_input_style & XIMPreeditArea) 
      {
	 _xim_set_color(&fg, &bg);
         /* The necessary width of preedit area is unknown until create input context. */
         needed_rect.width = 0;
         _xim_get_area(&rect, &status_rect, &needed_rect);
         preedit_attr = XVaCreateNestedList(0, 
	       XNArea, &rect, 
	       XNForeground, fg, 
	       XNBackground, bg, 
	       XNFontSet, data->fontset, 
	       NULL);

         status_attr = XVaCreateNestedList(0, 
	       XNArea, &status_rect, 
	       XNForeground, fg, 
	       XNBackground, bg, 
	       XNFontSet, data->fontset, 
	       NULL);

    }

    printf("im %d, style %d\n", eic->xim_input_method, eic->xim_input_style);

    /* Create the context */
    eic->xim_input_context = XCreateIC(eic->xim_input_method, 
	  XNInputStyle, eic->xim_input_style, 
	  XNClientWindow, data->win, 
	  XNFocusWindow, data->win,
	  preedit_attr ? XNPreeditAttributes : NULL, preedit_attr, 
	  status_attr ? XNStatusAttributes : NULL, status_attr,
	  NULL);
    
    if (preedit_attr) XFree(preedit_attr);
    if (status_attr) XFree(status_attr);

    if (eic->xim_input_context == NULL) 
      {
	 fprintf(stderr, "Failed to create input context\n");
         XCloseIM(eic->xim_input_method);
	 eic->xim_input_method = NULL;
         return -1;
      }

    // Update now
    if (eic->xim_input_style & XIMPreeditArea) xim_set_status_position(eic);

    return 0;
}

static void
_xim_set_size(XRectangle * size)
{
   XWindowAttributes attr;

   XGetWindowAttributes(data->display, data->win, &attr);

   size->x = attr.x;
   size->y = attr.y;
   size->width = attr.width;
   size->height = attr.height;
}

static void
_xim_set_color(unsigned long *fg, unsigned long *bg)
{
   *fg = BlackPixel(data->display, DefaultScreen(data->display));
   *bg = WhitePixel(data->display, DefaultScreen(data->display));
}

// If the cursor moves call this to change the spot
static void
_xim_send_spot(const Ecore_Input_Context * eic)
{
   XPoint spot;
   static XPoint oldSpot = { -1, -1 };
   XVaNestedList preedit_attr;


   if (eic == NULL || eic->xim_input_context == NULL) 
     {
	return;
     }
  
   // Get and set preedit position if needed 
   if (eic->xim_input_style & XIMPreeditPosition) 
     {
        xim_get_position(&spot);
        if (spot.x != oldSpot.x || spot.y != oldSpot.y) 
	  {
	     oldSpot.x = spot.x;
             oldSpot.y = spot.y;
             preedit_attr = XVaCreateNestedList(0, XNSpotLocation, &spot, NULL);
             XSetICValues(eic->xim_input_context, XNPreeditAttributes, preedit_attr, NULL);
             XFree(preedit_attr);
	  }
     }
}

static void
_xim_get_area(XRectangle * preedit_rect, XRectangle * status_rect, XRectangle * needed_rect)
{
    preedit_rect->x = 20;
    preedit_rect->y = 20;
    preedit_rect->width = 64;
    preedit_rect->height = 12;

    status_rect->x = 20;
    status_rect->y = 40;
    status_rect->width = 64;
    status_rect->height = 12;
}

static int
_event_loop(void)
{
   if (data->eic == NULL) return 1;

   for (;;)
     {
	XEvent e;

	XNextEvent(data->display, &e);

	  {
	     if (XFilterEvent(&e, e.xkey.window)) continue;
	  }

	switch (e.type)  
	  {
	   case FocusIn:
	      printf("Focus In\n");
	      if (data->eic && data->eic->xim_input_context)    
		{
		   XSetICFocus(data->eic->xim_input_context);
		}    
	      break;    
	   case FocusOut:
	      printf("Focus Out \n");
	      if (data->eic && data->eic->xim_input_context)
	  	{
		   XUnsetICFocus(data->eic->xim_input_context);
		}
	      break;
	   case MapNotify:
	   case Expose:
	      XSetForeground(data->display, data->gc, WhitePixel(data->display, DefaultScreen(data->display)));
	      XDrawLine(data->display, data->win, data->gc, 10, 10, 190, 190);
	      XDrawLine(data->display, data->win, data->gc, 190, 10, 10, 190);
	      XFlush(data->display);
	      break;
	   case KeyPress:
		{
 		   char str[128];
 
  		   KeySym sym;
   		   Status status;
    		   XComposeStatus comp_status;
     		   XKeyPressedEvent* keyev;

		   memset(str, 0, 128 * sizeof(char));
		   keyev = (XKeyPressedEvent*) &e;

		   if (data->eic->xim_input_context)
		     {
			printf("Window %d\n", e.xkey.window);
		     	Xutf8LookupString(data->eic->xim_input_context, keyev, str, 128, &sym, &status);
		     	printf("UTF8: Key down '%s', %d, (0x%x %d) ... ", str, sym, keyev->state, keyev->keycode);
		     }
      		   else
      		     {
      			XLookupString(keyev, str, 128, &sym, &comp_status);
      			printf("ASCII: Key down '%s', %d, (0x%x %d) ... ", str, sym, keyev->state, keyev->keycode);
      		     }
       		   break;
       		}
	   case KeyRelease:
       	      printf("Key up!\n ");
    	      break;
	  }
	_xim_send_spot(data->eic);
     }

   return 0;
}

int main() 
{
   int blackColor, whiteColor;
   int  xic_ev_mask;
   XClassHint class_hints;

   data = calloc(1, sizeof(Private_Data));
   if (data == NULL) return 1;

   data->display = XOpenDisplay(NULL);
   if (data->display == NULL) 
     {
	fprintf(stderr, "Failed to open display\n");
     }

   blackColor = BlackPixel(data->display, DefaultScreen(data->display));
   whiteColor = WhitePixel(data->display, DefaultScreen(data->display));
	          
   data->win = XCreateSimpleWindow(data->display, 
	 DefaultRootWindow(data->display), 0, 0, 200, 200, 0, blackColor, blackColor);

   printf("Created window ID %d\n", data->win);

   class_hints.res_name = "ecore_IM";
   class_hints.res_class = "Ecore_IM";
   XmbSetWMProperties(data->display, data->win, ".:Ecore IM:.", "ecore_icon", NULL, 0,
	 NULL, NULL, &class_hints);

   data->gc = XCreateGC(data->display, data->win, 0, (0));
   init_locale();

   /* Configure inputs and map window */
   xic_ev_mask = 0;
   if (data->eic->xim_input_context) 
     {
	XGetICValues (data->eic->xim_input_context, 
	      XNFilterEvents , 
	      &xic_ev_mask , NULL);
     }
   xic_ev_mask |=  KeyPressMask |
                   KeyReleaseMask |
                   ButtonPressMask |
                   ButtonReleaseMask |
                   EnterWindowMask |
                   LeaveWindowMask |
                   PointerMotionMask |
                   ExposureMask |
                   VisibilityChangeMask |
                   StructureNotifyMask |
                   FocusChangeMask |
                   PropertyChangeMask |
                   ColormapChangeMask;
   XSelectInput(data->display, data->win, xic_ev_mask);
   XMapWindow(data->display, data->win);

   return _event_loop();
}
