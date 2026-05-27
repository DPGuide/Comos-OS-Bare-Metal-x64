#include "cosmos_ahci.h"
#include "cosmos_bytes.h"
#include "schneider_lang.h"
/// ==========================================
/// ARSHIVA STREAM & TBA PACKER (BARE METAL)
/// ==========================================
/// Fester RAM-Block für unser 4KB Cluster (16 Blöcke á 256 Bytes)
_184* tba_ram_buffer = (_184*)0x900000;
_43 tba_block_idx = 0;
/// Festplatten-Positionen für den Stream
/// Wir reservieren 10.000 Sektoren (~5 MB) ab LBA 3000 für die Endlos-Aufzeichnung
_89 tba_current_lba = 3000; 
_89 tba_max_lba = 13000;
_50 tba_flush_to_disk() {
    HBA_PORT* port = get_active_ahci_port();
    /// Fallback: Wenn keine Platte da ist, verwerfen wir den Puffer, 
    /// damit der RAM nicht überläuft und die Main-Loop nicht crasht.
    _15(port EQ 0) { 
        tba_block_idx = 0; 
        _96; 
    }
    /// Wir schreiben das 4KB Cluster in 8 sequentiellen 512-Byte-Sektoren.
    /// Dein ahci_write() ist auf 1 Sektor pro Aufruf limitiert, daher der Loop.
    _39(_43 s = 0; s < 8; s++) {
        _184* sector_ptr = tba_ram_buffer + (s * 512);
        ahci_write(port, tba_current_lba + s, sector_ptr);
    }
    /// Cursor auf der Platte um 8 Sektoren vorschieben
    tba_current_lba += 8;
    /// Puffer zurücksetzen für die nächsten 16 Frames
    tba_block_idx = 0;
    /// Ring-Puffer Logik: Wenn wir das Ende des reservierten Platzes erreichen,
    /// springen wir wieder auf Anfang und überschreiben die ältesten Daten.
    _15(tba_current_lba >= tba_max_lba) {
        tba_current_lba = 3000;
    }
}
/// Diese Funktion rufst Du in Deiner Main-Loop in kernel.cpp auf!
_50 tba_record_frame(_182 sensor_val, _184 r, _184 g, _184 b, _184 a) {
    /// 1. Offset im Puffer berechnen (Jeder Block belegt exakt 256 Bytes)
    _184* target_ptr = tba_ram_buffer + (tba_block_idx * 256);
    /// 2. Den Block über Deine cosmos_bytes Engine bauen.
    /// cb_build() nullt den restlichen Payload-Bereich automatisch aus!
    cb_build(target_ptr, sensor_val, r, g, b, a);
    tba_block_idx++;
    /// 3. Ist das 4KB Cluster voll? Dann ab auf die Platte!
    _15(tba_block_idx EQ 16) {
        tba_flush_to_disk();
    }
}
/// ==========================================
/// NETZWERK HOOK (Linker Fix für net.cpp)
/// ==========================================
_50 tba_master_stream(_184* network_payload) {
    /// 1. Offset im 4KB DMA Puffer berechnen
    _184* target_ptr = tba_ram_buffer + (tba_block_idx * 256);
    /// 2. Den 256-Byte Block 1:1 aus dem Netzwerk in den Ringpuffer kopieren
    _39(_43 i = 0; i < 256; i++) {
        target_ptr[i] = network_payload[i];
    }
    tba_block_idx++;
    /// 3. Ist das 4KB Cluster voll? Dann ab auf die Platte!
    _15(tba_block_idx EQ 16) {
        tba_flush_to_disk();
    }
}
