/*
 *
 */

#include <gdk-pixbuf/gdk-pixbuf.h>
#include <gtk/gtk.h>
#include <libfprint/fprint.h>

#include "fprint_ml.h"

static GtkWidget *cwin_non_img_label;
static GtkWidget *cwin_scanned_img;
static GtkWidget *cwin_clt_scale;
static GtkWidget *cwin_clt_button;
static GtkWidget *cwin_clt_status;
static GtkWidget *cwin_minutiae_cnt;
static GtkWidget *cwin_ctrl_frame;
static GtkWidget *cwin_radio_normal;
static GtkWidget *cwin_radio_bin;
static GtkWidget *cwin_show_minutiae;
static GtkWidget *cwin_img_save_btn;

static struct fp_img *img_normal = NULL;
static struct fp_img *img_bin = NULL;
static struct fp_print_data *scan_data = NULL;

// static void cwin_clt_status_no_print(void) {
//     gtk_label_set_markup(GTK_LABEL(cwin_clt_status),
//                          "<b>Status:</b> No prints detected for this
//                          device.");
//     // gtk_widget_set_sensitive(cwin_vfy_button, FALSE);
// }

static void cwin_clear(void) {
    fp_img_free(img_normal);
    img_normal = NULL;
    fp_img_free(img_bin);
    img_bin = NULL;

    fp_print_data_free(scan_data);
    scan_data = NULL;

    gtk_image_clear(GTK_IMAGE(cwin_scanned_img));
    gtk_widget_set_sensitive(cwin_img_save_btn, FALSE);

    gtk_label_set_text(GTK_LABEL(cwin_clt_status), NULL);
    gtk_label_set_text(GTK_LABEL(cwin_minutiae_cnt), NULL);
    // gtk_widget_set_sensitive(cwin_clt_button, FALSE);
}

static void cwin_refresh(void) {}

static void cwin_activate_dev(void) {
    g_assert(fpdev);

    if (fp_dev_supports_imaging(fpdev)) {
        int width = fp_dev_get_img_width(fpdev);
        int height = fp_dev_get_img_height(fpdev);
        gtk_widget_set_size_request(cwin_scanned_img,
                                    (width == 0) ? 192 : width,
                                    (height == 0) ? 192 : height);
        gtk_widget_hide(cwin_non_img_label);
        gtk_widget_show(cwin_scanned_img);
        gtk_widget_set_sensitive(cwin_ctrl_frame, TRUE);
    } else {
        gtk_widget_show(cwin_non_img_label);
        gtk_widget_hide(cwin_scanned_img);
        gtk_widget_set_sensitive(cwin_ctrl_frame, FALSE);
    }
}

// static void cwin_clt_status_print_loaded(int status)
// {
// 	if (status == 0) {
// 		gtk_label_set_markup(GTK_LABEL(cwin_clt_status),
// 			"<b>Status:</b> Ready for verify scan.");
// 		//gtk_widget_set_sensitive(cwin_vfy_button, TRUE);
// 	} else {
// 		gchar *msg = g_strdup_printf("<b>Status:</b> Error %d, print
// corrupt?", 			status);
// gtk_label_set_markup(GTK_LABEL(cwin_clt_status), msg);
// 		//gtk_widget_set_sensitive(cwin_vfy_button, FALSE);
// 		g_free(msg);
// 	}
// }

static void cwin_clt_status_scan_result(int code) {
    const char *msgs[] = {
        [FP_CAPTURE_COMPLETE] = "Finger capture complete",
        [FP_CAPTURE_FAIL] = "Finger capture fail",
        // [FP_VERIFY_NO_MATCH] = "Finger does not match.",
        // [FP_VERIFY_MATCH] = "Finger matches!",
        // [FP_VERIFY_RETRY] = "Bad scan.",
        // [FP_VERIFY_RETRY_TOO_SHORT] = "Swipe was too short.",
        // [FP_VERIFY_RETRY_CENTER_FINGER] = "Finger was not centered on
        // sensor.", [FP_VERIFY_RETRY_REMOVE_FINGER] = "Bad scan, remove
        // finger.",
    };
    gchar *msg;

    if (code < 0) {
        msg = g_strdup_printf("<b>Status:</b> Scan failed, error %d", code);
        gtk_label_set_text(GTK_LABEL(cwin_minutiae_cnt), NULL);
    } else {
        msg = g_strdup_printf("<b>Status:</b> %s", msgs[code]);
    }

    gtk_label_set_markup(GTK_LABEL(cwin_clt_status), msg);
    g_free(msg);
}

static void plot_minutiae(unsigned char *rgbdata, int width, int height,
                          struct fp_minutia **minlist, int nr_minutiae) {
    int i;
#define write_pixel(num)                                                       \
    do {                                                                       \
        rgbdata[((num)*3)] = 0xff;                                             \
        rgbdata[((num)*3) + 1] = 0;                                            \
        rgbdata[((num)*3) + 2] = 0;                                            \
    } while (0)

    for (i = 0; i < nr_minutiae; i++) {
        struct fp_minutia *min = minlist[i];
        size_t pixel_offset = (min->y * width) + min->x;
        write_pixel(pixel_offset - 2);
        write_pixel(pixel_offset - 1);
        write_pixel(pixel_offset);
        write_pixel(pixel_offset + 1);
        write_pixel(pixel_offset + 2);

        write_pixel(pixel_offset - (width * 2));
        write_pixel(pixel_offset - (width * 1) - 1);
        write_pixel(pixel_offset - (width * 1));
        write_pixel(pixel_offset - (width * 1) + 1);
        write_pixel(pixel_offset + (width * 1) - 1);
        write_pixel(pixel_offset + (width * 1));
        write_pixel(pixel_offset + (width * 1) + 1);
        write_pixel(pixel_offset + (width * 2));
    }
}

static void cwin_img_draw(void) {
    struct fp_minutia **minlist;
    unsigned char *rgbdata;
    GdkPixbuf *pixbuf;
    gchar *tmp;
    int nr_minutiae;
    int width;
    int height;

    if (!img_normal || !img_bin)
        return;

    minlist = fp_img_get_minutiae(img_normal, &nr_minutiae);

    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(cwin_radio_normal)))
        rgbdata = img_to_rgbdata(img_normal);
    else
        rgbdata = img_to_rgbdata(img_bin);

    width = fp_img_get_width(img_normal);
    height = fp_img_get_height(img_normal);
    gtk_widget_set_size_request(cwin_scanned_img, width, height);

    tmp = g_strdup_printf("Detected %d minutiae.", nr_minutiae);
    gtk_label_set_text(GTK_LABEL(cwin_minutiae_cnt), tmp);
    g_free(tmp);

    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(cwin_show_minutiae)))
        plot_minutiae(rgbdata, width, height, minlist, nr_minutiae);

    pixbuf =
        gdk_pixbuf_new_from_data(rgbdata, GDK_COLORSPACE_RGB, FALSE, 8, width,
                                 height, width * 3, pixbuf_destroy, NULL);
    gtk_image_set_from_pixbuf(GTK_IMAGE(cwin_scanned_img), pixbuf);
    g_object_unref(pixbuf);

    gtk_widget_set_sensitive(cwin_img_save_btn, TRUE);
}

static void cwin_cb_imgfmt_toggled(GtkWidget *widget, gpointer data) {
    cwin_img_draw();
}

static void scan_stopped_cb(struct fp_dev *dev, void *user_data) {
    gtk_widget_destroy(GTK_WIDGET(user_data));
}

static void scan_cb(struct fp_dev *dev, int result, struct fp_img *img,
                    void *user_data) {
    GtkWidget *dialog;
    int r;

    destroy_scan_finger_dialog(GTK_WIDGET(user_data));
    cwin_clt_status_scan_result(result);

    fp_img_free(img_normal);
    img_normal = NULL;
    fp_img_free(img_bin);
    img_bin = NULL;

    if (img) {
        img_normal = img;
        img_bin = fp_img_binarize(img);
        cwin_img_draw();
    }

    dialog = run_please_wait_dialog("Scan ending...");
    r = fp_async_capture_stop(dev, scan_stopped_cb, dialog);
    if (r < 0)
        gtk_widget_destroy(dialog);
}

static void scan_finger_response(GtkWidget *dialog, gint arg,
                                 gpointer user_data) {
    int r;

    destroy_scan_finger_dialog(dialog);
    dialog = run_please_wait_dialog("Scan ending...");
    r = fp_async_capture_stop(fpdev, scan_stopped_cb, dialog);
    if (r < 0)
        gtk_widget_destroy(dialog);
}

static void cwin_cb_scan(GtkWidget *widget, gpointer user_data) {
    GtkWidget *dialog;
    int r;
    gchar *label = (gchar *)gtk_button_get_label(GTK_BUTTON(widget));

    if (g_strcmp0(label, "Start") == 0) {
        gtk_button_set_label(GTK_BUTTON(widget), "Stop");
        gtk_widget_set_sensitive(cwin_clt_scale, FALSE);
        gtk_widget_set_sensitive(cwin_img_save_btn, FALSE);

        dialog = create_scan_finger_dialog();
        // r = fp_async_verify_start(fpdev, enroll_data, verify_cb, dialog);
        r = fp_async_capture_start(fpdev, 0, scan_cb, dialog);
        if (r < 0) {
            destroy_scan_finger_dialog(dialog);
            dialog = gtk_message_dialog_new_with_markup(
                GTK_WINDOW(mwin_window),
                GTK_DIALOG_DESTROY_WITH_PARENT | GTK_DIALOG_MODAL,
                GTK_MESSAGE_ERROR, GTK_BUTTONS_OK,
                "Could not start collection, error %d", r);

            gtk_dialog_run(GTK_DIALOG(dialog));
            gtk_widget_destroy(dialog);
            return;
        }

        g_signal_connect(dialog, "response", G_CALLBACK(scan_finger_response),
                         NULL);
        run_scan_finger_dialog(dialog);
    } else {
        gtk_button_set_label(GTK_BUTTON(widget), "Start");
        gtk_widget_set_sensitive(cwin_clt_scale, TRUE);
        gtk_widget_set_sensitive(cwin_img_save_btn, TRUE);
    }
}

static void cwin_cb_img_save(GtkWidget *widget, gpointer user_data) {
    GdkPixbuf *pixbuf = gtk_image_get_pixbuf(GTK_IMAGE(cwin_scanned_img));
    GtkWidget *dialog;
    gchar *filename;
    GError *error = NULL;

    g_assert(pixbuf);

    dialog = gtk_file_chooser_dialog_new(
        "Save Image", GTK_WINDOW(mwin_window), GTK_FILE_CHOOSER_ACTION_SAVE,
        GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, GTK_STOCK_SAVE,
        GTK_RESPONSE_ACCEPT, NULL);

    gtk_file_chooser_set_do_overwrite_confirmation(GTK_FILE_CHOOSER(dialog),
                                                   TRUE);
    gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER(dialog),
                                      "fingerprint.png");

    if (gtk_dialog_run(GTK_DIALOG(dialog)) != GTK_RESPONSE_ACCEPT) {
        gtk_widget_destroy(dialog);
        return;
    }

    filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
    gtk_widget_destroy(dialog);
    if (!gdk_pixbuf_save(pixbuf, filename, "png", &error, NULL)) {
        dialog = gtk_message_dialog_new(
            GTK_WINDOW(mwin_window), GTK_DIALOG_DESTROY_WITH_PARENT,
            GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE, "%s", error->message);
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        g_error_free(error);
    }
    g_free(filename);
}

static GtkWidget *cwin_create(void) {
    // GtkCellRenderer *renderer;
    GtkWidget *ui_vbox;
    GtkWidget *label, *clt_vbox, *clt_frame, *scan_frame, *img_vbox;
    GtkWidget *cwin_ctrl_vbox;
    GtkWidget *cwin_main_hbox;

    cwin_main_hbox = gtk_hbox_new(FALSE, 1);

    /* Image frame */
    scan_frame = gtk_frame_new("Scanned Image");
    gtk_box_pack_start(GTK_BOX(cwin_main_hbox), scan_frame, TRUE, TRUE, 0);

    /* Image vbox */
    img_vbox = gtk_vbox_new(FALSE, 1);
    gtk_container_add(GTK_CONTAINER(scan_frame), img_vbox);

    /* Image */
    cwin_scanned_img = gtk_image_new();
    gtk_box_pack_start(GTK_BOX(img_vbox), cwin_scanned_img, TRUE, FALSE, 0);

    /* Non-imaging device */
    cwin_non_img_label =
        gtk_label_new("This device does not have imaging "
                      "capabilities, no images will be displayed.");
    gtk_box_pack_start_defaults(GTK_BOX(img_vbox), cwin_non_img_label);

    /* vbox for collection status and image control frames */
    ui_vbox = gtk_vbox_new(FALSE, 1);
    gtk_box_pack_end(GTK_BOX(cwin_main_hbox), ui_vbox, FALSE, FALSE, 0);

    /* Collection status */
    clt_frame = gtk_frame_new("Collection");
    gtk_box_pack_start_defaults(GTK_BOX(ui_vbox), clt_frame);

    clt_vbox = gtk_vbox_new(FALSE, 1);
    gtk_container_add(GTK_CONTAINER(clt_frame), clt_vbox);

    /*  */
    label = gtk_label_new("Enter number of samples:");
    gtk_box_pack_start(GTK_BOX(clt_vbox), label, FALSE, FALSE, 0);

    cwin_clt_scale = gtk_hscale_new_with_range(0, 500, 1);
    gtk_box_pack_start(GTK_BOX(clt_vbox), cwin_clt_scale, FALSE, FALSE, 0);

    /* Collection button */
    cwin_clt_button = gtk_button_new_with_label("Start");
    g_signal_connect(G_OBJECT(cwin_clt_button), "clicked",
                     G_CALLBACK(cwin_cb_scan), NULL);
    gtk_box_pack_start(GTK_BOX(clt_vbox), cwin_clt_button, FALSE, FALSE, 0);

    /* Collection status */
    cwin_clt_status = gtk_label_new(NULL);
    gtk_box_pack_start(GTK_BOX(clt_vbox), cwin_clt_status, FALSE, FALSE, 0);

    /* Minutiae count */
    cwin_minutiae_cnt = gtk_label_new(NULL);
    gtk_box_pack_start(GTK_BOX(clt_vbox), cwin_minutiae_cnt, FALSE, FALSE, 0);

    /* Image controls frame */
    cwin_ctrl_frame = gtk_frame_new("Image control");
    gtk_box_pack_end_defaults(GTK_BOX(ui_vbox), cwin_ctrl_frame);

    cwin_ctrl_vbox = gtk_vbox_new(FALSE, 1);
    gtk_container_add(GTK_CONTAINER(cwin_ctrl_frame), cwin_ctrl_vbox);

    /* Image format radio buttons */
    cwin_radio_normal = gtk_radio_button_new_with_label(NULL, "Normal");
    g_signal_connect(G_OBJECT(cwin_radio_normal), "toggled",
                     G_CALLBACK(cwin_cb_imgfmt_toggled), NULL);
    gtk_box_pack_start(GTK_BOX(cwin_ctrl_vbox), cwin_radio_normal, FALSE, FALSE,
                       0);

    cwin_radio_bin = gtk_radio_button_new_with_label_from_widget(
        GTK_RADIO_BUTTON(cwin_radio_normal), "Binarized");
    gtk_box_pack_start(GTK_BOX(cwin_ctrl_vbox), cwin_radio_bin, FALSE, FALSE,
                       0);

    /* Minutiae plotting */
    cwin_show_minutiae = gtk_check_button_new_with_label("Show minutiae");
    g_signal_connect(GTK_OBJECT(cwin_show_minutiae), "toggled",
                     G_CALLBACK(cwin_cb_imgfmt_toggled), NULL);
    gtk_box_pack_start(GTK_BOX(cwin_ctrl_vbox), cwin_show_minutiae, FALSE,
                       FALSE, 0);

    /* Save image */
    cwin_img_save_btn = gtk_button_new_from_stock(GTK_STOCK_SAVE);
    g_signal_connect(G_OBJECT(cwin_img_save_btn), "clicked",
                     G_CALLBACK(cwin_cb_img_save), NULL);
    gtk_box_pack_end(GTK_BOX(cwin_ctrl_vbox), cwin_img_save_btn, FALSE, FALSE,
                     0);

    return cwin_main_hbox;
}

struct fpd_tab collect_tab = {
    .name = "Collect",
    .create = cwin_create,
    .activate_dev = cwin_activate_dev,
    .clear = cwin_clear,
    .refresh = cwin_refresh,
};