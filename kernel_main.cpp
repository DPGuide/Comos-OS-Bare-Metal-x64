/// kernel_main.cpp (Wird zu KERNEL.BIN kompiliert)
/// Er muss wissen, wie das Paket aussieht!
#include "boot_info.h"
#include "schneider_lang.h"
/// extern "C" verbietet dem Compiler, den Namen zu verändern!
extern "C" _50 main(BootInfo* sys_info) {
    _89 fb = sys_info->framebuffer_addr;
    _43 drives = sys_info->ahci_drive_count;
    /// Wir färben den Bildschirm zum Test komplett Blau
    _43* screen = (_43*)fb;
    _39(_43 i = 0; i < (sys_info->screen_width * sys_info->screen_height); i++) {
        screen[i] = 0x000000FF; 
    }
    /// Endlosschleife des neuen OS
    _114(1) {
        asm volatile("hlt");
    }
}