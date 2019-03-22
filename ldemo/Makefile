FPRINT_CFLAGS = -I/usr/include/libfprint
FPRINT_LIBS = -lfprint

GTK_LIBS = -pthread -I/usr/include/gtk-2.0 -I/usr/lib/x86_64-linux-gnu/gtk-2.0/include -I/usr/include/gio-unix-2.0/ -I/usr/include/cairo -I/usr/include/pango-1.0 -I/usr/include/atk-1.0 -I/usr/include/cairo -I/usr/include/pixman-1 -I/usr/include/gdk-pixbuf-2.0 -I/usr/include/libpng16 -I/usr/include/pango-1.0 -I/usr/include/harfbuzz -I/usr/include/pango-1.0 -I/usr/include/glib-2.0 -I/usr/lib/x86_64-linux-gnu/glib-2.0/include -I/usr/include/freetype2 -I/usr/include/libpng16
GTK_CFLAGS = -lgtk-x11-2.0 -lgdk-x11-2.0 -lpangocairo-1.0 -latk-1.0 -lcairo -lgdk_pixbuf-2.0 -lgio-2.0 -lpangoft2-1.0 -lpango-1.0 -lgobject-2.0 -lglib-2.0 -lfontconfig -lfreetype

SOURCES = main.c enroll.c img.c verify.c identify.c scan.c fprint_ml.h
CFLAGS = -std=gnu99 -fgnu89-inline -Wall -Wundef -Wunused -Wstrict-prototypes -Werror-implicit-function-declaration -Wno-pointer-sign -Wshadow
BIN = fprint_ml

all: fprint_ml

fprint_ml:
	gcc $(SOURCES) $(CFLAGS) $(FPRINT_LIBS) $(FPRINT_CFLAGS) $(GTK_LIBS) $(GTK_CFLAGS) -g -O2 -o $(BIN)
run: all
	./$(BIN)
clean:
	rm -rf *.o fprint_ml