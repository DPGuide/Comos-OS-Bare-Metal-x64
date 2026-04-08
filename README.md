***My PC is old , so***
+ use qemu for quick test / under construction (add REAL hardware)

***The solution: You absolutely must enable CSM (Compatibility Support Module) or Legacy Boot in the BIOS of your test computer, otherwise the computer will completely ignore your USB stick.***    
***yeah its easier then you know or think to put a video into the README (all EMBED code crap at the internet not work !)***

https://github.com/user-attachments/assets/79b9b1fa-d518-4435-82da-7836880f9b3a

  ***Bare-Metal-SATA-Driver!***
  
  ***qemu-system-x86_64 -cdrom cosmos.iso -hda cosmos_hdd.img***
  
***create for test QEMU "dd if=/dev/zero of=cosmos_drive.img bs=1M count=64"***
+ for boot with 100 MB HDD image "qemu-system-i386 -cdrom cosmos.iso \
  -device ahci,id=ahci \
  -drive if=none,file=cosmos_drive.img,id=sata_disk,format=raw \
  -device ide-hd,drive=sata_disk,bus=ahci.0"

***INCLUDES - all BAREMETAL***
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

***update 06.14.2026***
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

<img width="798" height="603" alt="image" src="https://github.com/user-attachments/assets/7d9fe7e1-41c6-4916-bae6-78d4903e1ca5" />

<img width="517" height="341" alt="image" src="https://github.com/user-attachments/assets/c707b310-ed25-4674-8807-7a8873ded320" /><img width="478" height="325" alt="image" src="https://github.com/user-attachments/assets/e1d2085e-8b44-41e8-b192-4c37f932adea" />

***Earlie easter egg / Format Disk to install Cosmos OS***
<img width="798" height="599" alt="image" src="https://github.com/user-attachments/assets/44805546-cdb7-4293-b94b-08d3dddabe33" />
<img width="573" height="442" alt="image" src="https://github.com/user-attachments/assets/337cf577-607b-4c99-abe2-1269b1a1203a" />
<img width="765" height="563" alt="image" src="https://github.com/user-attachments/assets/76521a50-ca49-41b1-9c36-5c20809db9d3" />

***Ping Pong - BareMetal***

https://github.com/user-attachments/assets/7c9e9b0f-48f5-4ff4-9125-79d3032fd856

***Blobbey BareMetal***

https://github.com/user-attachments/assets/e41248a9-968c-4747-ac06-b0fdac0dd0f2
