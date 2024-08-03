set windows-shell := ["powershell.exe", "-NoLogo", "-Command"]

build: configure
    cmake --build build/

configure:
    cmake . -B build -G Ninja

[windows]
clean:
    -rm -Recurse build/

[unix]
clean:
    -rm -rf build/

[macos]
run TARGET: build
    MTL_HUD_ENABLED=1 ./build/{{TARGET}}/{{TARGET}}

[windows, linux]
run TARGET: build
    ./build/{{TARGET}}/{{TARGET}}

release: clean
    cmake -DCMAKE_BUILD_TYPE=Release . -B build -G Ninja
    cmake --build build/
    ./build/tests/tests
