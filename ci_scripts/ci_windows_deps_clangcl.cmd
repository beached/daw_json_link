echo "Installing LLVM"
REM choco install llvm

echo "Making build directory"
md build
cd build

REM ##############################
REM Setting VCVars
@call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Enterprise\VC\Auxiliary\Build\vcvarsall.bat" amd64
REM SET PATH=%PATH%;C:\Program Files\LLVM\bin\

REM ##############################
REM Running cmake
cmake -DCMAKE_BUILD_TYPE=%1 -GNinja -DCMAKE_CXX_COMPILER="C:/Program Files (x86)/Microsoft Visual Studio/2019/Enterprise/VC/Tools/Llvm/x64/bin/clang-cl.exe" -DCMAKE_C_COMPILER="C:/Program Files (x86)/Microsoft Visual Studio/2019/Enterprise/VC/Tools/Llvm/x64/bin/clang-cl.exe"..

