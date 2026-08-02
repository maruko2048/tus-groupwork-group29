@echo off
setlocal EnableExtensions EnableDelayedExpansion

set "BINVEC_EXE=binvec.exe"
set "MAIN_EXE=main.exe"
set "RESULT_FILE=results.txt"

gcc -Wall -Wextra -O2 -o binvec.exe binvec.c
if errorlevel 1 (
    echo Compile failed: binvec.c
    exit /b 1
)

gcc -Wall -Wextra -O2 -o main.exe main.c
if errorlevel 1 (
    echo Compile failed: main.c
    exit /b 1
)

for /L %%I in (1,1,5) do (
    set "TIME_VALUE="
    for /F "tokens=3" %%T in ('binvec.exe ^| findstr /C:"time ="') do set "TIME_VALUE=%%T"

    if not defined TIME_VALUE (
        echo Failed to read time from binvec run %%I. 1>&2
        exit /b 1
    )

    set "TIME_%%I=!TIME_VALUE!"
)

set "T="
set "THREE_T="
for /F %%T in ('powershell.exe -NoProfile -Command "$c=[Globalization.CultureInfo]::InvariantCulture; $s=[double]::Parse('!TIME_1!',$c)+[double]::Parse('!TIME_2!',$c)+[double]::Parse('!TIME_3!',$c)+[double]::Parse('!TIME_4!',$c)+[double]::Parse('!TIME_5!',$c); $s.ToString('F6',$c)"') do set "T=%%T"
for /F %%T in ('powershell.exe -NoProfile -Command "$c=[Globalization.CultureInfo]::InvariantCulture; (3*[double]::Parse('!T!',$c)).ToString('F6',$c)"') do set "THREE_T=%%T"

if not defined T (
    echo Failed to calculate T. 1>&2
    exit /b 1
)
if not defined THREE_T (
    echo Failed to calculate 3*T. 1>&2
    exit /b 1
)

> "%RESULT_FILE%" echo Results
echo Results
call :write "Binvec times:"
for /L %%I in (1,1,5) do call :write "Run %%I: !TIME_%%I!"
call :write "T: !T!"

call :run "DataFiles\data8.txt"
call :run "DataFiles\data14-1.txt"
call :run "DataFiles\data14-2.txt"
call :run "DataFiles\data17-1.txt"
call :run "DataFiles\data17-2.txt"
call :run "DataFiles\data50-1.txt"
call :run "DataFiles\data50-2.txt"
call :run "DataFiles\data50-3.txt"
call :run "DataFiles\data50-4.txt"
call :run "DataFiles\data100-1.txt"
call :run "DataFiles\data100-2.txt"
call :run "DataFiles\data100-3.txt"
call :run "DataFiles\data100-4.txt"
call :run "DataFiles\data162-1.txt"
call :run "DataFiles\data162-2.txt"
call :run "DataFiles\data162-3.txt"
call :run "DataFiles\data162-4.txt"

echo ========================================
echo Finished.
exit /b 0

:write
echo %~1
>> "%RESULT_FILE%" echo %~1
exit /b 0

:run
set "DATA_FILE=%~1"
set "TIME_LIMIT=%T%"
if /I "!DATA_FILE:~0,18!"=="DataFiles\data162-" set "TIME_LIMIT=%THREE_T%"

echo.
>> "%RESULT_FILE%" echo.
call :write "========================================"
call :write "Running: !DATA_FILE!"

powershell.exe -NoProfile -Command "$output=$env:DATA_FILE | & '.\main.exe' $env:TIME_LIMIT; if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }; $output -replace '^input filename: ', '' | ForEach-Object { Write-Output $_; [IO.File]::AppendAllText($env:RESULT_FILE, $_ + [Environment]::NewLine, [Text.Encoding]::ASCII) }"
if errorlevel 1 exit /b 1
exit /b 0
