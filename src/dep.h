#ifndef DEP_H
#define DEP_H

#include <iostream>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pthread.h>

#include <gtk/gtk.h>
#include <cairo.h>

using namespace std;

struct AppWindow {
    GtkWidget *window;
    GtkWidget *main_page;
    GtkWidget *box_vertical;
    GtkWidget *top_box_horizontal;
    GtkWidget *top_box_alignment;
    GtkWidget *hide_button;
    GtkWidget *exit_button;
    GtkWidget *main_page_box_scrolled;
    GtkWidget *status_box_alignment;
    GtkWidget *status_label;
    char statebuff[128];
    int width;
    int height;
};

struct IconWidget {
    GtkWidget *window;
    GtkWidget *box_horizontal;
    GtkWidget *box_vertical;
    GtkWidget *action_label;
    GtkWidget *app_label;
    GtkWidget *state_label;
    GtkWidget *settings_label;
    char icon_state;
    bool exits;
    char icon_name[128];
    char icon_path[1024];
};

struct IconPage {
    GtkWidget *window;
    GtkWidget *box_fixed;
    GtkWidget *about_icon;
    GtkWidget *about_icon_label;
    GtkWidget *settings_icon;
    GtkWidget *settings_icon_label;
    GtkWidget *new_icon;
    GtkWidget *new_icon_label;
    char about_icon_state;
    char settings_icon_state;
    char new_icon_state;
    int icon_location_x;
    int icon_location_y;
    int app_icon_number;
    int app_actual_icon_number;
    IconWidget *app_icons;
    IconWidget *new_app_icons;
    cairo_surface_t *icon_shape;
    cairo_surface_t *icon_normal;
    cairo_surface_t *icon_hover;
    cairo_surface_t *icon_pressed;
    cairo_surface_t *background;
    GdkPixmap *icon_mask;
    cairo_t *cairo_context;
    int width;
    int height;
    int current_icon;
};

struct AboutPage {
    GtkWidget *window;
    GtkWidget *box_vertical;
    GtkWidget *back_box_horizontal;
    GtkWidget *back_box_alignment;
    GtkWidget *info;
    GtkWidget *back_button;
};

struct IconSettingPage {
    GtkWidget *window;
    GtkWidget *box_vertical;
    GtkWidget *icon_box_horizontal;
    GtkWidget *exe_box_horizontal;
    GtkWidget *delete_box_horizontal;
    GtkWidget *delete_box_alignment;
    GtkWidget *icon_name_label;
    GtkWidget *icon_name_entry;
    GtkWidget *delete_icon_button;
    GtkWidget *exe_label;
    GtkWidget *exe_entry;
    GtkWidget *exe_browse_button;
    GtkWidget *back_icon_button;
};

struct ExeChooserPage {
    GtkWidget *window;
    GtkWidget *box_vertical;
    GtkWidget *action_box_horizontal;
    GtkWidget *action_box_alignment;
    GtkWidget *file_chooser_widget;
    GtkFileFilter *file_chooser_filter;
    GtkWidget *open_button;
    GtkWidget *cancel_button;
};

struct OPP {
    pid_t child_pid;
    int env_preparation_phase;
    bool child_busy;
    bool timera;
};

struct PidSave {
    pid_t pid;
    int current_icon;
    bool use;
};

struct PidSaveCollection {
    PidSave *pid_save;
    int collection_size;
};

typedef struct AppWindow AppWindow;
typedef struct IconWidget IconWidget;
typedef struct IconPage IconPage;
typedef struct AboutPage AboutPage;
typedef struct IconSettingPage IconSettingPage;
typedef struct ExeChooserPage ExeChooserPage;
typedef struct OPP OPP;
typedef PidSave PidSave;
typedef PidSaveCollection PidSaveCollection;

GdkPixbuf *create_pixbuf (const gchar *filename);
void add_pid (pid_t pid, int current_icon);
void remove_pid (pid_t pid);

#endif // DEP_H
