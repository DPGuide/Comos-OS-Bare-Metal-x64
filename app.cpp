#include <stdint.h>

extern "C" __attribute__((section(".text.entry"))) void app_main() {
    // 1. DIE MAGISCHE SIGNATUR
    __asm__ volatile("nop; nop; nop; nop;");
    
    // 2. RAM-FENSTER VOM KERNEL ANFORDERN (Syscall 4)
    // Die Adresse eures 50x50 Puffers landet in "window_buffer"
    volatile uint32_t* window_buffer;
    __asm__ volatile("mov $4, %%rax \n int $0x80" : "=a"(window_buffer) : : "memory");
    
    // 3. FLUMMI VARIABLEN
    long long x = 10;
    long long y = 10;
    long long dx = 1;
    long long dy = 1;
    long long color = 0x00FF00; /// Hacker-Grün!
    
    while(1) {
        // Alten Flummi löschen (0 = Transparent für den Kernel!)
        window_buffer[y * 50 + x] = 0;
        window_buffer[y * 50 + (x+1)] = 0;
        window_buffer[(y+1) * 50 + x] = 0;
        window_buffer[(y+1) * 50 + (x+1)] = 0;
        
        x += dx;
        y += dy;
        
        // Wand-Kollision (Der Puffer ist 50x50 Pixel groß!)
        if (x <= 1) dx = 1;
        if (x >= 48) dx = -1;
        if (y <= 1) dy = 1;
        if (y >= 48) dy = -1;
        
        // Neuen Flummi in den Puffer zeichnen
        window_buffer[y * 50 + x] = color;
        window_buffer[y * 50 + (x+1)] = color;
        window_buffer[(y+1) * 50 + x] = color;
        window_buffer[(y+1) * 50 + (x+1)] = color;
        
        // Kurze Bremse
        for(volatile int i = 0; i < 500000; i++) {}
        
        // Yield (Syscall 0)
        __asm__ volatile("mov $0, %%rax \n int $0x80" : : : "rax", "memory");
    }
}
