#include <gtk/gtk.h>
//Ref: https://developer.gnome.org/gtk3/stable/ch01s05.html
// a clone of tryDraw.c morphed into library form
#include "tc.h"
#include <stdlib.h>
#include <stdio.h>

/* Surface to store current scribbles
 */
static cairo_surface_t *surface = NULL;
cairo_t *cr;
/*  window size
 */
int drawWidth, drawHeight;
/*  simple wrappers around cairo functions to make do_draw_code
 *  as simple as possible. 
 */
startdraw() {
  cr = cairo_create(surface);
}
finishdraw() {
  cairo_stroke(cr);
  cairo_destroy(cr);
}
nextdraw() {
  cairo_new_sub_path(cr);
}
void rectangle (int x, int y, int w, int h) {
  cairo_rectangle (cr,  x,y,w,h);
}
void moveto (int x, int y) {
  cairo_move_to (cr,  x,y);
}
void lineto (int x, int y) {
  cairo_line_to (cr,  x,y);
}
#ifndef M_PI
#    define M_PI 3.14159265358979323846
#endif
void arc (int x, int y, int r, int b, int e) {
  cairo_arc (cr,  x,y,r,  b*(M_PI/180),  e*(M_PI/180)  );
}
void arcneg (int x, int y, int r, int b, int e) {
  cairo_arc_negative (cr,  x,y,r,  b*(M_PI/180),  e*(M_PI/180)  );
}
void ellipse(int x, int y, int width, int height){
  cairo_save (cr);
  cairo_translate (cr, x + width / 2., y + height / 2.);
  cairo_scale (cr, width / 2., height / 2.);
  cairo_arc (cr, 0., 0., 1., 0., 2 * M_PI);
  cairo_restore (cr);
}

enum fcn {FINISH,START,RECTANGLE,MOVETO,LINETO,NEXT,ELLIPSE,
  ARC,ARCNEG};

/* draw_this is advanced past len,width,height
 */
int draw_len;
do_draw_code(int *draw_this){
  int i=0;
  int segment = 0;
  while(i < draw_len){
    ++segment;
    switch( *(draw_this+i) ) {
      case FINISH:
        finishdraw();
        i += 99999999;
        break;
      case START:
        startdraw();
        i += 1;
        break;
      case RECTANGLE:
        rectangle(*(draw_this+i+1),*(draw_this+i+2),*(draw_this+i+3),*(draw_this+i+4));
        i += 5;
        break;
      case MOVETO:
        moveto(*(draw_this+i+1),*(draw_this+i+2));
        i += 3;
        break;
      case LINETO:
        lineto(*(draw_this+i+1),*(draw_this+i+2));
        i += 3;
        break;
      case NEXT:
        nextdraw();
        i += 1;
        break;
      case ELLIPSE:
        ellipse(*(draw_this+i+1),*(draw_this+i+2),*(draw_this+i+3),*(draw_this+i+4));
        i += 5;
        break;
      case ARC:
        arc(*(draw_this+i+1),*(draw_this+i+2),*(draw_this+i+3),*(draw_this+i+4),*(draw_this+i+5));
        i += 6;
        break;
      case ARCNEG:
        arcneg(*(draw_this+i+1),*(draw_this+i+2),*(draw_this+i+3),*(draw_this+i+4),*(draw_this+i+5));
        i += 6;
        break;
      default:
      g_print("Unknown draw_this at %d, segment %d\n",*(draw_this+1), segment);
      g_print("A frequent cause is START misuse: it must be first and only\n");      
        i += 99999999;
        break;
    }
  }
}

/*  From here down is infrastructure. */
static void
clear_surface (void)
{
  cairo_t *cr;
  cr = cairo_create (surface);
  cairo_set_source_rgb (cr, 1, 1, 1);
  cairo_paint (cr);
  cairo_destroy (cr);
}

/* Create a new surface of the appropriate size to store our scribbles */
static gboolean
configure_event_cb (GtkWidget         *widget,
                    GdkEventConfigure *event,
                    gpointer           data)
{
  if (surface)
    cairo_surface_destroy (surface);

  surface = gdk_window_create_similar_surface (
       gtk_widget_get_window (widget),
       CAIRO_CONTENT_COLOR,
       gtk_widget_get_allocated_width (widget),
       gtk_widget_get_allocated_height (widget));
  /* Initialize the surface to white */
  clear_surface ();
  /* We've handled the configure event, no need for further processing. */
  return TRUE;
}


/* Redraw the screen from the surface. Note that the ::draw
 * signal receives a ready-to-be-used cairo_t that is already
 * clipped to only draw the exposed areas of the widget
 */
static gboolean
draw_cb (GtkWidget *widget,
         cairo_t   *cr,
         gpointer   data)
{
  cairo_set_source_surface (cr, surface, 0, 0);
  cairo_paint (cr);
  do_draw_code(data);
  return FALSE;
}

static void
close_window (void)
{
  if (surface)
    cairo_surface_destroy (surface);
}

static void
activate (GtkApplication *app,
          gpointer        draw_this)
{
  GtkWidget *window;
  GtkWidget *frame;
  GtkWidget *drawing_area;

  window = gtk_application_window_new (app);
  gtk_window_set_title (GTK_WINDOW (window), "Drawing Area");

  g_signal_connect (window, "destroy", G_CALLBACK (close_window), NULL);

  gtk_container_set_border_width (GTK_CONTAINER (window), 8);

  frame = gtk_frame_new (NULL);
  gtk_frame_set_shadow_type (GTK_FRAME (frame), GTK_SHADOW_IN);
  gtk_container_add (GTK_CONTAINER (window), frame);

  drawing_area = gtk_drawing_area_new ();
  /* set a minimum size */
  gtk_widget_set_size_request (drawing_area, drawWidth, drawHeight);

  gtk_container_add (GTK_CONTAINER (frame), drawing_area);

  /* Signals used to handle the backing surface */
  g_signal_connect (drawing_area, "draw",
                    G_CALLBACK (draw_cb), draw_this);
  g_signal_connect (drawing_area,"configure-event",
                    G_CALLBACK (configure_event_cb), NULL);

  gtk_widget_show_all (window);
}

void dump(int *stuff, int n){
  int i;
  fprintf(stderr,"dump %d: ",n);
  for(i=0;i<n;++i)fprintf(stderr,"%d ",stuff[i]);
}

int init ( int *draw_glob )
{
  int status;
  int *draw_this = draw_glob+3;
  GtkApplication *app;
  draw_len = draw_glob[0]-3;
  drawWidth = draw_glob[1]; 
  drawHeight = draw_glob[2];
  app = gtk_application_new ("org.gtk.example", G_APPLICATION_FLAGS_NONE);
  g_signal_connect (app, "activate", G_CALLBACK (activate), draw_this);
  status = g_application_run (G_APPLICATION (app), 0, NULL);
  g_object_unref (app);
  return status;
}

int main(int argc, char** argv){
  if(argc<2){
    printf("usage: cairoGraphics filename\n");
    exit(1);
  }
  FILE *file;
  file = fopen(argv[1], "r");
  int num,i;
  int *code;
  fscanf(file, "%d", &num);
  code = calloc(num+9, sizeof(int));  // pad for safety
  *code = num;
  for(i=1; i<num; ++i){
    fscanf(file, "%d", code+i);
  }
  fclose(file);
/*  for(i=0; i<num; ++i){
    printf(" %d", *(code+i));
  }
*/
  init(code);
  free(code);
}
//enum fcn {FINISH,START,RECTANGLE,MOVETO,LINETO,NEXT,ELLIPSE,ARC};
//             0     1      2        3      4      5    6      7
