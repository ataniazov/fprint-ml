SVM_LIBS = -lsvm
SVM_CFLAGS = -I/usr/include/libsvm

SOURCES = main.c
CFLAGS = -std=gnu99 -fgnu89-inline -Wall -Wundef -Wunused -Wstrict-prototypes -Werror-implicit-function-declaration -Wno-pointer-sign -Wshadow
BIN = svm_ml

all: svm_ml

svm_ml:
	gcc $(SOURCES) $(CFLAGS) -o $(BIN)
run: all
	./$(BIN)
clean:
	rm -rf *.o $(BIN)