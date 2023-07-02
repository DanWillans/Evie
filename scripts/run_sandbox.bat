@echo off

setlocal enabledelayedexpansion

set "binaryPath=%CD%\out\install\"

rem Iterate through each directory within "binaryPath"
for /D %%D in ("%binaryPath%*") do (
    set "binaryDir=%%D\bin"

    rem Check if the "sandbox.exe" file exists within the "bin" directory
    if exist "!binaryDir!\sandbox.exe" (
        echo Running "sandbox.exe" in !binaryDir!
        start "" "!binaryDir!\sandbox.exe"
    )
)

endlocal