  ******
  ******
  ***Big Fixes & updates , Oracle can find mostly everykind of Hardwere Address it doesnt matter if old Hardware or New***


https://github.com/user-attachments/assets/48e6fa54-ce7e-4704-960c-55f482039838

16 OS1 16 Bit BareMEtal
<img width="797" height="598" alt="image" src="https://github.com/user-attachments/assets/ab1aa5c1-4e27-479e-bb6b-31812c86cd5c" />
OS2 16 - 64 Bit Bare Metal
<img width="735" height="575" alt="image" src="https://github.com/user-attachments/assets/09d68d32-95e2-4582-a91f-5ef7bbddf63b" />




***with RUFUS in DD MODE THE ISO or Ventoy just copy the ISO on the USB-Stick***<img width="497" height="721" alt="image" src="https://github.com/user-attachments/assets/c074f8c4-bd73-43d8-b329-534915a1e63e" />

  ******
  ******
***For QEMU QUICK TEST - WSL COMMANDS:***
+ + DRVIERS HDD / CDROM / USB 
+ + ***WSL COMMANDS for QEMU Quick Test***
+ dd if=/dev/zero of=cosmos_usb.img bs=1M count=16
+ qemu-img create -f raw cosmos_hdd.img 100M
+ dd if=/dev/zero of=cosmos_drive.img bs=1M count=64

qemu-system-x86_64 -boot d -cdrom cosmos.iso -m 512 -device ich9-ahci,id=ahci0 -drive id=disk0,file=cosmos_drive.img,format=raw,if=none -device ide-hd,drive=disk0,bus=ahci0.0 -drive id=disk1,file=cosmos_hdd.img,format=raw,if=none -device ide-hd,drive=disk1,bus=ahci0.1 -device qemu-xhci,id=xhci0 -drive id=usbstick,file=cosmos_usb.img,format=raw,if=none -device usb-storage,bus=xhci0.0,drive=usbstick

  ******
  ******

***update 17.04.2026***

***INCLUDES - all BAREMETAL***
 ***Bare-Metal-SATA-Driver!***
+ #include "schneider_lang.cpp"
+ + #include "schneider_lang.h"
+ #include "pci.cpp"
+ + #include "pci.h"
+ #include "net.cpp"
+ + #include "net.h"
+ #include "cosmos_bytes.cpp"
+ + #include "cosmos_bytes.h"
+ #include "cosmos_fs.cpp"
+ + #include "cosmos_fs.h"
+ #include "cosmos_tba.cpp"
+ #cosmos_ahci.cpp
+ +#cosmos_ahci.f
+ #cosmos_cfs.cpp
+ + #cosmos_cfs.h
+ #include "kernel_main.cpp"
+ #include "cosmos_pci.cpp"
+ #include "cosmos_usb.cpp"
+ + Graphic Design & Oracle DB (its awesome)
 ******
 ******
