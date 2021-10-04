@ECHO OFF

md build
cd build

SET CMAKEBIN="C:\Program Files (x86)\Microsoft Visual Studio\%1\Enterprise\VC\Auxiliary\Build\vcvarsall.bat" 

ECHO "##############################"
ECHO "Setting VCVars"
@call %CMAKEBIN% amd64

ECHO "##############################"
ECHO "Running cmake"
%CMAKEBIN% -DCMAKE_BUILD_TYPE=%2 -GNinja -DCMAKE_CXX_COMPILER="C:/Program Files (x86)/Microsoft Visual Studio/%1/Enterprise/VC/Tools/Llvm/x64/bin/clang-cl.exe" -DCMAKE_C_COMPILER="C:/Program Files (x86)/Microsoft Visual Studio/%1/Enterprise/VC/Tools/Llvm/x64/bin/clang-cl.exe" -DDAW_ENABLE_TESTING=On -DDAW_WERROR=ON -DCMAKE_CXX_STANDARD=%3 ..

ECHO "##############################"
ECHO "Building"
%CMAKEBIN% --build . --target full -j 2 -- -k 1000
