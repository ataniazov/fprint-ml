# fprint-ml
Machine Learning fingerprint recognition

## build-dep

```
$ sudo apt build-dep fprint-demo
$ dpkg-buildpackage -rfakeroot -uc -b
```

## install

```
$ ./autogen.sh 
$ ./configure --prefix=$HOME/.local/
$ make install
$ fprint_ml 
```
