#include "dep.h"

extern PidSaveCollection pid_save_collection;

GdkPixbuf *create_pixbuf (const char *filename)
{
    GdkPixbuf *pixbuf;
    GError *error = NULL;
    pixbuf = gdk_pixbuf_new_from_file(filename, &error);
    if(!pixbuf) {
        fprintf(stderr, "%s\n", error->message);
        g_error_free(error);
    }
    return pixbuf;
}

void add_pid (pid_t pid, int current_icon)
{
    int new_pid = 1;
    PidSave *new_pid_save = NULL;

    if (pid_save_collection.pid_save == NULL) {
        pid_save_collection.collection_size++;
        new_pid_save = (PidSave *) realloc (pid_save_collection.pid_save, pid_save_collection.collection_size * sizeof (PidSave));
        if (new_pid_save == NULL) {
            pid_save_collection.collection_size--;
            return;
        }
        else {
            pid_save_collection.pid_save = new_pid_save;
            pid_save_collection.pid_save[pid_save_collection.collection_size-1].pid = pid;
            pid_save_collection.pid_save[pid_save_collection.collection_size-1].current_icon = current_icon;
            pid_save_collection.pid_save[pid_save_collection.collection_size-1].use = true;
        }
        return;
    }
    for (int i = 0; i < pid_save_collection.collection_size; i++) {
        if (pid_save_collection.pid_save[i].pid == pid && pid_save_collection.pid_save[i].use == false) {
            pid_save_collection.pid_save[i].current_icon = current_icon;
            pid_save_collection.pid_save[i].use = true;
            new_pid = 0;
            break;
        }
    }
    if (new_pid == 1) {
        pid_save_collection.collection_size++;
        new_pid_save = (PidSave *) realloc (pid_save_collection.pid_save, pid_save_collection.collection_size * sizeof (PidSave));
        if (new_pid_save == NULL) {
            pid_save_collection.collection_size--;
            return;
        }
        else {
            pid_save_collection.pid_save = new_pid_save;
            pid_save_collection.pid_save[pid_save_collection.collection_size-1].pid = pid;
            pid_save_collection.pid_save[pid_save_collection.collection_size-1].current_icon = current_icon;
            pid_save_collection.pid_save[pid_save_collection.collection_size-1].use = true;
        }
    }
}

void remove_pid (pid_t pid)
{
    for (int i = 0; i < pid_save_collection.collection_size; i++) {
        if (pid_save_collection.pid_save[i].pid == pid) {
            pid_save_collection.pid_save[i].use = false;
            break;
        }
    }
}
