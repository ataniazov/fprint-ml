#include <glib.h>
#include <glib/gstdio.h>
#include <gtk/gtk.h>
#include <libfprint/fprint.h>

int file_fprint_save(struct fp_img *data, gchar *path) {
    GError *err = NULL;
    size_t len;
    int r;
    g_file_set_contents(path, data, sizeof(struct fp_img *), &err);

    if (err) {
        r = err->code;
        g_print("save failed: %s", err->message);
        g_error_free(err);
        return r;
    }

    return 0;
}

int file_fprint_load(struct fp_img **data, gchar *path) {
    struct fp_img *fdata;
    gsize length;
    gchar *contents;
    GError *err = NULL;
    int r;

    g_file_get_contents(path, &fdata, &length, &err);
    if (err) {
        int r = err->code;
        g_print("%s load failed: %s", path, err->message);
        g_error_free(err);
        return r;
    }

    *data = fdata;
    return 0;
}

int file_fprint_delete(gchar *path);