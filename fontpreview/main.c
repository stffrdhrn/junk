#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Evas.h>
#include <Ecore.h>
#include <Ecore_Evas.h>

#define TOK_STYLE ":style="

struct _E_Font_Properties 
{
   const char *name;
   Evas_List *styles;
};

typedef struct _E_Font_Properties E_Font_Properties;

static Evas_Hash * e_font_available_list_parse(Evas_List *list);
static void e_font_available_hash_free(Evas_Hash *hash);
static const char *e_font_fontconfig_name_get(const char *name, const char *style);

static int thumb_w = 202;
static int thumb_h = 48;
Evas_Object *e_font_thumb_gen(Ecore_Evas *ee, Evas *evas, const char *font);

int
main(int argc, char **argv)
{
    Ecore_Evas *ee;
    Evas *evas;

    evas_init();
    ecore_init();
    ecore_evas_init();

    ee = ecore_evas_buffer_new(thumb_w, thumb_h);
    ecore_evas_title_set(ee, "Thumnail Genrator");
    ecore_evas_show(ee);

    /* get a pointer our new Evas canvas */
    evas = ecore_evas_get(ee);

    {
       Evas_List *fonts;
       Evas_List *next;
       Evas_Hash *font_hash;
       int i;
       
       i = 0;

       fonts = evas_font_available_list(evas);

#if 0
       for (next = fonts; next; next = next->next) 
	 {
	    char *name;
	    Evas_Object * img;
	    char buf[55];

	    i++;
	    name = next->data;
	    printf("Got name %s\n", name);
            img = e_font_thumb_gen(ee, evas, name);
    
	    if (img)
	      {
	    sprintf(buf, "tmb/file_thumb-%d.png", i);
	    evas_object_image_save(img, buf, NULL, NULL);
	    evas_object_del(img);
	      }
	 }
#endif
       font_hash = e_font_available_list_parse(fonts);
       e_font_available_hash_free(font_hash);
       font_hash = NULL;

       evas_font_available_list_free(evas, fonts);
    }

    {
       Evas_List *methods;
       Evas_List *next;

       methods = evas_render_method_list();
       for (next = methods; next; next = next->next) 
	 {
	    char * method;

	    method = next->data;
	    printf("Available method %s\n", method);
	 }
       evas_render_method_list_free(methods);
    }

    ecore_evas_shutdown();
    ecore_shutdown();
    evas_shutdown();

    return 0;
}

Evas_Object *
e_font_thumb_gen(Ecore_Evas *ee, Evas *evas, const char *font)
{
   Evas_Object *bg;
   Evas_Object *text;
   Evas_Object *img;
   Evas_Coord x,y,w,h;

   bg = evas_object_rectangle_add(evas);
   evas_object_color_set(bg, 255, 255, 255, 255);
   evas_object_move(bg, 0, 0);
   evas_object_resize(bg, thumb_w, thumb_h);
   evas_object_name_set(bg, "font_thum_bg");

   text = evas_object_text_add(evas);
   evas_object_text_font_set(text, font, 12.0);
   evas_object_color_set(text, 0,0,0,255);
   evas_object_text_text_set(text, "Fox Jumped and Stuffs! わたし");
   evas_object_geometry_get(text, &x,&y,&w,&h);

   x = (thumb_w/2) - (w/2);
   y = (thumb_h/2) - (h/2);
   evas_object_move(text, x, y);

   printf("text geometry = %d %d %d %d\n", x,y,w,h);

   if (h == 0 || w == 0 ) 
     {
	img = NULL;
     }
   else
     {
	void *pix;

	evas_object_show(bg);
	evas_object_show(text);

        pix = (void *)ecore_evas_buffer_pixels_get(ee);
	    
	img = evas_object_image_add(evas);
	evas_object_image_size_set(img,thumb_w,thumb_h);
	    
	evas_object_image_data_copy_set(img, pix);
	evas_object_image_data_update_add(img, 0, 0, thumb_w, thumb_h);
     }
   evas_object_del(bg);
   evas_object_del(text);
	
   return img;
}

static Evas_Bool
_font_hash_free_cb(Evas_Hash *hash, const char *key, void *data, void *fdata)
{
   E_Font_Properties *efp;
   efp = data;
   printf("Name: %s\n", efp->name);
   printf("Styles:\n");
   while (efp->styles) 
     {
	const char *str;
	const char *fc_str;

	str = efp->styles->data;
	fc_str = e_font_fontconfig_name_get(efp->name, str);
	printf("\t%s '%s'\n", str, fc_str);
	if (fc_str) evas_stringshare_del(fc_str);
	if (str) evas_stringshare_del(str);
	efp->styles = evas_list_remove_list(efp->styles, efp->styles);

     }
   if (efp->name) evas_stringshare_del(efp->name);
   free(efp);

   return 1;
}

static void 
e_font_available_hash_free(Evas_Hash *hash)
{
   evas_hash_foreach(hash, _font_hash_free_cb, NULL);
   evas_hash_free(hash);
}

static const char *
e_font_fontconfig_name_get(const char *name, const char *style)
{
   char buf[256];
   if (name == NULL) return NULL;
   if (style == NULL) return NULL;

   snprintf(buf, 256, "%s"TOK_STYLE"%s", name, style);
   return evas_stringshare_add(buf);

}

static Evas_Hash *
e_font_available_hash_add(Evas_Hash *font_hash, const char *full_name)
{
	char *s1;

	s1 = strchr(full_name, ':');
	if (s1) 
	  {
	     char *s2;
	     char *name;
	     char *style;
	     int len = s1 - full_name;

	     name = calloc(sizeof(char), len + 1);
	     strncpy(name, full_name, len);

	     s2 = strchr(name, ',');
	     if (s2)
	       {
		  len = s2 - name;

		  name = realloc(name, sizeof(char) * len + 1);
		  memset(name, 0, sizeof(char) * len + 1);
		  strncpy(name, full_name, len);
	       }

	     if (strncmp(s1, TOK_STYLE, strlen(TOK_STYLE)) == 0)
	       {
		  E_Font_Properties *efp;

		  style = s1 + strlen(TOK_STYLE);

		  efp = evas_hash_find(font_hash, name);
		  if (efp == NULL)
		    {
		       efp = calloc(1, sizeof(E_Font_Properties));
		       efp->name = evas_stringshare_add(name);
		       font_hash = evas_hash_add(font_hash, name, efp);
		    } 
		  efp->styles = evas_list_append(efp->styles, evas_stringshare_add(style));
	       }
	     
	     free(name);
	  }
	return font_hash;
}

/*
TODO: default fonts + styles

   */
static Evas_Hash* 
e_font_available_list_parse(Evas_List *list)
{
   Evas_Hash *font_hash;
   Evas_List *next;

   font_hash = NULL;

   font_hash = e_font_available_hash_add(font_hash, "Sans:style=Regular");
   font_hash = e_font_available_hash_add(font_hash, "Sans:style=Bold");
   font_hash = e_font_available_hash_add(font_hash, "Sans:style=Italic");

   font_hash = e_font_available_hash_add(font_hash, "Serif:style=Regular");
   font_hash = e_font_available_hash_add(font_hash, "Serif:style=Bold");
   font_hash = e_font_available_hash_add(font_hash, "Serif:style=Italic");
   
   font_hash = e_font_available_hash_add(font_hash, "Monospace:style=Regular");
   font_hash = e_font_available_hash_add(font_hash, "Monospace:style=Bold");
   font_hash = e_font_available_hash_add(font_hash, "Monospace:style=Italic");

   for (next = list; next; next = next->next) 
     {
	font_hash = e_font_available_hash_add(font_hash, next->data);
     }

   return font_hash;
}


