#ifndef __FILE_H__
#define __FILE_H__

#include <gtk/gtk.h>
#include <libfprint/fprint.h>

int file_fprint_save(struct fp_img *data, gchar *path);
int file_fprint_load(struct fp_img **data, gchar *path);
int file_fprint_delete(gchar *path);;

#endif