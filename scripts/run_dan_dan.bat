@echo off

setlocal enabledelayedexpansion

set "binaryPath=%CD%\out\install\"

if not exist %binaryPath% (
    echo "!binaryPath!" doesn't exist.
)

rem Iterate through each directory within "binaryPath"
for /D %%D in ("%binaryPath%*") do (
    set "binaryDir=%%D\bin"

    rem Check if the "dan_dan_the_game.exe" file exists within the "bin" directory
    if exist "!binaryDir!\dan_dan_the_game.exe" (
        echo Running "dan_dan_the_game.exe" in !binaryDir!
        start "" "!binaryDir!\dan_dan_the_game.exe"
    )
)

endlocal