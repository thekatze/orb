# orb engine

learning low level programming in c by architecting a game engine (...and game?)

following the kohi engine video series

right handed coordinate system

## building

requirements:
- cmake
- clang (>= 18.0.0)
- ninja
- vulkan

on windows, use [llvm-mingw](https://github.com/mstorsjo/llvm-mingw/releases)

```sh
$ cmake . -B build -G Ninja
$ cmake --build build/
$ ./build/example/example
```

for convenience this project uses just

```sh
$ just run example
```

to build documentation:
- doxygen
- graphviz

```sh
$ doxygen Doxyfile
```

### release build

```sh
$ just release
```

## notes:

check dynamically linked libraries on mac:

```sh
$ otool -L build/example/example
```

on linux

```sh
$ lld build/example/example
```

## architecture
