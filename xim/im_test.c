#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include <stdio.h>
#include <string.h>
#include <locale.h>

void edit_start_cb (XIM xim, XPointer p1, XPointer p2) 
{
   printf("---> Start IM!\n");
}

void edit_done_cb (XIM xim, XPointer p1, XPointer p2) 
{
   printf("---> Done IM!\n");
}

void edit_draw_cb (XIM xim, XPointer p1, XPointer p2) 
{
   printf("---> Draw IM!\n");
}

void edit_caret_cb (XIM xim, XPointer p1, XPointer p2) 
{
   printf("---> Caret IM!\n");
   
}

XIC get_ic(XIM xim, Window win) 
{
   XIC xic;
   XIMStyle input_style;
   XIMStyles* ecore_im_styles;
   int i;

   XIMCallback edit_start;
   XIMCallback edit_done; 
   XIMCallback edit_draw; 
   XIMCallback edit_caret; 

   /* Over the spot data */
   XWindowAttributes attr;
   Screen *screen;

   XRectangle rect; /* clipping rectangle (size of win */
   XPoint spot; /* Point where input window is over */
   unsigned long fg, bg; /* colors for input */
   XFontSet fs;

   /* To get the font */
   char *font;
   int mc;
   char **ml, *ds;

   XVaNestedList preedit_list;
   XVaNestedList status_list;
 
   int  xim_ev_mask;

   preedit_list = NULL;
   status_list = NULL;

   printf("Locale of IM '%s'\n", XLocaleOfIM(xim));

   /* Debug to get available styles */
   XGetIMValues(xim, XNQueryInputStyle, &ecore_im_styles, NULL);    
   for (i = 0; i < ecore_im_styles->count_styles; i++)
     {
	XIMStyle style;

	style = ecore_im_styles->supported_styles[i];
	printf("Style[%d] = '0x%x'\n", i, style);
     }
   XFree(ecore_im_styles);

/* Test Callbacks */
#if 0
   input_style = (XIMPreeditCallbacks | XIMStatusNothing);

   edit_start.client_data = NULL;
   edit_start.callback = (XIMProc) edit_start_cb;

   edit_done.client_data = NULL;
   edit_done.callback = (XIMProc) edit_done_cb;

   edit_draw.client_data = NULL;
   edit_draw.callback = (XIMProc) edit_draw_cb;

   edit_caret.client_data = NULL;
   edit_caret.callback = (XIMProc) edit_caret_cb;

   preedit_list = XVaCreateNestedList(0, 
	 XNPreeditStartCallback,&edit_start,
	 XNPreeditDrawCallback,&edit_draw,
	 XNPreeditDoneCallback,&edit_done,
	 XNPreeditCaretCallback,&edit_caret,
	 NULL);

   status_list = XVaCreateNestedList(0, 
	 XNStatusStartCallback,&edit_start,
	 XNStatusDrawCallback,&edit_draw,
	 XNStatusDoneCallback,&edit_done,
	 NULL);

#else	/* Over the spot */   

   XGetWindowAttributes(XDisplayOfIM(xim), win, &attr);

   rect.x = attr.x;
   rect.y = attr.y;
   rect.width = attr.width;
   rect.height = attr.height;
   printf("Clipping wid/hig %d/%d\n", rect.width, rect.height);

   spot.x = attr.width / 2;
   spot.y = attr.height / 2;

   screen = XDefaultScreenOfDisplay(XDisplayOfIM(xim));
   fg = screen->black_pixel;
   bg = screen->white_pixel;
   font = "-misc-fixed-medium-r-normal--7-*-*-*-c-*-iso10646-"
          ",-misc-fixed-*-r-*-*-*-120-*-*-*-*-*-*,*";
   fs = XCreateFontSet(XDisplayOfIM(xim), font, 
				     &ml, &mc, &ds ); 
   printf("Created fontset from %s, %d missing charsets, default string \"%s\".\n", 
                  font, mc, ds);

   for ( i = 0; i < mc; i++)
      {
         printf("Fontset %d: missing %s\n", fs, ml[i]);
      } 

   preedit_list = XVaCreateNestedList(0, 
	 XNArea, &rect,
	 XNSpotLocation, &spot,
         XNForeground, fg,
         XNBackground, bg,
         XNFontSet, fs,
	 NULL);

   if (mc) 
     {
        XFreeStringList(ml);   
     }

   /* Try over-the-spot */
   input_style = (XIMPreeditPosition | XIMStatusNothing);

#endif  

   printf("Trying to configure style = 0x%x\n", input_style);
   xic = XCreateIC(xim,
	 XNInputStyle, input_style,
	 XNClientWindow, win,
	 XNFocusWindow, win,  
	 preedit_list ? XNPreeditAttributes : NULL, preedit_list,
	 status_list ? XNStatusAttributes : NULL, status_list,       
	 NULL);

   if (preedit_list) XFree(preedit_list);
   if (status_list) XFree(status_list);
   if (!xic) fprintf(stderr, "Failed to get your IC!\n");

   xim_ev_mask = 0;
   if (xic) XGetICValues (xic , XNFilterEvents , &xim_ev_mask , NULL);
   printf("IM Returned mask 0x%x\n", xim_ev_mask);
   xim_ev_mask |=  KeyPressMask |
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
   XSelectInput(XDisplayOfIM(xim), win, xim_ev_mask);

  return xic;
}

void xim_spot_ping(XIC xic, Window win)
{
   /* Over the spot data */
   XWindowAttributes attr;
   XPoint spot; /* Point where input window is over */
   XVaNestedList preedit_list;

   XGetWindowAttributes(XDisplayOfIM(XIMOfIC(xic)), win, &attr);

   spot.x = attr.width / 2;
   spot.y = attr.height / 2;
   
   preedit_list = XVaCreateNestedList(0, 
	 XNSpotLocation, &spot,
	 NULL);
   XSetICValues(xic, XNPreeditAttributes, preedit_list, NULL);
   XFree(preedit_list);
}

int main() {
     Display *disp;
     XIM ecore_im;
     XIC ecore_ic;
     XClassHint class_hints;

     if (setlocale(LC_ALL, "") == NULL)
      {
	     fprintf(stderr, "ERROR: cannot set locale\n");
	         return 1;
		    }
 
     if (!XSupportsLocale())
      {
	     fprintf(stderr, "ERROR: X does not support locale\n");
	         return 1;
		    }
  
     if (XSetLocaleModifiers("") == NULL)
        fprintf(stderr, "WARNING: Cannot set locale modifiers\n");

     disp = XOpenDisplay(NULL);

     int blackColor = BlackPixel(disp, DefaultScreen(disp));
     int whiteColor = WhitePixel(disp, DefaultScreen(disp));
     Window win = XCreateSimpleWindow(disp, DefaultRootWindow(disp), 0, 0, 
				     200, 200, 0, blackColor, blackColor);

     class_hints.res_name = "ecore_IM";		
     class_hints.res_class = "Ecore_IM";			
					
     XmbSetWMProperties(disp, win, ".:Ecore IM:.", "ecore_icon", NULL, 0,
	   NULL, NULL, &class_hints);

     GC gc = XCreateGC(disp, win, 0, (0));
     
     ecore_im = XOpenIM(disp, NULL, "ecore_IM", "Ecore_IM");
     ecore_ic = get_ic(ecore_im, win);      

     XMapWindow(disp, win);

     for(;;) 
       {
            XEvent e;

	    XNextEvent(disp, &e);            
            printf("%d ", e.type);

	    if (XFilterEvent(&e, win)) 
              {
		 if (e.type == KeyPress || e.type == KeyRelease) 
                   {
		      XKeyEvent *ev = (XKeyEvent*) &e;						
		      printf (" Filtered 0x%X %d\n", ev->state, ev->keycode); 
		   }
                 continue;		 
	      }

	    switch (e.type) {
	       case MappingNotify:
		  printf("Mapping Notify!\n");
		  break;
	       case FocusIn:  
		  if (ecore_ic) 
                    {
			printf("Focus In\n");
                        XSetICFocus(ecore_ic);
                    }
		  break;
	       case FocusOut:	  
		  if (ecore_ic) 
		    {
			printf("Focus Out \n");
			XUnsetICFocus(ecore_ic);
		    }
		  break;
	       case MapNotify:
	       case Expose:
		  XSetForeground(disp, gc, whiteColor);
		  XDrawLine(disp, win, gc, 10, 10, 190, 190);
		  XDrawLine(disp, win, gc, 190, 10, 10, 190);
		  XFlush(disp);
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

		  if (ecore_ic)
		    {
		     	Xutf8LookupString(ecore_ic, keyev, str, 128, &sym, &status);
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

	      xim_spot_ping(ecore_ic, win);
      }
     XCloseIM(ecore_im);

     return XCloseDisplay(disp);

}
