mkdir build-windows && cd build-windows    
cmake .. -DCMAKE_TOOLCHAIN_FILE=../mingw64-toolchain.cmake
make

