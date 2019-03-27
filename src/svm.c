#include <glib.h>
#include <glib/gstdio.h>
#include <gtk/gtk.h>
#include <libfprint/fprint.h>

char buf[10000000];
float feature[100000];

int csv_to_sparse(char *csv_file_path) {
    FILE *fp;
    FILE *outfile;

    if ((fp = fopen(csv_file_path, "r")) == NULL) {
        fprintf(stderr, "Can't open input file %s\n", csv_file_path);
        return 1;
    }

    outfile = fopen((char *)g_strconcat(csv_file_path, ".sparse", NULL), "a+");
    if (outfile == NULL) {
        g_print("WARNING: csv_to_sparse() -> fopen() can't write file\n");
        return 1;
    }

    while (fscanf(fp, "%[^\n]\n", buf) == 1) {
        int i = 0, j;
        /*
        char *p=strtok(buf,",");

        feature[i++]=atof(p);

        while((p=strtok(NULL,",")))
                feature[i++]=atof(p);
        */

        char *ptr = buf;
        while (ptr != NULL) {
            char *token = strsep(&ptr, ",");
            if (strlen(token) == 0)
                feature[i++] = 0.0;
            else
                feature[i++] = atof(token);
        }

        //		--i;
        /*
        if ((int) feature[i]==1)
                printf("-1 ");
        else
                printf("+1 ");
        */
        //		printf("%f ", feature[1]);
        fprintf(outfile, "%d", (int)feature[0]);
        for (j = 1; j < i; j++)
            if (feature[j] != 0)
                fprintf(outfile, " %d:%f", j, feature[j]);

        fprintf(outfile, "\n");
    }
    fclose(fp);
    fclose(outfile);

    return 0;
}

gint minutia_csv_data(char **data, int class, int x, int y, int ex, int ey,
                      int direction, int type, int appearing) {
    // *data = (char *)g_strdup_printf("1:%d 2:%d 3:%d 4:%d 5:%d 6:%d 7:%d\n",
    // x, y, ex, ey, direction, type, appearing);
    *data = (char *)g_strdup_printf("%d,%d,%d,%d,%d\n", class, ex, ey,
                                    direction, type);
    return 0;
}

int fprint_sparse_format_write(struct fp_img *img, gchar *path) {
    struct fp_minutia **minlist;
    int nr_minutiae;
    int i;

    FILE *outfile;

    if (!img) {
        g_print("WARNING: fprint_sparse_format_write() -> img is NULL\n");
        return 1;
    }

    outfile =
        fopen((char *)g_strconcat(path, "fprint-ml/minutia_svm", NULL), "a+");
    if (outfile == NULL) {
        g_print("WARNING: fprint_sparse_format_write() -> fopen() can't write "
                "file\n");
        return 1;
    }

    minlist = fp_img_get_minutiae(img, &nr_minutiae);

    for (i = 0; i < nr_minutiae; i++) {
        struct fp_minutia *min = minlist[i];
        char *data;
        int class = 1;
        minutia_csv_data(&data, class, min->x, min->y, min->ex, min->ey,
                         min->direction, min->type, min->appearing);

        if (data == NULL) {
            g_print("WARNING: fp_img_get_minutiae() -> minutia_svm_data() "
                    "returned data is NULL\n");
            return 1;
        }

        fputs(data, outfile);
    }
    fclose(outfile);

    return 0;
}

int fprint_csv_format_write(struct fp_img *img, gchar *path) {
    struct fp_minutia **minlist;
    int nr_minutiae;
    int i;

    FILE *outfile;

    if (!img) {
        g_print("WARNING: fprint_csv_format_write() -> img is NULL\n");
        return 1;
    }

    outfile =
        fopen((char *)g_strconcat(path, "fprint-ml/minutia_svm", NULL), "a+");
    if (outfile == NULL) {
        g_print(
            "WARNING: fprint_svm_format_write() -> fopen() can't write file\n");
        return 1;
    }

    minlist = fp_img_get_minutiae(img, &nr_minutiae);

    for (i = 0; i < nr_minutiae; i++) {
        struct fp_minutia *min = minlist[i];
        char *data;
        int class = 1;
        minutia_csv_data(&data, class, min->x, min->y, min->ex, min->ey,
                         min->direction, min->type, min->appearing);

        if (data == NULL) {
            g_print("WARNING: fp_img_get_minutiae() -> minutia_svm_data() "
                    "returned data is NULL\n");
            return 1;
        }

        fputs(data, outfile);
    }
    fclose(outfile);

    csv_to_sparse((char *)g_strconcat(path, "fprint-ml/minutia_svm", NULL));

    return 0;
}

void fprint_print_minutia(struct fp_minutia *min) {
    g_print("x:%d, y:%d, ex:%d, ey:%d, dir:%d, rlbt:%f, type:%d, apr:%d, "
            "ftr_id:%d, nbrs:%d, rdg_cnt:%d, n_nbrs:%d\n",
            min->x, min->y, min->ex, min->ey, min->direction, min->reliability,
            min->type, min->appearing, min->feature_id, *(min->nbrs),
            *(min->ridge_counts), min->num_nbrs);
}
