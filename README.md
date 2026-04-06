My PC is old , so 

***The solution: You absolutely must enable CSM (Compatibility Support Module) or Legacy Boot in the BIOS of your test computer, otherwise the computer will completely ignore your USB stick.***

<iframe  frameborder="0" 
        style=" width: 100%; height: 110%;" src="[https://app](https://github.com/DPGuide/Comos-OS-Bare-Metal-x64/blob/main/0406.mp4)">
      </iframe>
      
      <iframe width="560" height="315" src="[https://www.youtube.com/embed/fxKnmW0QxHQ](https://github.com/DPGuide/Comos-OS-Bare-Metal-x64/blob/main/0406.mp4)" frameborder="0" allow="accelerometer; autoplay; clipboard-write; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>

      <iframe src="[https://www.google.com/maps/embed?pb=!1m14!1m12!1m3!1d38877164.76467194!2d-95.677068!3d37.06250000000001!2m3!1f0!2f0!3f0!3m2!1i1024!2i768!4f13.1!5e0!3m2!1sen!2sus!4v1422925644298](https://github.com/DPGuide/Comos-OS-Bare-Metal-x64/blob/main/0406.mp4)" width="600" height="450" frameborder="0" style="border:0"></iframe>

      <object width="625" height="800">
  <embed src="[http://url](https://github.com/DPGuide/Comos-OS-Bare-Metal-x64/blob/main/0406.mp4)"
         type="application/x-shockwave-flash"
         wmode="transparent" width="625" height="800" />
</object>
      
<iframe src="https://github.com/DPGuide/Comos-OS-Bare-Metal-x64/blob/main/0406.mp4" width="560" height="315" width="600" height="450"></iframe>

***INCLUDES***
+ #include "schneider_lang.h"
+ #include "pci.h"
+ #include "net.h"
+ #include "cosmos_bytes.h"
+ #include "cosmos_fs.h"
  
<img width="432" height="219" alt="image" src="https://github.com/user-attachments/assets/66bcdf7e-f4e7-4595-ae12-9d23ee497cf0" /><img width="455" height="248" alt="image" src="https://github.com/user-attachments/assets/a043fc47-542d-429f-ac74-2f184694371a" />
<img width="450" height="247" alt="image" src="https://github.com/user-attachments/assets/800622e5-9b4a-4a77-9917-d58874323edc" />

<img width="799" height="659" alt="image" src="https://github.com/user-attachments/assets/7395f95f-94e1-4303-9076-3ffb0484a883" />

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


