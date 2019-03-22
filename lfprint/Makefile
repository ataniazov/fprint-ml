#verify_ml.c verify_live.c enroll.c verify.c img_capture.c cpp-test.cpp img_capture_continuous.c

FPRINT_LIBS = -lfprint
FPRINT_CFLAGS = -I/usr/include/libfprint

SVM_LIBS = -lsvm
SVM_CFLAGS = -I/usr/include/libsvm

CFLAGS = -std=gnu99 -fgnu89-inline -Wall -Wundef -Wunused -Wstrict-prototypes -Werror-implicit-function-declaration -Wno-pointer-sign -Wshadow
BIN = verify_live enroll verify img_capture cpp-test verify_ml enroll_ml

all: verify_ml verify_live enroll verify img_capture cpp-test enroll_ml#img_capture_continuous

verify_ml:
	gcc verify_ml.c -o verify_ml $(CFLAGS) $(FPRINT_LIBS) $(FPRINT_CFLAGS)
enroll_ml:
	gcc enroll_ml.c -o enroll_ml $(CFLAGS) $(FPRINT_LIBS) $(FPRINT_CFLAGS)
verify_live:
	gcc verify_live.c -o verify_live $(CFLAGS) $(FPRINT_LIBS) $(FPRINT_CFLAGS)
enroll:
	gcc enroll.c -o enroll $(CFLAGS) $(FPRINT_LIBS) $(FPRINT_CFLAGS)
verify:
	gcc verify.c -o verify $(CFLAGS) $(FPRINT_LIBS) $(FPRINT_CFLAGS)
img_capture:
	gcc img_capture.c -o img_capture $(CFLAGS) $(FPRINT_LIBS) $(FPRINT_CFLAGS)
cpp-test:
	gcc cpp-test.cpp -o cpp-test $(CFLAGS) $(FPRINT_LIBS) $(FPRINT_CFLAGS)

ICC_CFLAGS = $(X_CFLAGS) $(XV_CFLAGS)
ICC_LIBS = $(X_LIBS) $(X_PRE_LIBS) $(XV_LIBS) -lX11 $(X_EXTRA_LIBS)
img_capture_continuous:
	gcc img_capture_continuous.c -o img_capture_continuous $(CFLAGS) $(FPRINT_LIBS) $(FPRINT_CFLAGS) 
clean:
	rm -rf *.o *.pgm $(BIN)