[unix]
build: configure
    cmake --build build/

[unix]
configure:
    cmake . -B build -G Ninja

[macos]
run TARGET: build
    MTL_HUD_ENABLED=1 ./build/{{TARGET}}/{{TARGET}}

[linux]
run TARGET: build
    ./build/{{TARGET}}/{{TARGET}}

[unix]
release:
    -rm -rf build/
    cmake -DCMAKE_BUILD_TYPE=Release . -B build
    cmake --build build/
    ./build/tests/tests

[unix]
clean:
    -rm -rf build/

