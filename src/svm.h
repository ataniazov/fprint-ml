#ifndef __SVM_H__
#define __SVM_H__

#include <gtk/gtk.h>
#include <libfprint/fprint.h>

int fprint_csv_format_write(struct fp_img *img, gchar *path);
int fprint_sparse_format_write(struct fp_img *img, gchar *path);
void fprint_print_data(struct fp_minutia *min);

#endif