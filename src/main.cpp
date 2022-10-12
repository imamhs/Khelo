#include "dep.h"

AppWindow app_window;
IconPage icon_page;
AboutPage about_page;
IconSettingPage icon_settings_page;
ExeChooserPage exe_chooser_page;
OPP opp;

PidSaveCollection pid_save_collection;

void create_app_icon (void);
void create_main_icons (void);
void update_app_icon_location (void);
void rearrange_app_icons (void);
int load_resource (void);
void free_resource (void);
void create_icon_page (void);
void create_about_page (void);
void create_icon_settings_page (void);
void create_exe_chooser_page (void);
int load_games (void);
int save_games (void);

bool dir_exists (char *path);

int delete_directory (char *path);
void delete_dir_callback (int signal_number);
int create_env (char *env_path, char *arch, char *wine_bin_path, char *wine_lib_path);
void create_env_callback (int signal_number);
void prepare_environment (void);
static gboolean tloop (gpointer data);
int run_icon (char *env_path, char *wine_bin_path, char *wine_lib_path);
void run_icon_callback (int signal_number);

static gboolean gicon_press_callback (GtkWidget *widget, GdkEventButton *event, gpointer data);
static gboolean gicon_release_callback (GtkWidget *widget, GdkEventButton *event, gpointer data);
static gboolean gicon_enter_callback (GtkWidget *widget, GdkEvent *event, gpointer data);
static gboolean gicon_leave_callback (GtkWidget *widget, GdkEvent *event, gpointer data);
static gboolean gicon_expose_callback (GtkWidget *widget, GdkEventExpose *event, gpointer data);
static gboolean icon_press_callback (GtkWidget *widget, GdkEventButton *event, gpointer data);
static gboolean icon_release_callback (GtkWidget *widget, GdkEventButton *event, gpointer data);
static gboolean icon_enter_callback (GtkWidget *widget, GdkEvent *event, gpointer data);
static gboolean icon_leave_callback (GtkWidget *widget, GdkEvent *event, gpointer data);
static gboolean icon_expose_callback (GtkWidget *widget, GdkEventExpose *event, gpointer data);
static gboolean icon_page_expose_callback (GtkWidget *widget, GdkEventExpose *event, gpointer data);
static void iset_back_press_callback (GtkWidget *widget, gpointer data);
static void iset_browse_press_callback (GtkWidget *widget, gpointer data);
static void iset_detete_press_callback (GtkWidget *widget, gpointer data);
static void about_back_press_callback (GtkWidget *widget, gpointer data);
static void bexit_press_callback (GtkWidget *widget, gpointer data);
static void bhide_press_callback (GtkWidget *widget, gpointer data);
static void chooser_open_press_callback (GtkWidget *widget, gpointer data);
static void chooser_cancel_press_callback (GtkWidget *widget, gpointer data);
static gboolean window_expose_callback (GtkWidget *widget, GdkEventExpose *event, gpointer data);
static gboolean window_press_callback (GtkWidget *widget, GdkEventButton *event, gpointer data);
static void window_destroy_callback (GtkWidget *widget, gpointer data);
static gboolean window_delete_callback (GtkWidget *widget, GdkEvent *event, gpointer data);

int main (int argc, char *argv[])
{
    int r_load = 0;

    if (gtk_init_check (&argc, &argv) == FALSE) {
        fprintf (stderr, "Error: could not initialize GTK!\n");
        return 1;
    }

    r_load = load_resource ();

    if (r_load == 1) {
        fprintf (stderr, "Error: could not load image files!\n");
        return 1;
    }

    if (r_load == 2) {
        fprintf (stderr, "Error: could not shape icons!\n");
    }

    app_window.window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

    gtk_widget_set_events (app_window.window, GDK_BUTTON_PRESS_MASK | GDK_EXPOSURE_MASK);

    g_signal_connect (G_OBJECT (app_window.window), "delete-event", G_CALLBACK(window_delete_callback), NULL);
    g_signal_connect (G_OBJECT (app_window.window), "destroy", G_CALLBACK(window_destroy_callback), NULL);
    g_signal_connect (G_OBJECT (app_window.window), "button_press_event", G_CALLBACK (window_press_callback), NULL);
    g_signal_connect (G_OBJECT (app_window.window), "expose_event", G_CALLBACK (window_expose_callback), NULL);

    gtk_window_set_title (GTK_WINDOW(app_window.window), "Onthim PC play");
    gtk_window_set_icon (GTK_WINDOW(app_window.window), create_pixbuf("data/img/iconk.png"));
    gtk_window_set_position (GTK_WINDOW(app_window.window), GTK_WIN_POS_CENTER);
    gtk_window_set_default_size (GTK_WINDOW(app_window.window), 600, 430);
    gtk_container_set_border_width (GTK_CONTAINER(app_window.window), 0);
    gtk_window_set_resizable (GTK_WINDOW(app_window.window), TRUE);
    gtk_window_set_decorated (GTK_WINDOW (app_window.window), FALSE);
    gtk_widget_set_app_paintable (app_window.window, TRUE);

    app_window.top_box_horizontal = gtk_hbox_new (FALSE, 0);

    app_window.hide_button = gtk_button_new_with_label ("Hide");
    g_signal_connect (G_OBJECT(app_window.hide_button), "clicked", G_CALLBACK (bhide_press_callback), app_window.window);
    gtk_widget_set_size_request (app_window.hide_button, 100, -1);
    app_window.exit_button = gtk_button_new_with_label ("Exit");
    g_signal_connect (G_OBJECT(app_window.exit_button), "clicked", G_CALLBACK (bexit_press_callback), app_window.window);
    gtk_widget_set_size_request (app_window.exit_button, 100, -1);

    gtk_box_pack_start (GTK_BOX(app_window.top_box_horizontal), app_window.hide_button, FALSE, FALSE, 0);
    gtk_box_pack_start (GTK_BOX(app_window.top_box_horizontal), app_window.exit_button, FALSE, FALSE, 0);
    gtk_widget_show (app_window.hide_button);
    gtk_widget_show (app_window.exit_button);

    app_window.top_box_alignment = gtk_alignment_new (1, 0, 0, 0);
    gtk_container_add (GTK_CONTAINER(app_window.top_box_alignment), app_window.top_box_horizontal);
    gtk_widget_show (app_window.top_box_horizontal);

    app_window.main_page = gtk_event_box_new ();
    app_window.main_page_box_scrolled = gtk_scrolled_window_new (NULL, NULL);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW(app_window.main_page_box_scrolled), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_add_with_viewport (GTK_SCROLLED_WINDOW(app_window.main_page_box_scrolled), app_window.main_page);
    gtk_viewport_set_shadow_type (GTK_VIEWPORT(gtk_bin_get_child(GTK_BIN(app_window.main_page_box_scrolled))), GTK_SHADOW_NONE);
    gtk_widget_show (app_window.main_page);

    app_window.status_label = gtk_label_new ("Onthim PC play version 2014");
    app_window.status_box_alignment = gtk_alignment_new (0, 0, 0, 0);
    gtk_container_add (GTK_CONTAINER(app_window.status_box_alignment), app_window.status_label);
    gtk_widget_show (app_window.status_label);

    app_window.box_vertical = gtk_vbox_new (FALSE, 5);
    gtk_box_pack_start (GTK_BOX(app_window.box_vertical), app_window.top_box_alignment, FALSE, FALSE, 0);
    gtk_box_pack_start (GTK_BOX(app_window.box_vertical), app_window.main_page_box_scrolled, TRUE, TRUE, 0);
    gtk_box_pack_start (GTK_BOX(app_window.box_vertical), app_window.status_box_alignment, FALSE, FALSE, 0);
    gtk_widget_show (app_window.top_box_alignment);
    gtk_widget_show (app_window.main_page_box_scrolled);
    gtk_widget_show (app_window.status_box_alignment);

    gtk_container_add (GTK_CONTAINER(app_window.window), app_window.box_vertical);
    gtk_widget_show (app_window.box_vertical);

    gtk_widget_show (app_window.window);

    create_icon_page ();

    create_about_page ();

    create_icon_settings_page ();

    create_exe_chooser_page ();

    load_games ();

    prepare_environment ();

    gtk_main ();

    free_resource ();

    return EXIT_SUCCESS;
}

static gboolean window_delete_callback (GtkWidget *widget, GdkEvent *event, gpointer data)
{
    return FALSE;
}

static void window_destroy_callback (GtkWidget *widget, gpointer data)
{
    save_games ();
    gtk_main_quit ();
}

static gboolean window_press_callback (GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    if (event->type == GDK_BUTTON_PRESS)
    {
        switch (event->button)
	{
            case 1:
            if ((event->x + 10 > widget->allocation.width) && (event->y + 10 > widget->allocation.height))  gtk_window_begin_resize_drag (GTK_WINDOW (widget), GDK_WINDOW_EDGE_SOUTH_EAST, event->button, event->x_root, event->y_root, event->time);
            else if (event->x + 10 > widget->allocation.width)  gtk_window_begin_resize_drag (GTK_WINDOW (widget), GDK_WINDOW_EDGE_EAST, event->button, event->x_root, event->y_root, event->time);
            else if (event->y + 10 > widget->allocation.height) gtk_window_begin_resize_drag (GTK_WINDOW (widget), GDK_WINDOW_EDGE_SOUTH, event->button, event->x_root, event->y_root, event->time);
            else if (event->y < 20) gtk_window_begin_move_drag (GTK_WINDOW (widget), event->button, event->x_root, event->y_root, event->time);
            break;
	}
    }
    return FALSE;
}

static gboolean window_expose_callback (GtkWidget *widget, GdkEventExpose *event, gpointer data)
{
    int locx = 0, locy = 0;
    cairo_t *cr;
    cr = gdk_cairo_create (widget->window);
    while ((locy) < widget->allocation.height) {
        cairo_set_source_surface (cr, icon_page.background, locx, locy);
        cairo_paint (cr);
        if ((locx) > widget->allocation.width) {
            locx = 0;
            locy += 16;
        }
        else locx += 16;
    }
    cairo_destroy (cr);

    if ((app_window.width != widget->allocation.width) || (app_window.height != widget->allocation.height)){
        rearrange_app_icons ();
        app_window.width = widget->allocation.width;
        app_window.height = widget->allocation.height;
    }

    return FALSE;
}

static void bhide_press_callback (GtkWidget *widget, gpointer data)
{
    gtk_window_iconify (GTK_WINDOW(data));
}

static void bexit_press_callback (GtkWidget *widget, gpointer data)
{
    gtk_widget_destroy (GTK_WIDGET(data));
}

static void about_back_press_callback (GtkWidget *widget, gpointer data)
{
    g_object_ref (about_page.window);
    gtk_container_remove (GTK_CONTAINER(app_window.main_page), about_page.window);
    gtk_container_add (GTK_CONTAINER(app_window.main_page), icon_page.window);
    g_object_unref (icon_page.window);
    icon_page.about_icon_state = 0;
    gtk_widget_shape_combine_mask (icon_page.about_icon, icon_page.icon_mask, 0, 0);
    gtk_widget_shape_combine_mask (icon_page.settings_icon, icon_page.icon_mask, 0, 0);
    gtk_widget_shape_combine_mask (icon_page.new_icon, icon_page.icon_mask, 0, 0);
    for (int i=0; i < icon_page.app_icon_number; i++) if (icon_page.app_icons[i].exits == true) gtk_widget_shape_combine_mask (icon_page.app_icons[i].window, icon_page.icon_mask, 0, 0);
}

static void iset_back_press_callback (GtkWidget *widget, gpointer data)
{
    if ((strlen(gtk_entry_get_text(GTK_ENTRY(icon_settings_page.icon_name_entry))) != 0) && (strlen(gtk_entry_get_text(GTK_ENTRY(icon_settings_page.exe_entry))) != 0)) {
        strcpy (icon_page.app_icons[icon_page.current_icon-1].icon_name, gtk_entry_get_text(GTK_ENTRY(icon_settings_page.icon_name_entry)));
        strcpy (icon_page.app_icons[icon_page.current_icon-1].icon_path, gtk_entry_get_text(GTK_ENTRY(icon_settings_page.exe_entry)));

        for (int i=0; i < icon_page.app_icon_number; i++) {
            if (icon_page.app_icons[i].icon_state == 2) {
                icon_page.app_icons[i].icon_state = 0;
                break;
            }
        }
        g_object_ref (icon_settings_page.window);
        gtk_container_remove (GTK_CONTAINER(app_window.main_page), icon_settings_page.window);
        gtk_container_add (GTK_CONTAINER(app_window.main_page), icon_page.window);
        g_object_unref (icon_page.window);
        gtk_widget_shape_combine_mask (icon_page.about_icon, icon_page.icon_mask, 0, 0);
        gtk_widget_shape_combine_mask (icon_page.settings_icon, icon_page.icon_mask, 0, 0);
        gtk_widget_shape_combine_mask (icon_page.new_icon, icon_page.icon_mask, 0, 0);
        for (int i=0; i < icon_page.app_icon_number; i++) if (icon_page.app_icons[i].exits == true) gtk_widget_shape_combine_mask (icon_page.app_icons[i].window, icon_page.icon_mask, 0, 0);
    }
}

static void iset_browse_press_callback (GtkWidget *widget, gpointer data)
{
    g_object_ref (icon_settings_page.window);
    gtk_container_remove (GTK_CONTAINER(app_window.main_page), icon_settings_page.window);
    gtk_container_add (GTK_CONTAINER(app_window.main_page), exe_chooser_page.window);
    g_object_unref (exe_chooser_page.window);
}

static void iset_detete_press_callback (GtkWidget *widget, gpointer data)
{
    gtk_widget_destroy (icon_page.app_icons[icon_page.current_icon-1].action_label);
    gtk_widget_destroy (icon_page.app_icons[icon_page.current_icon-1].app_label);
    gtk_widget_destroy (icon_page.app_icons[icon_page.current_icon-1].state_label);
    gtk_widget_destroy (icon_page.app_icons[icon_page.current_icon-1].settings_label);
    gtk_widget_destroy (icon_page.app_icons[icon_page.current_icon-1].box_horizontal);
    gtk_widget_destroy (icon_page.app_icons[icon_page.current_icon-1].box_vertical);
    gtk_widget_destroy (icon_page.app_icons[icon_page.current_icon-1].window);
    icon_page.app_icons[icon_page.current_icon-1].exits = false;
    icon_page.app_actual_icon_number--;
    memset (app_window.statebuff, '\0', sizeof app_window.statebuff);
    sprintf (app_window.statebuff, "Onthim PC play version 2014, %d games", icon_page.app_actual_icon_number);
    gtk_label_set_text (GTK_LABEL(app_window.status_label), app_window.statebuff);
    rearrange_app_icons ();
    for (int i=0; i < icon_page.app_icon_number; i++) {
        if (icon_page.app_icons[i].icon_state == 2) {
            icon_page.app_icons[i].icon_state = 0;
            break;
        }
    }
    g_object_ref (icon_settings_page.window);
    gtk_container_remove (GTK_CONTAINER(app_window.main_page), icon_settings_page.window);
    gtk_container_add (GTK_CONTAINER(app_window.main_page), icon_page.window);
    g_object_unref (icon_page.window);
    gtk_widget_shape_combine_mask (icon_page.about_icon, icon_page.icon_mask, 0, 0);
    gtk_widget_shape_combine_mask (icon_page.settings_icon, icon_page.icon_mask, 0, 0);
    gtk_widget_shape_combine_mask (icon_page.new_icon, icon_page.icon_mask, 0, 0);
    for (int i=0; i < icon_page.app_icon_number; i++) if (icon_page.app_icons[i].exits == true) gtk_widget_shape_combine_mask (icon_page.app_icons[i].window, icon_page.icon_mask, 0, 0);
}

static void chooser_open_press_callback (GtkWidget *widget, gpointer data)
{
    char *file_uri = gtk_file_chooser_get_uri (GTK_FILE_CHOOSER(exe_chooser_page.file_chooser_widget));
    if (file_uri != NULL) {
        GFile *exe_gfile = g_file_new_for_uri (file_uri);
        char *exe_file_path = g_file_get_path (exe_gfile);
        char *exe_file_name = g_file_get_basename (exe_gfile);
        if (exe_file_name[strlen(exe_file_name)-4] == '.') {
            char ext_chc[5];
            memset (ext_chc, '\0', sizeof ext_chc);
            strcpy (ext_chc, strchr (exe_file_name, '.'));
            for (int i = 0; i < 4; i++) {
                char c = ext_chc[i];
                ext_chc[i] = tolower (c);
            }
            if (strcmp (ext_chc, ".exe") == 0) {
                char icon_name_buffer[128];
                memset (icon_name_buffer, '\0', sizeof icon_name_buffer);
                strncpy (icon_name_buffer, exe_file_name, strlen(exe_file_name)-4);
                gtk_entry_set_text (GTK_ENTRY(icon_settings_page.exe_entry), exe_file_path);
                gtk_entry_set_text (GTK_ENTRY(icon_settings_page.icon_name_entry), icon_name_buffer);
                g_object_ref (exe_chooser_page.window);
                gtk_container_remove (GTK_CONTAINER(app_window.main_page), exe_chooser_page.window);
                gtk_container_add (GTK_CONTAINER(app_window.main_page), icon_settings_page.window);
                g_object_unref (icon_settings_page.window);
            }
        }
        g_free (file_uri);
        g_free (exe_file_path);
        g_free (exe_file_name);
    }
}

static void chooser_cancel_press_callback (GtkWidget *widget, gpointer data)
{
    g_object_ref (exe_chooser_page.window);
    gtk_container_remove (GTK_CONTAINER(app_window.main_page), exe_chooser_page.window);
    gtk_container_add (GTK_CONTAINER(app_window.main_page), icon_settings_page.window);
    g_object_unref (icon_settings_page.window);
}


static gboolean icon_expose_callback (GtkWidget *widget, GdkEventExpose *event, gpointer data)
{
    if (strcmp((gchar *)data, "about") == 0) {
        if (icon_page.about_icon_state == 0) {
            cairo_t *cr;
            cr = gdk_cairo_create (widget->window);
            cairo_set_source_surface (cr, icon_page.icon_normal, 0, 0);
            cairo_paint (cr);
            cairo_destroy (cr);
        }
        else if (icon_page.about_icon_state == 1) {
            cairo_t *cr;
            cr = gdk_cairo_create (widget->window);
            cairo_set_source_surface (cr, icon_page.icon_hover, 0, 0);
            cairo_paint (cr);
            cairo_destroy (cr);
        }
        else if (icon_page.about_icon_state == 2) {
            cairo_t *cr;
            cr = gdk_cairo_create (widget->window);
            cairo_set_source_surface (cr, icon_page.icon_pressed, 0, 0);
            cairo_paint (cr);
            cairo_destroy (cr);
        }
    }
    if (strcmp((gchar *)data, "set") == 0) {
        if (icon_page.settings_icon_state == 0) {
            cairo_t *cr;
            cr = gdk_cairo_create (widget->window);
            cairo_set_source_surface (cr, icon_page.icon_normal, 0, 0);
            cairo_paint (cr);
            cairo_destroy (cr);
        }
        else if (icon_page.settings_icon_state == 1) {
            cairo_t *cr;
            cr = gdk_cairo_create (widget->window);
            cairo_set_source_surface (cr, icon_page.icon_hover, 0, 0);
            cairo_paint (cr);
            cairo_destroy (cr);
        }
        else if (icon_page.settings_icon_state == 2) {
            cairo_t *cr;
            cr = gdk_cairo_create (widget->window);
            cairo_set_source_surface (cr, icon_page.icon_pressed, 0, 0);
            cairo_paint (cr);
            cairo_destroy (cr);
        }
    }
    if (strcmp((gchar *)data, "new") == 0) {
        if (icon_page.new_icon_state == 0) {
            cairo_t *cr;
            cr = gdk_cairo_create (widget->window);
            cairo_set_source_surface (cr, icon_page.icon_normal, 0, 0);
            cairo_paint (cr);
            cairo_destroy (cr);
        }
        else if (icon_page.new_icon_state == 1) {
            cairo_t *cr;
            cr = gdk_cairo_create (widget->window);
            cairo_set_source_surface (cr, icon_page.icon_hover, 0, 0);
            cairo_paint (cr);
            cairo_destroy (cr);
        }
        else if (icon_page.new_icon_state == 2) {
            cairo_t *cr;
            cr = gdk_cairo_create (widget->window);
            cairo_set_source_surface (cr, icon_page.icon_pressed, 0, 0);
            cairo_paint (cr);
            cairo_destroy (cr);
        }
    }
    return FALSE;
}

static gboolean icon_press_callback (GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    if (event->type == GDK_BUTTON_PRESS)
    {
        switch (event->button)
	{
            case 1:
            if (strcmp((gchar *)data, "about") == 0) {
                icon_page.about_icon_state = 2;
                gtk_widget_queue_draw (widget);
                g_object_ref (icon_page.window);
                gtk_container_remove (GTK_CONTAINER(app_window.main_page), icon_page.window);
                gtk_container_add (GTK_CONTAINER(app_window.main_page), about_page.window);
                g_object_unref (about_page.window);
            }
            else if (strcmp((gchar *)data, "set") == 0) {
                icon_page.settings_icon_state = 2;
                gtk_widget_queue_draw (widget);
            }
            else if (strcmp((gchar *)data, "new") == 0) {
                icon_page.new_icon_state = 2;
                gtk_widget_queue_draw (widget);
                create_app_icon ();
            }
            break;
	}
    }
    return FALSE;
}

static gboolean icon_release_callback (GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    if (event->type == GDK_BUTTON_RELEASE)
    {
	switch (event->button)
	{
            case 1:
            if (strcmp((gchar *)data, "about") == 0) {
                icon_page.about_icon_state = 1;
                gtk_widget_queue_draw (widget);
            }
            else if (strcmp((gchar *)data, "set") == 0) {
                icon_page.settings_icon_state = 1;
                gtk_widget_queue_draw (widget);
            }
            else if (strcmp((gchar *)data, "new") == 0) {
                icon_page.new_icon_state = 1;
                gtk_widget_queue_draw (widget);
            }
            break;
	}
    }
    return FALSE;
}

static gboolean icon_enter_callback (GtkWidget *widget, GdkEvent *event, gpointer data)
{
    if (strcmp((gchar *)data, "about") == 0) {
        icon_page.about_icon_state = 1;
        gtk_widget_queue_draw (widget);
    }
    else if (strcmp((gchar *)data, "set") == 0) {
        icon_page.settings_icon_state = 1;
        gtk_widget_queue_draw (widget);
    }
    else if (strcmp((gchar *)data, "new") == 0) {
        icon_page.new_icon_state = 1;
        gtk_widget_queue_draw (widget);
    }
    return FALSE;
}

static gboolean icon_leave_callback (GtkWidget *widget, GdkEvent *event, gpointer data)
{
    if (strcmp((gchar *)data, "about") == 0) {
        icon_page.about_icon_state = 0;
        gtk_widget_queue_draw (widget);
    }
    else if (strcmp((gchar *)data, "set") == 0) {
        icon_page.settings_icon_state = 0;
        gtk_widget_queue_draw (widget);
    }
    else if (strcmp((gchar *)data, "new") == 0) {
        icon_page.new_icon_state = 0;
        gtk_widget_queue_draw (widget);
    }
    return FALSE;
}

static gboolean gicon_expose_callback (GtkWidget *widget, GdkEventExpose *event, gpointer data)
{
    int i = GPOINTER_TO_INT(data);
    if (icon_page.app_icons[i-1].icon_state == 0) {
        cairo_t *cr;
        cr = gdk_cairo_create (widget->window);
        cairo_set_source_surface (cr, icon_page.icon_normal, 0, 0);
        cairo_paint (cr);
        cairo_destroy (cr);
    }
    else if (icon_page.app_icons[i-1].icon_state == 1) {
        cairo_t *cr;
        cr = gdk_cairo_create (widget->window);
        cairo_set_source_surface (cr, icon_page.icon_hover, 0, 0);
        cairo_paint (cr);
        cairo_destroy (cr);
    }
    else if (icon_page.app_icons[i-1].icon_state == 2) {
        cairo_t *cr;
        cr = gdk_cairo_create (widget->window);
        cairo_set_source_surface (cr, icon_page.icon_pressed, 0, 0);
        cairo_paint (cr);
        cairo_destroy (cr);
    }
    return FALSE;
}

static gboolean gicon_press_callback (GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    int i = GPOINTER_TO_INT(data);
    icon_page.app_icons[i-1].icon_state = 2;
    gtk_widget_queue_draw (widget);
    icon_page.current_icon = i;
    if (event->type == GDK_BUTTON_PRESS)
    {
        switch (event->button)
	{
            case 1:
            if ((event->x < 20) && (event->y + 20 > widget->allocation.height)) {
                gtk_entry_set_text (GTK_ENTRY(icon_settings_page.icon_name_entry), icon_page.app_icons[icon_page.current_icon-1].icon_name);
                gtk_entry_set_text (GTK_ENTRY(icon_settings_page.exe_entry), icon_page.app_icons[icon_page.current_icon-1].icon_path);
                g_object_ref (icon_page.window);
                gtk_container_remove (GTK_CONTAINER(app_window.main_page), icon_page.window);
                gtk_container_add (GTK_CONTAINER(app_window.main_page), icon_settings_page.window);
                g_object_unref (icon_settings_page.window);
            }
            else {
                gtk_label_set_text (GTK_LABEL(icon_page.app_icons[icon_page.current_icon-1].state_label), "Running");
                run_icon ("env64", NULL, NULL);
            }
            break;
	}
    }
    return FALSE;
}

static gboolean gicon_release_callback (GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    int i = GPOINTER_TO_INT(data);
    icon_page.app_icons[i-1].icon_state = 1;
    gtk_widget_queue_draw (widget);
    return FALSE;
}

static gboolean gicon_enter_callback (GtkWidget *widget, GdkEvent *event, gpointer data)
{
    int i = GPOINTER_TO_INT(data);
    icon_page.app_icons[i-1].icon_state = 1;
    gtk_widget_queue_draw (widget);
    return FALSE;
}

static gboolean gicon_leave_callback (GtkWidget *widget, GdkEvent *event, gpointer data)
{
    int i = GPOINTER_TO_INT(data);
    icon_page.app_icons[i-1].icon_state = 0;
    gtk_widget_queue_draw (widget);
    return FALSE;
}

static gboolean icon_page_expose_callback (GtkWidget *widget, GdkEventExpose *event, gpointer data)
{
    int locx = 0, locy = 0;
    cairo_t *cr;
    cr = gdk_cairo_create (widget->window);
    while ((locy) < widget->allocation.height) {
        cairo_set_source_surface (cr, icon_page.background, locx, locy);
        cairo_paint (cr);
        if ((locx) > widget->allocation.width) {
            locx = 0;
            locy += 16;
        }
        else locx += 16;
    }
    cairo_destroy (cr);

    return FALSE;
}

int load_resource (void)
{
    memset (&pid_save_collection, 0, sizeof pid_save_collection);
    app_window.width = 600;
    app_window.height = 430;
    memset (app_window.statebuff, '\0', sizeof app_window.statebuff);
    icon_page.app_icon_number = 0;
    icon_page.app_actual_icon_number = 0;
    icon_page.icon_location_x = 10;
    icon_page.icon_location_y = 10;
    icon_page.about_icon_state = 0;
    icon_page.settings_icon_state = 0;
    icon_page.new_icon_state = 0;
    icon_page.width = 100;
    icon_page.height = 100;
    icon_page.app_icons = NULL;
    icon_page.new_app_icons = NULL;

    // load image files for the icons
    icon_page.icon_shape = cairo_image_surface_create_from_png("data/img/iconm.png");
    if (cairo_surface_status(icon_page.icon_shape) != CAIRO_STATUS_SUCCESS) return 1;
    icon_page.icon_normal = cairo_image_surface_create_from_png("data/img/iconn.png");
    if (cairo_surface_status(icon_page.icon_normal) != CAIRO_STATUS_SUCCESS) return 1;
    icon_page.icon_hover = cairo_image_surface_create_from_png("data/img/iconh.png");
    if (cairo_surface_status(icon_page.icon_hover) != CAIRO_STATUS_SUCCESS) return 1;
    icon_page.icon_pressed = cairo_image_surface_create_from_png("data/img/iconp.png");
    if (cairo_surface_status(icon_page.icon_pressed) != CAIRO_STATUS_SUCCESS) return 1;
    icon_page.background = cairo_image_surface_create_from_png("data/img/back.png");
    if (cairo_surface_status(icon_page.background) != CAIRO_STATUS_SUCCESS) return 1;

    // prepare mask for the icons
    icon_page.icon_mask = gdk_pixmap_new (NULL, 100, 100, 1);
    if (icon_page.icon_mask == NULL) return 2;
    icon_page.cairo_context = gdk_cairo_create (icon_page.icon_mask);
    if (icon_page.cairo_context == NULL) return 2;
    cairo_save (icon_page.cairo_context);
    cairo_rectangle (icon_page.cairo_context, 0, 0, 100, 100);
    cairo_set_operator (icon_page.cairo_context, CAIRO_OPERATOR_CLEAR);
    cairo_fill (icon_page.cairo_context);
    cairo_restore (icon_page.cairo_context);
    cairo_set_source_surface(icon_page.cairo_context, icon_page.icon_shape, 0, 0);
    cairo_paint(icon_page.cairo_context);
    cairo_destroy (icon_page.cairo_context);

    return 0;
}

void free_resource (void)
{
    free (icon_page.app_icons);

    cairo_surface_destroy (icon_page.icon_shape);
    cairo_surface_destroy (icon_page.icon_normal);
    cairo_surface_destroy (icon_page.icon_hover);
    cairo_surface_destroy (icon_page.icon_pressed);
    cairo_surface_destroy (icon_page.background);
}

void update_app_icon_location (void)
{
    if ((icon_page.icon_location_x + 260) > app_window.main_page_box_scrolled->allocation.width) {
        icon_page.icon_location_x = 10;
        icon_page.icon_location_y += 130;
    }
    else icon_page.icon_location_x += 130;
}

void rearrange_app_icons (void)
{
    icon_page.icon_location_x = 10;
    icon_page.icon_location_y = 130;
    for (int i=0; i < icon_page.app_icon_number; i++) {
        if (icon_page.app_icons[i].exits == true) {
            gtk_fixed_move (GTK_FIXED(icon_page.box_fixed), icon_page.app_icons[i].window, icon_page.icon_location_x, icon_page.icon_location_y);
            update_app_icon_location ();
        }
    }
}

void create_app_icon (void)
{
    icon_page.app_icon_number++;
    icon_page.new_app_icons = (IconWidget *) realloc (icon_page.app_icons, icon_page.app_icon_number * sizeof(IconWidget));

    if (icon_page.new_app_icons != NULL) {

        icon_page.app_actual_icon_number++;

        icon_page.app_icons = icon_page.new_app_icons;

        icon_page.app_icons[icon_page.app_icon_number-1].icon_state = 0;
        icon_page.app_icons[icon_page.app_icon_number-1].exits = true;
        memset (icon_page.app_icons[icon_page.app_icon_number-1].icon_name, '\0', sizeof icon_page.app_icons[icon_page.app_icon_number-1].icon_name);
        memset (icon_page.app_icons[icon_page.app_icon_number-1].icon_path, '\0', sizeof icon_page.app_icons[icon_page.app_icon_number-1].icon_path);
        icon_page.app_icons[icon_page.app_icon_number-1].window = gtk_event_box_new ();
        g_signal_connect (G_OBJECT(icon_page.app_icons[icon_page.app_icon_number-1].window), "expose_event", G_CALLBACK(gicon_expose_callback), GINT_TO_POINTER(icon_page.app_icon_number));
	g_signal_connect (G_OBJECT(icon_page.app_icons[icon_page.app_icon_number-1].window), "button_press_event", G_CALLBACK(gicon_press_callback), GINT_TO_POINTER(icon_page.app_icon_number));
	g_signal_connect (G_OBJECT(icon_page.app_icons[icon_page.app_icon_number-1].window), "button_release_event", G_CALLBACK(gicon_release_callback), GINT_TO_POINTER(icon_page.app_icon_number));
        g_signal_connect (G_OBJECT(icon_page.app_icons[icon_page.app_icon_number-1].window), "enter_notify_event", G_CALLBACK(gicon_enter_callback), GINT_TO_POINTER(icon_page.app_icon_number));
	g_signal_connect (G_OBJECT(icon_page.app_icons[icon_page.app_icon_number-1].window), "leave_notify_event", G_CALLBACK(gicon_leave_callback), GINT_TO_POINTER(icon_page.app_icon_number));
	gtk_widget_set_app_paintable(icon_page.app_icons[icon_page.app_icon_number-1].window, TRUE);
	gtk_widget_set_size_request (icon_page.app_icons[icon_page.app_icon_number-1].window, icon_page.width, icon_page.height);
	gtk_widget_shape_combine_mask (icon_page.app_icons[icon_page.app_icon_number-1].window, icon_page.icon_mask, 0, 0);

	icon_page.app_icons[icon_page.app_icon_number-1].action_label = gtk_label_new ("Action");
	icon_page.app_icons[icon_page.app_icon_number-1].app_label = gtk_label_new ("Application Name");
	icon_page.app_icons[icon_page.app_icon_number-1].state_label = gtk_label_new ("State");
	icon_page.app_icons[icon_page.app_icon_number-1].settings_label = gtk_label_new ("S");

        icon_page.app_icons[icon_page.app_icon_number-1].box_horizontal = gtk_hbox_new (FALSE, 2);
        gtk_box_pack_start (GTK_BOX(icon_page.app_icons[icon_page.app_icon_number-1].box_horizontal), icon_page.app_icons[icon_page.app_icon_number-1].settings_label, FALSE, FALSE, 0);
        gtk_box_pack_end (GTK_BOX(icon_page.app_icons[icon_page.app_icon_number-1].box_horizontal), icon_page.app_icons[icon_page.app_icon_number-1].state_label, TRUE, TRUE, 0);
        gtk_widget_show (icon_page.app_icons[icon_page.app_icon_number-1].settings_label);
        gtk_widget_show (icon_page.app_icons[icon_page.app_icon_number-1].state_label);

	icon_page.app_icons[icon_page.app_icon_number-1].box_vertical = gtk_vbox_new (FALSE, 10);
	gtk_box_pack_start (GTK_BOX(icon_page.app_icons[icon_page.app_icon_number-1].box_vertical), icon_page.app_icons[icon_page.app_icon_number-1].action_label, FALSE, FALSE, 0);
        gtk_box_pack_start (GTK_BOX(icon_page.app_icons[icon_page.app_icon_number-1].box_vertical), icon_page.app_icons[icon_page.app_icon_number-1].app_label, FALSE, FALSE, 0);
        gtk_box_pack_end (GTK_BOX(icon_page.app_icons[icon_page.app_icon_number-1].box_vertical), icon_page.app_icons[icon_page.app_icon_number-1].box_horizontal, FALSE, FALSE, 0);
        gtk_widget_show (icon_page.app_icons[icon_page.app_icon_number-1].action_label);
        gtk_widget_show (icon_page.app_icons[icon_page.app_icon_number-1].app_label);
        gtk_widget_show (icon_page.app_icons[icon_page.app_icon_number-1].box_horizontal);

	gtk_container_add (GTK_CONTAINER(icon_page.app_icons[icon_page.app_icon_number-1].window), icon_page.app_icons[icon_page.app_icon_number-1].box_vertical);
        gtk_widget_show (icon_page.app_icons[icon_page.app_icon_number-1].box_vertical);

	gtk_fixed_put (GTK_FIXED(icon_page.box_fixed), icon_page.app_icons[icon_page.app_icon_number-1].window, icon_page.icon_location_x, icon_page.icon_location_y);
	gtk_widget_show (icon_page.app_icons[icon_page.app_icon_number-1].window);

	update_app_icon_location ();

        memset (app_window.statebuff, '\0', sizeof app_window.statebuff);
	sprintf (app_window.statebuff, "Onthim PC play version 2014, %d games", icon_page.app_actual_icon_number);
	gtk_label_set_text (GTK_LABEL(app_window.status_label), app_window.statebuff);
    }
    else icon_page.app_icon_number--;
}

void create_icon_page (void)
{
    icon_page.about_icon_label = gtk_label_new ("About");
    icon_page.about_icon = gtk_event_box_new ();
    g_signal_connect (G_OBJECT(icon_page.about_icon), "expose_event", G_CALLBACK(icon_expose_callback), (gpointer) "about");
    g_signal_connect (G_OBJECT(icon_page.about_icon), "button_press_event", G_CALLBACK(icon_press_callback), (gpointer) "about");
    g_signal_connect (G_OBJECT(icon_page.about_icon), "button_release_event", G_CALLBACK(icon_release_callback), (gpointer) "about");
    g_signal_connect (G_OBJECT(icon_page.about_icon), "enter_notify_event", G_CALLBACK(icon_enter_callback), (gpointer) "about");
    g_signal_connect (G_OBJECT(icon_page.about_icon), "leave_notify_event", G_CALLBACK(icon_leave_callback), (gpointer) "about");
    gtk_widget_set_app_paintable(icon_page.about_icon, TRUE);
    gtk_widget_set_size_request (icon_page.about_icon, icon_page.width, icon_page.width);
    gtk_widget_shape_combine_mask (icon_page.about_icon, icon_page.icon_mask, 0, 0);
    gtk_container_add (GTK_CONTAINER(icon_page.about_icon), icon_page.about_icon_label);
    gtk_widget_show (icon_page.about_icon_label);

    icon_page.box_fixed = gtk_fixed_new ();
    gtk_fixed_put (GTK_FIXED(icon_page.box_fixed), icon_page.about_icon, icon_page.icon_location_x, icon_page.icon_location_x);
    gtk_widget_show (icon_page.about_icon);

    update_app_icon_location ();

    icon_page.settings_icon_label = gtk_label_new ("Settings");
    icon_page.settings_icon = gtk_event_box_new ();
    g_signal_connect (G_OBJECT(icon_page.settings_icon), "expose_event", G_CALLBACK(icon_expose_callback), (gpointer) "set");
    g_signal_connect (G_OBJECT(icon_page.settings_icon), "button_press_event", G_CALLBACK(icon_press_callback), (gpointer) "set");
    g_signal_connect (G_OBJECT(icon_page.settings_icon), "button_release_event", G_CALLBACK(icon_release_callback), (gpointer) "set");
    g_signal_connect (G_OBJECT(icon_page.settings_icon), "enter_notify_event", G_CALLBACK(icon_enter_callback), (gpointer) "set");
    g_signal_connect (G_OBJECT(icon_page.settings_icon), "leave_notify_event", G_CALLBACK(icon_leave_callback), (gpointer) "set");
    gtk_widget_set_app_paintable(icon_page.settings_icon, TRUE);
    gtk_widget_set_size_request (icon_page.settings_icon, icon_page.width, icon_page.height);
    gtk_widget_shape_combine_mask (icon_page.settings_icon, icon_page.icon_mask, 0, 0);
    gtk_container_add (GTK_CONTAINER(icon_page.settings_icon), icon_page.settings_icon_label);
    gtk_widget_show (icon_page.settings_icon_label);

    gtk_fixed_put (GTK_FIXED(icon_page.box_fixed), icon_page.settings_icon, icon_page.icon_location_x, icon_page.icon_location_y);
    gtk_widget_show (icon_page.settings_icon);

    update_app_icon_location ();

    icon_page.new_icon_label = gtk_label_new ("New");
    icon_page.new_icon = gtk_event_box_new ();
    g_signal_connect (G_OBJECT(icon_page.new_icon), "expose_event", G_CALLBACK(icon_expose_callback), (gpointer) "new");
    g_signal_connect (G_OBJECT(icon_page.new_icon), "button_press_event", G_CALLBACK(icon_press_callback), (gpointer) "new");
    g_signal_connect (G_OBJECT(icon_page.new_icon), "button_release_event", G_CALLBACK (icon_release_callback), (gpointer) "new");
    g_signal_connect (G_OBJECT(icon_page.new_icon), "enter_notify_event", G_CALLBACK(icon_enter_callback), (gpointer) "new");
    g_signal_connect (G_OBJECT(icon_page.new_icon), "leave_notify_event", G_CALLBACK(icon_leave_callback), (gpointer) "new");
    gtk_widget_set_app_paintable (icon_page.new_icon, TRUE);
    gtk_widget_set_size_request (icon_page.new_icon, icon_page.width, icon_page.height);
    gtk_widget_shape_combine_mask (icon_page.new_icon, icon_page.icon_mask, 0, 0);
    gtk_container_add (GTK_CONTAINER(icon_page.new_icon), icon_page.new_icon_label);
    gtk_widget_show (icon_page.new_icon_label);

    gtk_fixed_put (GTK_FIXED(icon_page.box_fixed), icon_page.new_icon, icon_page.icon_location_x, icon_page.icon_location_y);
    gtk_widget_show (icon_page.new_icon);

    icon_page.icon_location_x = 10;
    icon_page.icon_location_y = 130;

    icon_page.window = gtk_event_box_new ();
    g_signal_connect (G_OBJECT(icon_page.window), "expose_event", G_CALLBACK(icon_page_expose_callback), NULL);
    gtk_widget_set_app_paintable(icon_page.window, TRUE);
    gtk_container_add (GTK_CONTAINER(icon_page.window), icon_page.box_fixed);
    gtk_widget_show (icon_page.box_fixed);

    gtk_container_add (GTK_CONTAINER(app_window.main_page), icon_page.window);
    gtk_widget_show (icon_page.window);
}

void create_about_page (void)
{
    about_page.info = gtk_label_new ("Onthim PC play 2014\nPlay Windows games comfortably");
    about_page.back_button = gtk_button_new_with_label ("Back to games");
    g_signal_connect (G_OBJECT(about_page.back_button), "clicked", G_CALLBACK (about_back_press_callback), NULL);

    about_page.back_box_horizontal = gtk_vbox_new (FALSE, 0);
    gtk_box_pack_start (GTK_BOX(about_page.back_box_horizontal), about_page.back_button, FALSE, FALSE, 0);
    gtk_widget_show (about_page.back_button);

    about_page.back_box_alignment = gtk_alignment_new (0, 0, 0, 0);
    gtk_container_add (GTK_CONTAINER(about_page.back_box_alignment), about_page.back_box_horizontal);
    gtk_widget_show (about_page.back_box_horizontal);

    about_page.box_vertical = gtk_vbox_new (FALSE, 0);
    gtk_box_pack_start (GTK_BOX(about_page.box_vertical), about_page.info, TRUE, TRUE, 0);
    gtk_box_pack_start (GTK_BOX(about_page.box_vertical), about_page.back_box_alignment, FALSE, FALSE, 0);
    gtk_widget_show (about_page.info);
    gtk_widget_show (about_page.back_box_alignment);

    about_page.window = gtk_event_box_new ();
    gtk_container_add (GTK_CONTAINER(about_page.window), about_page.box_vertical);
    gtk_widget_show (about_page.box_vertical);
    gtk_widget_show (about_page.window);
    g_object_ref_sink (about_page.window);
}

void create_icon_settings_page (void)
{
    icon_settings_page.icon_name_label = gtk_label_new ("Game Name:");
    icon_settings_page.icon_name_entry = gtk_entry_new ();
    icon_settings_page.back_icon_button = gtk_button_new_with_label ("Back to games");
    g_signal_connect (G_OBJECT(icon_settings_page.back_icon_button), "clicked", G_CALLBACK (iset_back_press_callback), NULL);

    icon_settings_page.icon_box_horizontal = gtk_hbox_new (FALSE, 0);
    gtk_box_pack_start (GTK_BOX(icon_settings_page.icon_box_horizontal), icon_settings_page.icon_name_label, FALSE, FALSE, 0);
    gtk_box_pack_start (GTK_BOX(icon_settings_page.icon_box_horizontal), icon_settings_page.icon_name_entry, TRUE, TRUE, 0);
    gtk_box_pack_end (GTK_BOX(icon_settings_page.icon_box_horizontal), icon_settings_page.back_icon_button, FALSE, FALSE, 0);
    gtk_widget_show (icon_settings_page.icon_name_label);
    gtk_widget_show (icon_settings_page.icon_name_entry);
    gtk_widget_show (icon_settings_page.back_icon_button);

    icon_settings_page.exe_label = gtk_label_new ("Executable file:");
    icon_settings_page.exe_entry = gtk_entry_new ();
    icon_settings_page.exe_browse_button = gtk_button_new_with_label ("Browse");
    g_signal_connect (G_OBJECT(icon_settings_page.exe_browse_button), "clicked", G_CALLBACK (iset_browse_press_callback), NULL);

    icon_settings_page.exe_box_horizontal = gtk_hbox_new (FALSE, 0);
    gtk_box_pack_start (GTK_BOX(icon_settings_page.exe_box_horizontal), icon_settings_page.exe_label, FALSE, FALSE, 0);
    gtk_box_pack_start (GTK_BOX(icon_settings_page.exe_box_horizontal), icon_settings_page.exe_entry, TRUE, TRUE, 0);
    gtk_box_pack_end (GTK_BOX(icon_settings_page.exe_box_horizontal), icon_settings_page.exe_browse_button, FALSE, FALSE, 0);
    gtk_widget_show (icon_settings_page.exe_label);
    gtk_widget_show (icon_settings_page.exe_entry);
    gtk_widget_show (icon_settings_page.exe_browse_button);

    icon_settings_page.delete_icon_button = gtk_button_new_with_label ("Delete");
    g_signal_connect (G_OBJECT(icon_settings_page.delete_icon_button), "clicked", G_CALLBACK (iset_detete_press_callback), NULL);

    icon_settings_page.delete_box_horizontal = gtk_hbox_new (FALSE, 0);
    gtk_box_pack_start (GTK_BOX(icon_settings_page.delete_box_horizontal), icon_settings_page.delete_icon_button, FALSE, FALSE, 0);
    gtk_widget_show (icon_settings_page.delete_icon_button);

    icon_settings_page.delete_box_alignment = gtk_alignment_new (1, 0, 0, 0);
    gtk_container_add (GTK_CONTAINER(icon_settings_page.delete_box_alignment), icon_settings_page.delete_box_horizontal);
    gtk_widget_show (icon_settings_page.delete_box_horizontal);

    icon_settings_page.box_vertical = gtk_vbox_new (FALSE, 0);
    gtk_box_pack_start (GTK_BOX(icon_settings_page.box_vertical), icon_settings_page.icon_box_horizontal, FALSE, FALSE, 0);
    gtk_box_pack_start (GTK_BOX(icon_settings_page.box_vertical), icon_settings_page.exe_box_horizontal, FALSE, FALSE, 0);
    gtk_box_pack_end (GTK_BOX(icon_settings_page.box_vertical), icon_settings_page.delete_box_alignment, FALSE, FALSE, 0);
    gtk_widget_show (icon_settings_page.icon_box_horizontal);
    gtk_widget_show (icon_settings_page.exe_box_horizontal);
    gtk_widget_show (icon_settings_page.delete_box_alignment);

    icon_settings_page.window = gtk_event_box_new ();
    gtk_container_add (GTK_CONTAINER(icon_settings_page.window), icon_settings_page.box_vertical);
    gtk_widget_show (icon_settings_page.box_vertical);
    gtk_widget_show (icon_settings_page.window);
    g_object_ref_sink (icon_settings_page.window);
}

void create_exe_chooser_page (void)
{
    exe_chooser_page.open_button = gtk_button_new_with_label ("Open");
    g_signal_connect (G_OBJECT(exe_chooser_page.open_button), "clicked", G_CALLBACK (chooser_open_press_callback), NULL);
    exe_chooser_page.cancel_button = gtk_button_new_with_label ("Cancel");
    g_signal_connect (G_OBJECT(exe_chooser_page.cancel_button), "clicked", G_CALLBACK (chooser_cancel_press_callback), NULL);

    exe_chooser_page.action_box_horizontal = gtk_hbox_new (FALSE, 0);
    gtk_box_pack_start (GTK_BOX(exe_chooser_page.action_box_horizontal), exe_chooser_page.open_button, FALSE, FALSE, 0);
    gtk_box_pack_start (GTK_BOX(exe_chooser_page.action_box_horizontal), exe_chooser_page.cancel_button, FALSE, FALSE, 0);
    gtk_widget_show (exe_chooser_page.open_button);
    gtk_widget_show (exe_chooser_page.cancel_button);

    exe_chooser_page.action_box_alignment = gtk_alignment_new (1, 0, 0, 0);
    gtk_container_add (GTK_CONTAINER(exe_chooser_page.action_box_alignment), exe_chooser_page.action_box_horizontal);
    gtk_widget_show (exe_chooser_page.action_box_horizontal);

    exe_chooser_page.file_chooser_filter = gtk_file_filter_new ();
    gtk_file_filter_add_pattern (exe_chooser_page.file_chooser_filter, "*.exe");
    gtk_file_filter_add_pattern (exe_chooser_page.file_chooser_filter, "*.EXE");

    exe_chooser_page.file_chooser_widget = gtk_file_chooser_widget_new (GTK_FILE_CHOOSER_ACTION_OPEN);
    gtk_file_chooser_set_local_only (GTK_FILE_CHOOSER(exe_chooser_page.file_chooser_widget), true);
    gtk_file_chooser_set_select_multiple (GTK_FILE_CHOOSER(exe_chooser_page.file_chooser_widget), true);
    gtk_file_chooser_set_filter (GTK_FILE_CHOOSER(exe_chooser_page.file_chooser_widget), GTK_FILE_FILTER(exe_chooser_page.file_chooser_filter));

    exe_chooser_page.box_vertical = gtk_vbox_new (FALSE, 0);
    gtk_box_pack_start (GTK_BOX(exe_chooser_page.box_vertical), exe_chooser_page.file_chooser_widget, TRUE, TRUE, 0);
    gtk_box_pack_end (GTK_BOX(exe_chooser_page.box_vertical), exe_chooser_page.action_box_alignment, FALSE, FALSE, 0);
    gtk_widget_show (exe_chooser_page.file_chooser_widget);
    gtk_widget_show (exe_chooser_page.action_box_alignment);

    exe_chooser_page.window = gtk_event_box_new ();
    gtk_container_add (GTK_CONTAINER(exe_chooser_page.window), exe_chooser_page.box_vertical);
    gtk_widget_show (exe_chooser_page.box_vertical);
    gtk_widget_show (exe_chooser_page.window);
    g_object_ref_sink (exe_chooser_page.window);
}

int load_games (void)
{
    FILE *filep;

    filep = fopen ("data/gameslist.txt","r");

    if (filep != NULL)
    {
        for (char read_line_buffer [8192]; fgets (read_line_buffer, 8192, filep) != NULL; memset (read_line_buffer, '\0', sizeof read_line_buffer)) {
            char *data = strtok (read_line_buffer, ",\n");
            int c = 0;
            create_app_icon ();
            while (data != NULL)
            {
                if (c == 0) {
                    strcpy(icon_page.app_icons[icon_page.app_icon_number-1].icon_name, data);
                }
                else if (c == 1) {
                    strcpy(icon_page.app_icons[icon_page.app_icon_number-1].icon_path, data);
                }
                else break;
                data = strtok (NULL, ",\n");
                c++;
            }
        }
        fclose (filep);
    }
    return 0;
}

int save_games (void)
{
    FILE *filep;

    filep = fopen ("data/gameslist.txt","w+");

    if (filep != NULL)
    {
        for (int i=0; i < icon_page.app_icon_number; i++) {
            if (icon_page.app_icons[i].exits == true) {
                char write_line_buffer [8192];
                memset (write_line_buffer, '\0', sizeof write_line_buffer);
                sprintf (write_line_buffer, "%s,%s\n", icon_page.app_icons[i].icon_name, icon_page.app_icons[i].icon_path);
                fputs (write_line_buffer, filep);
            }
        }
        fclose (filep);
    }
    return 0;
}

bool dir_exists (char *path)
{
    GDir *dfp = g_dir_open (path, 0, NULL);
    if (dfp != NULL) {
        g_dir_close (dfp);
        return true;
    }
    else return false;
}

void delete_dir_callback (int signal_number)
{
    int pstat = 0;
    int cpid = waitpid (opp.child_pid, &pstat, WUNTRACED);

    if (WEXITSTATUS(pstat) == 0) {
        printf ("%d Dir delete successful\n", cpid);
        opp.child_busy = false;
    }
    else {
        printf ("%d Dir delete unsuccessful\n", cpid);
        opp.child_busy = false;
    }
}

int delete_directory (char *path)
{
    char comm[1024];
    pid_t cpid;
    struct sigaction sa;

    memset (comm, 0, sizeof comm);
    memset (&sa, 0, sizeof sa);

    sprintf (comm, "rm -R \"%s\"", path);

    cpid = fork ();

    if (cpid == -1) {
        opp.child_busy = false;
        return 1;
    }
    else if (cpid == 0) {
        int rval = system (comm);
        _exit (WEXITSTATUS(rval));
    }
    else {
        opp.child_pid = cpid;
        sa.sa_handler = &delete_dir_callback;
        sigaction (SIGCHLD, &sa, NULL);
    }
    return 0;
}

void create_env_callback (int signal_number)
{
    int pstat = 0;
    waitpid (opp.child_pid, &pstat, WUNTRACED);

    if (WEXITSTATUS(pstat) == 0) {
        if (opp.env_preparation_phase == 0) {
            printf ("finished env32\n");
            opp.env_preparation_phase = 1;
            opp.child_busy = false;
        }
        else if (opp.env_preparation_phase == 1) {
            printf ("finished env64\n");
            opp.env_preparation_phase = 2;
            opp.child_busy = false;
        }
    }
    else {
        if (opp.env_preparation_phase == 0) {
            printf ("error finishing env32\n");
            opp.env_preparation_phase = 1;
            opp.child_busy = false;
        }
        else if (opp.env_preparation_phase == 1) {
            printf ("error finishing env64\n");
            opp.env_preparation_phase = 2;
            opp.child_busy = false;
        }
    }
}

int create_env (char *env_path, char *arch, char *wine_bin_path, char *wine_lib_path)
{
    if (env_path == NULL || arch == NULL) {
        opp.child_busy = false;
        return 1;
    }
    if (wine_bin_path != NULL && wine_lib_path != NULL) {

    }
    else {
        char comm[8192];
        pid_t cpid;
        struct sigaction sa;

        memset (comm, 0, sizeof comm);
        memset (&sa, 0, sizeof sa);
        opp.child_pid = 0;

        sprintf (comm, "cd data; WINEPREFIX=\"`pwd`/%s\" WINEARCH=%s wineboot", env_path, arch);

        cpid = fork ();

        if (cpid == -1) {
            if (opp.env_preparation_phase == 0) {
                opp.env_preparation_phase = 1;
                opp.child_busy = false;
            }
            else if (opp.env_preparation_phase == 1) {
                opp.env_preparation_phase = 2;
                opp.child_busy = false;
            }
            return 1;
        }
        else if (cpid == 0) {
            int rval = system (comm);
            _exit (WEXITSTATUS(rval));
        }
        else {
            opp.child_pid = cpid;
            sa.sa_handler = &create_env_callback;
            sigaction (SIGCHLD, &sa, NULL);
        }
    }
    return 0;
}

static gboolean tloop (gpointer data)
{
    if (opp.env_preparation_phase == 0 && opp.child_busy == false) {
        if (dir_exists ("data/env32") == false) {
            printf ("env32 doest not exist\n");
            opp.child_busy = true;
            create_env ("env32", "win32", NULL, NULL);
        }
        else opp.env_preparation_phase = 1;
    }
    else if (opp.env_preparation_phase == 1 && opp.child_busy == false) {
        if (dir_exists ("data/env64") == false) {
            printf ("env64 does not exist\n");
            opp.child_busy = true;
            create_env ("env64", "win64", NULL, NULL);
        }
        else opp.env_preparation_phase = 2;
    }
    else if (opp.env_preparation_phase == 2) opp.timera = false;

    return opp.timera;
}

void prepare_environment (void)
{
    opp.env_preparation_phase = 0;
    opp.child_busy = false;
    opp.timera = true;

    g_timeout_add (1000, tloop, NULL);
}

void run_icon_callback (int signal_number)
{
    int pstat = 0;
    int cpid = waitpid (-1, &pstat, WUNTRACED);
    int icon_current = 0;

    for (int i = 0; i < pid_save_collection.collection_size; i++) {
        if (pid_save_collection.pid_save[i].pid == cpid) {
            icon_current = pid_save_collection.pid_save[i].current_icon;
            pid_save_collection.pid_save[i].use = false;
            if (WEXITSTATUS(pstat) == 0) {
                printf ("%d app closed successfully\n", cpid);
                gtk_label_set_text (GTK_LABEL(icon_page.app_icons[icon_current-1].state_label), "Not Running");
            }
            else {
                printf ("%d app closed unsuccessfully\n", cpid);
                gtk_label_set_text (GTK_LABEL(icon_page.app_icons[icon_current-1].state_label), "Crashed");
            }
            break;
        }
    }
}

int run_icon (char *env_path, char *wine_bin_path, char *wine_lib_path)
{
    if (env_path == NULL) return 1;
    if (wine_bin_path != NULL && wine_lib_path != NULL) {

    }
    else {
        char comm[8192];
        pid_t cpid;
        struct sigaction sa;

        memset (comm, 0, sizeof comm);
        memset (&sa, 0, sizeof sa);

        char *file_dir = g_path_get_dirname (icon_page.app_icons[icon_page.current_icon-1].icon_path);
        char *file_name = g_path_get_basename (icon_page.app_icons[icon_page.current_icon-1].icon_path);
        char *current_dir = g_get_current_dir ();

        sprintf (comm, "cd \"%s\" ; WINEDEBUG=-all WINEPREFIX=\"%s/data/%s\" wine \"%s\"", file_dir, current_dir, env_path, file_name);

        g_free (file_dir);
        g_free (file_name);
        g_free (current_dir);

        cpid = fork ();

        if (cpid == -1) return 1;
        else if (cpid == 0) {
            int rval = system (comm);
            _exit (WEXITSTATUS(rval));
        }
        else {
            add_pid (cpid, icon_page.current_icon);
            sa.sa_handler = &run_icon_callback;
            sigaction (SIGCHLD, &sa, NULL);
        }
    }
    return 0;
}

