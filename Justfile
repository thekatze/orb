set windows-shell := ["powershell.exe", "-NoLogo", "-Command"]

build TARGET="example": configure
    cmake --build build/ --target {{TARGET}}

configure:
    cmake . -B build -G Ninja

[windows]
clean:
    -rm -Recurse build/

[unix]
clean:
    -rm -rf build/

[macos]
run TARGET="example": (build TARGET)
    MTL_HUD_ENABLED=1 ./build/{{TARGET}}/{{TARGET}}

[windows, linux]
run TARGET="example": (build TARGET)
    ./build/{{TARGET}}/{{TARGET}}

release: clean
    cmake -DCMAKE_BUILD_TYPE=Release . -B build -G Ninja
    cmake --build build/
    ./build/tests/tests
