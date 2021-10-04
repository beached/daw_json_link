cd build

REM ##############################
REM Setting VCVars
@call "C:\Program Files (x86)\Microsoft Visual Studio\%1\Enterprise\VC\Auxiliary\Build\vcvarsall.bat" amd64

REM ##############################
REM Building
cmake --build . --target full --config %2 -j 2 
