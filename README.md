# orb engine

learning low level programming in c by architecting a game engine (...and game?)

following the kohi engine video series

## building

requirements:
- cmake
- a c compiler supporting c23
- vulkan

```sh
$ cmake . -B build
$ cmake --build build/
$ ./build/orb
```

on unix-like systems using the ninja generator is strongly encouraged:

```sh
$ cmake . -B build -G Ninja
```

to build documentation:
- doxygen
- graphviz

```sh
$ doxygen Doxyfile
```

### release build

```sh
$ cmake -DCMAKE_BUILD_TYPE=Release . -B build
$ cmake --build build/
$ ./build/orb
```

## architecture


