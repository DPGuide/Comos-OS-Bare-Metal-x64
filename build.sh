#!/bin/bash
set -e

# 1. Aufräumen & Verzeichnisstruktur
rm -rf build isoroot efiboot.img cosmos.iso
mkdir -p build isoroot/boot

# 2. Kernel Kompilieren (Deine Warnungen werden hier ignoriert)
echo "[1/5] Compiling Kernel..."
clang++ -target x86_64-unknown-none-elf -O2 -ffreestanding \
    -fno-stack-protector -fno-pic -m64 -march=x86-64 \
    -mno-red-zone -mcmodel=kernel -fno-exceptions -fno-rtti \
    -Wno-empty-body \
    -c kernel.cpp -o build/kernel.o

echo "[2/5] Linking..."
ld.lld -m elf_x86_64 -static -T linker.ld build/kernel.o -o isoroot/boot/kernel.elf

# 3. Limine Binaries sicherstellen
echo "[3/5] Checking Limine binaries..."
BASE_URL="https://raw.githubusercontent.com/limine-bootloader/limine/v8.x-binary"

for file in limine-bios.sys limine-bios-cd.bin limine-uefi-cd.bin BOOTX64.EFI; do
    if [ ! -f "$file" ]; then
        echo "Lade $file herunter..."
        curl -L "$BASE_URL/$file" -o "$file"
    fi
done

# [4/5] Das UEFI-FAT-Image bauen
echo "Erstelle EFI boot image..."
dd if=/dev/zero of=efiboot.img bs=1M count=4
mkfs.vfat efiboot.img
mmd -i efiboot.img ::/EFI
mmd -i efiboot.img ::/EFI/BOOT
mcopy -i efiboot.img BOOTX64.EFI ::/EFI/BOOT/

# WICHTIG: Die Datei muss in den isoroot Ordner!
mkdir -p isoroot/boot
cp efiboot.img isoroot/boot/

# [5/5] Die ISO brennen
echo "Erstelle ISO..."
cp limine.conf limine-bios.sys limine-bios-cd.bin limine-uefi-cd.bin isoroot/

xorriso -as mkisofs \
    -b limine-bios-cd.bin \
    -no-emul-boot -boot-load-size 4 -boot-info-table \
    --eltorito-alt-boot \
    -e boot/efiboot.img \
    -no-emul-boot \
    -isohybrid-gpt-basdat \
    -o cosmos.iso \
    isoroot/
echo "------------------------------------------"
echo "FERTIG: cosmos.iso erstellt."
echo "Wähle in Rufus nun den 'ISO-Abbild Modus'!"