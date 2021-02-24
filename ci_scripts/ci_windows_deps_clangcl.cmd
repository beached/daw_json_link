echo "Making build directory"
md build
cd build

REM ##############################
REM Setting VCVars
@call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Enterprise\VC\Auxiliary\Build\vcvarsall.bat" amd64

REM ##############################
REM Running cmake
"C:/Program Files (x86)/Microsoft Visual Studio/2019/Enterprise/COMMON7/COMMONEXTENSIONS/MICROSOFT/CMAKE/CMake/bin/cmake.exe" -DCMAKE_BUILD_TYPE=%1 -GNinja -DDAW_ENABLE_TESTING=On -DCMAKE_CXX_COMPILER="C:/Program Files (x86)/Microsoft Visual Studio/2019/Enterprise/VC/Tools/Llvm/x64/bin/clang-cl.exe" -DCMAKE_C_COMPILER="C:/Program Files (x86)/Microsoft Visual Studio/2019/Enterprise/VC/Tools/Llvm/x64/bin/clang-cl.exe" ..


