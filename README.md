# ChrysaLisp

C++ ChrysaLisp !

A version of ChrysaLisp Lisp written in C++.

Missing a few built in functions from ChrysaLisp yet. However it can now use
the same compiler enviroment from ChrysaLisp OS to compile and build a full OS
boot image from identical source !

If you want to give it a try on your ChrysaLisp OS files, make the exe as per
below and copy over into your ChrysaLisp OS folder and run there with:

```
./chrysalisp -b class/lisp/boot.inc cmd/asm.inc
```

You can then try out the Lisp make commands such as (make), (make-all),
(make-test) etc.

Make with:

```
make -j
```

Clean with:

```
make clean
```

Run with:

```
./chrysalisp
```
