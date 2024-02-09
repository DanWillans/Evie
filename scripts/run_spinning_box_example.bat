@echo off

setlocal enabledelayedexpansion

set "binaryPath=%CD%\out\install\"

if not exist %binaryPath% (
    echo "!binaryPath!" doesn't exist.
)

rem Iterate through each directory within "binaryPath"
for /D %%D in ("%binaryPath%*") do (
    set "binaryDir=%%D\bin"

    rem Check if the "spinning_box_example.exe" file exists within the "bin" directory
    if exist "!binaryDir!\spinning_box_example.exe" (
        echo Running "spinning_box_example.exe" in !binaryDir!
        start "" "!binaryDir!\spinning_box_example.exe"
    )
)

endlocal