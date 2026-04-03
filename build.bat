@echo off
setlocal enabledelayedexpansion

echo ==========================================
echo COSMOS OS - BARE METAL BUILD (x64)
echo ==========================================

:: 1. PFADE DEFINIEREN
set CLANG_INC="C:\Program Files\LLVM\lib\clang\22\include"
set ISO_DIR=iso_root
set KERNEL=kernel.elf
set ISO_NAME=cosmos.iso

:: 2. KOMPILIEREN & LINKEN
echo [1/4] Compiling...
clang++ -target x86_64-pc-none-elf -ffreestanding -fno-exceptions -fno-rtti -mno-red-zone -mcmodel=kernel -I %CLANG_INC% -c kernel_x64.cpp -o kernel_x64.o
if %errorlevel% neq 0 goto :error

echo [2/4] Linking...
ld.lld -flavor gnu -T linker.ld -nostdlib -static -e kernel_main kernel_x64.o -o %KERNEL%
if %errorlevel% neq 0 goto :error

:: 3. ISO STRUKTUR VORBEREITEN
echo [3/4] Preparing ISO structure...
if not exist %ISO_DIR% mkdir %ISO_DIR%

:: Kopiere notwendige Dateien (Stelle sicher, dass diese im Hauptordner liegen!)
copy %KERNEL% %ISO_DIR%\ /Y
copy limine.conf %ISO_DIR%\ /Y
copy limine-bios.sys %ISO_DIR%\ /Y
copy limine-bios-cd.bin %ISO_DIR%\ /Y
copy limine-uefi-cd.bin %ISO_DIR%\ /Y

echo [3.5/4] Erzeuge saubere limine.conf (Limine 8.x Syntax)...
:: Alte configs rigoros loeschen
del /f /q "iso_root\limine.con*" 2>nul

:: Frische config per Terminal generieren
echo timeout: 5> "iso_root\limine.conf"
echo.>> "iso_root\limine.conf"
echo /COSMOS OS> "iso_root\limine.conf"
echo     protocol: limine>> "iso_root\limine.conf"
echo     kernel_path: boot():/kernel.elf>> "iso_root\limine.conf"
:: 4. ISO ERSTELLEN & BOOTLOADER INSTALLIEREN
echo [4/4] Creating ISO and Deploying Limine...
:: Erstellt das ISO-Image
xorriso -as mkisofs -b limine-bios-cd.bin -no-emul-boot -boot-load-size 4 -boot-info-table --efi-boot limine-uefi-cd.bin -efi-boot-part --efi-boot-image --protective-msdos-label %ISO_DIR% -o %ISO_NAME%

:: Installiert den BIOS-Bootsektor in das ISO
limine bios-install %ISO_NAME%

echo.
echo ==========================================
echo FERTIG! Datei: %ISO_NAME%
echo ==========================================
goto :end

:error
echo.
echo [FEHLER] Build abgebrochen.
exit /b 1

:end
pause