@echo off
set BOARD=%1
set PORT=%2
set CODETOUPLOAD=%3
IF "%BOARD%"=="mega" (
tools\avrdude -v -patmega2560 -cwiring -P%PORT% -b115200 -D -Uflash:w:binaries\%CODETOUPLOAD%:i
)
IF "%BOARD%"=="due" (
tools\bossac -i -d --port=%PORT% -U false -e -w -v -b binaries\%CODETOUPLOAD% -R 
) ELSE (
@echo on
echo "Board not in config!"
)