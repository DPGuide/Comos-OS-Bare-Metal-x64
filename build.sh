#!/bin/bash

# BARE METAL FIX: Bricht das Skript sofort ab, wenn der Compiler einen Fehler wirft!
set -e
# BARE METAL FIX: Bricht das Skript sofort ab, wenn der Compiler einen Fehler wirft!
set -e

echo "0. Alten Müll aufräumen (Clean Build)..."
rm -f *.o *.elf
rm -f isodir/boot/kernel.bin
rm -f isodir/KERNEL.BIN
rm -f cosmos.iso

echo "1. Ordner für GRUB anlegen..."
mkdir -p isodir/boot/grub

echo "2. Assembler Boot-Header kompilieren (32-Bit)..."
as --32 boot.s -o boot.o

echo "3. Kompiliere OS Teil 1 (Dein Disk-Manager / Bootloader)..."
g++ -m32 -O2 -c kernel.cpp -o kernel.o -ffreestanding -fno-exceptions -fno-rtti -fno-pic -Wno-misleading-indentation -Wno-sign-compare -Wno-unused-variable -Wno-int-to-pointer-cast
g++ -m32 -O2 -c pci.cpp -o pci.o -ffreestanding -fno-exceptions -fno-rtti -fno-pic -Wno-misleading-indentation -Wno-sign-compare -Wno-unused-variable -Wno-int-to-pointer-cast
g++ -m32 -O2 -c net.cpp -o net.o -ffreestanding -fno-exceptions -fno-rtti -fno-pic -Wno-misleading-indentation -Wno-sign-compare -Wno-unused-variable -Wno-int-to-pointer-cast
g++ -m32 -O2 -c cosmos_bytes.cpp -o cosmos_bytes.o -ffreestanding -fno-exceptions -fno-rtti -fno-pic -Wno-misleading-indentation -Wno-sign-compare -Wno-unused-variable -Wno-int-to-pointer-cast
g++ -m32 -O2 -c cosmos_fs.cpp -o cosmos_fs.o -ffreestanding -fno-exceptions -fno-rtti -fno-pic -Wno-misleading-indentation -Wno-sign-compare -Wno-unused-variable -Wno-int-to-pointer-cast
g++ -m32 -O2 -c cosmos_tba.cpp -o cosmos_tba.o -ffreestanding -fno-exceptions -fno-rtti -fno-pic -Wno-misleading-indentation -Wno-sign-compare -Wno-unused-variable -Wno-int-to-pointer-cast
g++ -m32 -O2 -c cosmos_ahci.cpp -o cosmos_ahci.o -ffreestanding -fno-exceptions -fno-rtti -fno-pic -Wno-misleading-indentation -Wno-sign-compare -Wno-unused-variable -Wno-int-to-pointer-cast
g++ -m32 -O2 -c cosmos_cfs.cpp -o cosmos_cfs.o -ffreestanding -fno-exceptions -fno-rtti -fno-pic -Wno-misleading-indentation -Wno-sign-compare -Wno-unused-variable -Wno-int-to-pointer-cast
g++ -m32 -O2 -c cosmos_usb.cpp -o cosmos_usb.o -ffreestanding -fno-exceptions -fno-rtti -fno-pic -Wno-misleading-indentation -Wno-sign-compare -Wno-unused-variable -Wno-int-to-pointer-cast

echo "4. Linke OS Teil 1 (Mit Multiboot-Header für GRUB)..."
# ACHTUNG: Hier darf kernel_main.o NICHT drinstehen!
ld -m elf_i386 -T linker.ld -static -o isodir/boot/kernel.bin boot.o kernel.o pci.o net.o cosmos_bytes.o cosmos_fs.o cosmos_tba.o cosmos_ahci.o cosmos_cfs.o cosmos_usb.o

# ==========================================
# NEUER SCHRITT: BAUE DAS NEUE OS (PAYLOAD)
# ==========================================
echo "5. Kompiliere OS Teil 2 (Die flache KERNEL.BIN)..."
# A) C++ zu Objektdatei
g++ -m32 -O2 -c kernel_main.cpp -o kernel_main.o -ffreestanding -fno-exceptions -fno-rtti -fno-pic
# B) Linken mit 16MB Adresse (0x1000000)
ld -m elf_i386 -Ttext 0x1000000 --entry main kernel_main.o -o kernel_main.elf
# C) In eine rohe Binärdatei konvertieren und direkt ins CD-Hauptverzeichnis legen
objcopy -O binary kernel_main.elf isodir/KERNEL.BIN

echo "6. GRUB Menü erstellen..."
cat > isodir/boot/grub/grub.cfg << EOF
set timeout=0
set default=0
menuentry "Cosmos OS" {
    set gfxpayload=800x600x32
    multiboot /boot/kernel.bin
    boot
}
EOF

echo "7. ISO generieren (grub-mkrescue ruft xorriso auf)..."
grub-mkrescue -o cosmos.iso isodir

echo "ISO erfolgreich gebaut! Du kannst cosmos.iso jetzt in QEMU starten."

# ==========================================
# 8. AUTO-BACKUP ALLER SOURCE-FILES
# ==========================================
echo "Erstelle Backup der C++ und Header Dateien..."
mkdir -p backup

for file in *.cpp *.h; do
    if [ -f "$file" ]; then
        base="${file%.*}"
        ext="${file##*.}"
        count=1
        
        while [ -f "backup/${base}_${count}.${ext}" ]; do
            ((count++))
        done
        
        cp "$file" "backup/${base}_${count}.${ext}"
    fi
done

echo "Backup sicher unter /backup/ abgelegt!"