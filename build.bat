@echo off
TITLE Athena Builder

:::      ___   __  __                        ________               __      
:::     /   | / /_/ /_  ___  ____  ____ _   / ____/ /_  ___  ____ _/ /______
:::    / /| |/ __/ __ \/ _ \/ __ \/ __ `/  / /   / __ \/ _ \/ __ `/ __/ ___/
:::   / ___ / /_/ / / /  __/ / / / /_/ /  / /___/ / / /  __/ /_/ / /_(__  ) 
:::  /_/  |_\__/_/ /_/\___/_/ /_/\__,_/   \____/_/ /_/\___/\__,_/\__/____/  
:::                                                                         
for /f "delims=: tokens=*" %%A in ('findstr /b ::: "%~f0"') do @echo(%%A

echo [+] Press Any Key to start compilation

echo Set WshShell = CreateObject("WScript.Shell") > temp.vbs
echo WScript.StdIn.ReadLine >> temp.vbs
cscript //nologo temp.vbs
del temp.vbs


"C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe" "Athena/AthenaEFI/AthenaEFI.sln" /t:Clean,Build /property:Configuration=Release /property:Platform=x64
"C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe" "Athena/AthenaUM/AthenaUM.sln" /t:Clean,Build /property:Configuration=Release /property:Platform=x64

cls

echo.
copy "Athena\AthenaUM\x64\Release\AthenaUM.exe" .
copy "Athena\AthenaEFI\x64\Release\AthenaEFI.efi" "USB\"
echo.

echo [+] Compilation done

pause