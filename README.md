  ******
  ******
  Your OS can format, write/read CFS, read FAT32 and exFAT, and it doesn't crash on modern Windows drives!

***with RUFUS in DD MODE THE ISO or Ventoy just copy the ISO on the USB-Stick***<img width="497" height="721" alt="image" src="https://github.com/user-attachments/assets/c074f8c4-bd73-43d8-b329-534915a1e63e" />

  
***My PC is old , so***
+ use qemu for quick test / under construction (add REAL hardware)

***The solution: You absolutely must enable CSM (Compatibility Support Module) or Legacy Boot in the BIOS of your test computer, otherwise the computer will completely ignore your USB stick.***    
***yeah its easier then you know or think to put a video into the README (all EMBED code crap at the internet not work !)***


https://github.com/user-attachments/assets/48e6fa54-ce7e-4704-960c-55f482039838



  ******
  ******
***WSL COMMANDS:***
+ + DRVIERS HDD / CDROM / USB 
+ + ***WSL COMMANDS for QEMU Quick Test***
+ dd if=/dev/zero of=cosmos_usb.img bs=1M count=16
+ qemu-img create -f raw cosmos_hdd.img 100M
+ dd if=/dev/zero of=cosmos_drive.img bs=1M count=64
+ qemu-system-i386 -cdrom cosmos.iso -m 512   -device ich9-ahci,id=ahci0   -drive id=disk0,file=cosmos_drive.img,format=raw,if=none   -device ide-hd,drive=disk0,bus=ahci0.0   -drive id=disk1,file=cosmos_hdd.img,format=raw,if=none   -device ide-hd,drive=disk1,bus=ahci0.1   -device piix3-usb-uhci,id=usb0   -drive id=usbstick,file=cosmos_usb.img,format=raw,if=none   -device usb-storage,bus=usb0.0,drive=usbstick
 
  ******
  ******

***update 13.04.2026***

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

 ******
 ******

 + The Network Stack (Your Latest Milestone)
 + You've made the leap from an isolated PC to a network-enabled system.
 + PCI Bus Mastering: Your E1000 network card can now write directly and asynchronously to RAM (0x880000) via DMA.
 + ARP (Address Resolution Protocol): Your kernel can query for MAC addresses on the LAN (CONNECT) and parse the responses.
 + DHCP Client: You can send a broadcast (DHCP), the router responds, and your kernel automatically extracts its own IP address from the UDP packet.
 + UDP Payloads: You have the functions (send_udp, send_cosmos_block) to send your own raw data blocks to the network.
 + The Custom Window Manager (GUI)
 + Hardware & Storage (I/O)
 + PCI scanner: You read the entire bus, identify vendor IDs, and differentiate between Realtek (8139), Intel (E1000), GPUs, and USB controllers.
 + ATA/IDE Hard Drives: Your ata_write_sector and ata_read_sector (PIO LBA28) read and write actual sectors on the hard drive.
 + Real-Time Clock (RTC): You can read the hardware clock directly via port 0x70/0x71.
 + Audio & Effects: PC speaker support (play_freq) including boot music and the awesome "Earthquake" screen-shake effect.
 + APPS :
 + CMD (ID 5): Your command prompt with history, parsing, and direct connection to the network stack (CONNECT, DHCP, IPCONFIG).
 + Cosmos Explorer (ID 4): A graphical file manager that visualizes your file system and can load files into the editor.
 + Notepad & Save As (ID 0 & 2): A text editor that writes sectors directly to the disk via the ATA driver.***
  ******
  ******
<img width="638" height="468" alt="image" src="https://github.com/user-attachments/assets/98b826c6-6d75-4d5e-a371-9e7d651f6561" />)<img width="698" height="127" alt="image" src="https://github.com/user-attachments/assets/fa0f31a6-f00c-467e-8a13-ef162c88e40c" />
