@ECHO OFF

cd build

ECHO "##############################"
ECHO "Setting VCVars"
@call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Enterprise\VC\Auxiliary\Build\vcvarsall.bat" amd64

echo "#################################"
echo "Starting Install Tests"
cmake --install .
cd ../cmake/test_project/
md build
cd build
"C:/PROGRAM FILES (X86)/MICROSOFT VISUAL STUDIO/2019/ENTERPRISE/COMMON7/IDE/COMMONEXTENSIONS/MICROSOFT/CMAKE/CMake/bin/cmake.exe" -DCMAKE_BUILD_TYPE=%1 -GNinja -DCMAKE_CXX_COMPILER="C:/Program Files (x86)/Microsoft Visual Studio/2019/Enterprise/VC/Tools/Llvm/x64/bin/clang-cl.exe" -DCMAKE_C_COMPILER="C:/Program Files (x86)/Microsoft Visual Studio/2019/Enterprise/VC/Tools/Llvm/x64/bin/clang-cl.exe" -DCMAKE_PREFIX_PATH="c:/testing" ..
IF %ERRORLEVEL% NEQ 0 EXIT %ERRORLEVEL%
"C:/PROGRAM FILES (X86)/MICROSOFT VISUAL STUDIO/2019/ENTERPRISE/COMMON7/IDE/COMMONEXTENSIONS/MICROSOFT/CMAKE/CMake/bin/cmake.exe" --build . --config %1 
IF %ERRORLEVEL% NEQ 0 EXIT %ERRORLEVEL%
"C:/PROGRAM FILES (X86)/MICROSOFT VISUAL STUDIO/2019/ENTERPRISE/COMMON7/IDE/COMMONEXTENSIONS/MICROSOFT/CMAKE/CMake/bin/ctest.exe" -C %1 -VV 
IF %ERRORLEVEL% NEQ 0 EXIT %ERRORLEVEL%
