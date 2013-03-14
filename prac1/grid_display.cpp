#include <stdio.h>
#include <allegro5/allegro.h>
#include<allegro5/allegro_primitives.h>
#include <grid_display.h>
 
void * showGridWindow(void * argument){
 
   ALLEGRO_DISPLAY *display = NULL;
 
   if(!al_init()) {
      fprintf(stderr, "failed to initialize allegro!\n");
      return NULL;
   }
 
   display = al_create_display(640, 480);
   if(!display) {
      fprintf(stderr, "failed to create display!\n");
      return NULL;
   }
   al_init_primitives_addon();
   al_clear_to_color(al_map_rgb(0,0,0));
 
   al_flip_display();
   al_draw_rectangle(0,0, 50, 50, al_map_rgb(255, 98, 100), 5);
 
   al_rest(10.0);
 
   al_destroy_display(display);
 
}