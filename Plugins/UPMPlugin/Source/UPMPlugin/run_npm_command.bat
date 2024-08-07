@echo off
setlocal

:: Define paths
set "NPM_PATH=C:\Program Files\nodejs\npm.cmd"
set "OUTPUT_FILE=%1"
set "ERROR_FILE=%2"
shift
shift

:: Run the command and redirect output
"%NPM_PATH%" %* > "%OUTPUT_FILE%" 2> "%ERROR_FILE%"

:: Capture the exit code of the command
set "EXIT_CODE=%ERRORLEVEL%"

:: End local environment
endlocal

:: Exit with the captured exit code
exit /b %EXIT_CODE%