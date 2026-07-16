@echo off
setlocal

set EXE=main.exe
set RESULT_FILE=results.txt

gcc -Wall -Wextra -O2 -o %EXE% main.c

if errorlevel 1 (
    echo Compile failed.
    pause
    exit /b 1
)

> "%RESULT_FILE%" echo Results

call :run DataFiles\data8.txt
call :run DataFiles\data14-1.txt
call :run DataFiles\data14-2.txt
call :run DataFiles\data17-1.txt
call :run DataFiles\data17-2.txt
call :run DataFiles\data50-1.txt
call :run DataFiles\data50-2.txt
call :run DataFiles\data50-3.txt
call :run DataFiles\data50-4.txt
call :run DataFiles\data100-1.txt
call :run DataFiles\data100-2.txt
call :run DataFiles\data100-3.txt
call :run DataFiles\data100-4.txt
call :run DataFiles\data162-1.txt
call :run DataFiles\data162-2.txt
call :run DataFiles\data162-3.txt
call :run DataFiles\data162-4.txt

type "%RESULT_FILE%"

echo ========================================
echo Finished.
pause
exit /b

:run
echo Running: %1
>> "%RESULT_FILE%" echo ========================================
>> "%RESULT_FILE%" echo Running: %1
echo %1 | %EXE% >> "%RESULT_FILE%"
exit /b