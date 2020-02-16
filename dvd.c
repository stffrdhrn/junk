#include <Evas.h>
#include <Ecore.h>
#include <Ecore_Evas.h>
#include <Emotion.h>
#include <stdlib.h>
int main(int argc, char **argv) {
     Evas_Object *video;
     Ecore_Evas *ee;
	      
     ecore_evas_init();	         
     ecore_evas_show(ee = ecore_evas_software_x11_new(NULL, 0, 0, 0, 800, 600));
     
     video = emotion_object_add(ecore_evas_get(ee));		       
     emotion_object_init(video, "emotion_decoder_xine.so");
     emotion_object_file_set(video, "dvd:/");		          
     emotion_object_play_set(video, 1);
     evas_object_resize(video, 800, 600);			        
     evas_object_show(video);

     ecore_main_loop_begin();
     ecore_evas_shutdown();
}
