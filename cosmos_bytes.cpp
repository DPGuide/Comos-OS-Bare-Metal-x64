#include "cosmos_bytes.h"
#include "schneider_lang.h"
/// ------------------------------------------
/// 1. CHECKSUMME (Jetzt über den VOLLEN 256-Block)
/// ------------------------------------------
_184 cb_checksum(_71 _184* block) {
    _184 cs = 0;
    /// Wir XORen jetzt bis Index 253, um den ganzen Block zu sichern
    _39(_43 i = 0; i < 254; i++) {
        cs ^= block[i];
    }
    _96 cs;
}
/// ------------------------------------------
/// 2. FRAME VALIDIEREN (Bleibt identisch, nur Indices wandern nach hinten)
/// ------------------------------------------
_44 cb_validate(_71 _184* block) {
    _15(block[0] NEQ ANCHOR_BYTE) _96 _86;
    /// Der Footer sitzt jetzt am Ende des 256-Byte Blocks (Index 255)
    _15(block[255] NEQ FOOTER_BYTE) _96 _86;
    /// Die Prüfsumme sitzt jetzt auf Index 254
    _15(block[254] NEQ cb_checksum(block)) _96 _86;
    _96 _128;
}
/// ------------------------------------------
/// 3. BIG-BLOCK BAUEN (Behält deine 26-Byte-Struktur bei!)
/// ------------------------------------------
_50 cb_build(_184* out_block, _182 sensor_val, _184 r, _184 g, _184 b, _184 a) {
    /// 1. Alles auf Null setzen
    _39(_43 i = 0; i < 256; i++) out_block[i] = 0;
    /// --- CLASSIC CORE (0-25) ---
    out_block[0] = ANCHOR_BYTE;
    out_block[1] = 0xC0;
    /// Sensor (2-3) bleibt!
    out_block[2] = (_184)(sensor_val >> 8);
    out_block[3] = (_184)(sensor_val & 0xFF);
    /// Der Handshake-Slot (7) für Transmitter/Receiver bleibt frei!
    out_block[7] = 0x00;
    /// VRAM (10-13) bleibt!
    out_block[10] = r; out_block[11] = g; out_block[12] = b; out_block[13] = a;
    /// "MTX" Magic String (15-17) bleibt!
    out_block[15] = 'M'; out_block[16] = 'T'; out_block[17] = 'X';
    /// Frame-ID (18-19) aus deinem Transmitter bleibt!
    /// (Hier kannst du weitere Metadaten einfügen)

    /// --- MANTRA PAYLOAD (26-253) ---
    /// Hier ist jetzt massig Platz für Rohdaten, die dein 
    /// TBA-Packer oder ArShiva verarbeiten kann.
    
    /// --- FINALE (254-255) ---
    out_block[254] = cb_checksum(out_block);
    out_block[255] = FOOTER_BYTE;
}
