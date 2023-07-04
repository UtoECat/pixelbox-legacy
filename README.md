# pixelbox
\[link blocked\]

# dependencies
many deps sources are already present in pixelbox :
- `sqlite3` - sql database to store world and configs efficiently
- `sc_sock` - small crossplatform sockets library
- `bearssl` - SSL and Cryptography library for client-server communication security
- `raygui.h` - my favourite gui headeronly library with some custom patches

but some of them not (by obvious reasons) :
- `raylib` - game development library written in pure c. Present in many platforms and package managers, can be compiled easily.
- `libm` - Standart C math library
- `libc` - Standart C library
- `pthreads` may be required on POSIX systems for multithreading
