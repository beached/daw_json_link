REM ##############################
REM Installing Ninja
REM choco install ninja
vcpkg install ninja
md build
cd build

REM ##############################
REM Setting VCVars
@call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Enterprise\VC\Auxiliary\Build\vcvarsall.bat" amd64
REM ##############################
REM Running cmake
cmake -DCMAKE_BUILD_TYPE=Debug -GNinja -DCMAKE_CXX_COMPILER=cl.exe -DCMAKE_C_COMPILER=cl.exe  ..
REM ##############################
REM Building
cmake --build . --config Debug --target full -j 2 -- -k 1000
