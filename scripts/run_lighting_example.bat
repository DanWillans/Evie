@echo off

setlocal enabledelayedexpansion

set "binaryPath=%CD%\out\install\"

if not exist %binaryPath% (
    echo "!binaryPath!" doesn't exist.
)

rem Iterate through each directory within "binaryPath"
for /D %%D in ("%binaryPath%*") do (
    set "binaryDir=%%D\bin"

    rem Check if the "lighting_example.exe" file exists within the "bin" directory
    if exist "!binaryDir!\lighting_example.exe" (
        echo Running "lighting_example.exe" in !binaryDir!
        start "" "!binaryDir!\lighting_example.exe"
    )
)

endlocal