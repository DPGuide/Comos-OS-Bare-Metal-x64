My PC is old , so 

***The solution: You absolutely must enable CSM (Compatibility Support Module) or Legacy Boot in the BIOS of your test computer, otherwise the computer will completely ignore your USB stick.***    
***yeah its easier then you know or think to put a video into the README (all EMBED code crap at the internet not work !)***

https://github.com/user-attachments/assets/79b9b1fa-d518-4435-82da-7836880f9b3a

***INCLUDES***
+ #include "schneider_lang.h"
+ #include "pci.h"
+ #include "net.h"
+ #include "cosmos_bytes.h"
+ #include "cosmos_fs.h"
  
<img width="432" height="219" alt="image" src="https://github.com/user-attachments/assets/66bcdf7e-f4e7-4595-ae12-9d23ee497cf0" /><img width="455" height="248" alt="image" src="https://github.com/user-attachments/assets/a043fc47-542d-429f-ac74-2f184694371a" />
<img width="450" height="247" alt="image" src="https://github.com/user-attachments/assets/800622e5-9b4a-4a77-9917-d58874323edc" />

***update 06.14.2026***
***1. The Network Stack (Your Latest Milestone)
You've made the leap from an isolated PC to a network-enabled system.
PCI Bus Mastering: Your E1000 network card can now write directly and asynchronously to RAM (0x880000) via DMA.
ARP (Address Resolution Protocol): Your kernel can query for MAC addresses on the LAN (CONNECT) and parse the responses.
DHCP Client: You can send a broadcast (DHCP), the router responds, and your kernel automatically extracts its own IP address from the UDP packet.
UDP Payloads: You have the functions (send_udp, send_cosmos_block) to send your own raw data blocks to the network.
2. The Custom Window Manager (GUI)
This is probably the visual highlight. You're not just rendering pixels, you're managing complex states:
Window logic: Drag & drop, resizing, minimizing, fullscreen, and Z-ordering (windows overlap correctly).
The "planetary" UI: Minimized apps move into a dynamic orbit around your center.
Input: Custom PS/2 keyboard driver (including shift logic and key press delay) and a clean PS/2 mouse driver.
Graphics engine: Custom sine/cosine lookup for performance, alpha blending (transparency in the orbit lines), and font rendering (even with bold text).
3. Hardware & Storage (I/O)
PCI scanner: You read the entire bus, identify vendor IDs, and differentiate between Realtek (8139), Intel (E1000), GPUs, and USB controllers.
ATA/IDE Hard Drives: Your ata_write_sector and ata_read_sector (PIO LBA28) read and write actual sectors on the hard drive.
Real-Time Clock (RTC): You can read the hardware clock directly via port 0x70/0x71.
Audio & Effects: PC speaker support (play_freq) including boot music and the awesome "Earthquake" screen-shake effect.
4. The Apps
CMD (ID 5): Your command prompt with history, parsing, and direct connection to the network stack (CONNECT, DHCP, IPCONFIG).
Cosmos Explorer (ID 4): A graphical file manager that visualizes your file system and can load files into the editor.
Notepad & Save As (ID 0 & 2): A text editor that writes sectors directly to the disk via the ATA driver.***


