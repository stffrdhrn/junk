/* X Windows Example
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include <Ecore.h>
#include <Ecore_X.h>

static int 
handle_key_down(void *data, int type, void *event)
{
   Ecore_X_Event_Key_Down * e;

   e = (Ecore_X_Event_Key_Down*) event;

   printf ("Got: %s\n", e->key_compose);

   return 0;
}
   
static int 
handle_preedit_done(void *data, int type, void *event)
{
   printf("Preedit_Done\n");

   return 1;
}

static int 
handle_preedit_draw(void *data, int type, void *event)
{
   Ecore_X_Event_Preedit_Draw *e;

   e = event;

   printf("Preedit_Draw '%s'\n", e->text);

   return 1;
}

static int 
handle_status_draw(void *data, int type, void *event)
{
   Ecore_X_Event_Status_Draw *e;

   e = event;

   printf("Status_Draw '%s'\n", e->text);

   return 1;
}

static int 
handle_preedit_start(void *data, int type, void *event)
{
   printf("Preedit_Start\n");

   return 1;
}


int main (int argc, char *argv[]) {
  Ecore_X_Window win;

  ecore_init();
  ecore_x_init(NULL);
  
  win = ecore_x_window_new(0, 20, 20, 200, 200);
  ecore_x_icccm_name_class_set(win, "ecore_im_test", "Ecore_Im_Test");
  ecore_x_icccm_title_set(win, "...ecore IM eroce...");

  ecore_x_window_input_context_init(win, NULL);
  ecore_x_im_preedit_position_set(win, 20, 20);

  ecore_event_handler_add(ECORE_X_EVENT_KEY_DOWN, handle_key_down, NULL);
  //  ecore_event_handler_add(ECORE_X_EVENT_INPUT_METHOD_PREEDIT_START, handle_preedit_start, NULL);
  //  ecore_event_handler_add(ECORE_X_EVENT_INPUT_METHOD_PREEDIT_DONE, handle_preedit_done, NULL);
  ecore_event_handler_add(ECORE_X_EVENT_INPUT_METHOD_PREEDIT_DRAW, handle_preedit_draw, NULL);
  ecore_event_handler_add(ECORE_X_EVENT_INPUT_METHOD_STATUS_DRAW, handle_status_draw, NULL);

  ecore_x_window_show(win);
  ecore_x_flush(); // Ensure requests are sent to the server.
  
  ecore_main_loop_begin();

  ecore_x_shutdown();
  ecore_shutdown();
  return 0;
}
