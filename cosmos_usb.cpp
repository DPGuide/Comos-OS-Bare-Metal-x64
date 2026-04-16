/// ==========================================
/// COSMOS OS - USB BULK-ONLY TRANSPORT (BOT)
/// ==========================================
#include "schneider_lang.h"
#pragma pack(push, 1)
struct USB_CBW {
    _89 signature;       
    _89 tag;             
    _89 transfer_len;    
    _184 flags;          
    _184 lun;            
    _184 cmd_len;        
    _184 scsi_cmd[16];   
};
struct USB_CSW {
    _89 signature;       
    _89 tag;             
    _89 data_residue;    
    _184 status;         
};
#pragma pack(pop)
/// ==========================================
/// BARE METAL FIX: UHCI HOST CONTROLLER (DER POSTBOTE)
/// ==========================================
_172 _89 usb_io_base; /// Wird vom Orakel/Kernel befüllt
_172 _50 outw(_182 p, _182 v);
_172 _182 inw(_182 p);
/// Das Formular für den USB-Chip (MUSS 16-Byte aligned sein!)
struct UHCI_TD {
    _89 link_ptr;      /// Zeiger auf das nächste Formular (1 = Ende)
    _89 ctrl_status;   /// Status-Flags (Aktiv, Fehler, Toggle)
    _89 token;         /// PID (IN/OUT), Endpoint, Device Address
    _89 buffer_ptr;    /// Pointer auf den Datenpuffer
    _89 pad[4];        /// Padding auf 32 Bytes
} __attribute__((aligned(16)));
/// Wir legen den Briefkasten (Frame List) und das Formular (TD) an absolut sichere RAM-Adressen!
/// Wir legen den Briefkasten an die absolut sichere 32-MB-Marke im RAM!
_89* uhci_frame_list = (_89*)0x02000000; 
UHCI_TD* current_td  = (UHCI_TD*)0x02001000;
/// --- DER EXECUTOR: Schickt das Formular ab ---
_44 uhci_execute_td(_89 pid, _43 dev_addr, _43 ep, void* buffer, _43 len) {
    _15(usb_io_base EQ 0) _96 _86;
    /// 1. Briefkasten einrichten (Neue 32MB Adresse an den Chip melden)
    outw(usb_io_base + 8, 0x02000000 & 0xFFFF);
    outw(usb_io_base + 10, 0x02000000 >> 16);
    /// 2. Das Formular (TD) ausfüllen
    current_td->link_ptr = 1;
    /// Status: Active (Bit 23), Error Limit = 3 (Bit 27-28)
    current_td->ctrl_status = (1 << 23) | (3 << 27);
    /// Token: Length, Data Toggle (Bit 19), Endpoint, Device, PID
    _89 max_len = (len > 0) ? (len - 1) : 0x7FF; 
    current_td->token = (max_len << 21) | (0 << 19) | (ep << 15) | (dev_addr << 8) | pid;
    current_td->buffer_ptr = (_89)buffer;
    /// 3. Formular in den Briefkasten werfen
    _39(_43 i=0; i<1024; i++) uhci_frame_list[i] = (_89)current_td;
    /// 4. DEN BUZZER DRÜCKEN! (Controller starten)
    outw(usb_io_base, inw(usb_io_base) | 0x0001);
    /// 5. Warten, bis der Chip das "Active" Bit (23) löscht
    _43 timeout = 1000000;
    _114((current_td->ctrl_status & (1 << 23)) AND timeout > 0) {
        __asm__ _192("pause");
        timeout--;
    }
    /// 6. Controller wieder anhalten
    outw(usb_io_base, inw(usb_io_base) & ~0x0001);
    _15(timeout EQ 0 OR (current_td->ctrl_status & 0x7E0000)) _96 _86;
    _96 _128; 
}
/// --- DIE SCHNITTSTELLEN FÜR DEIN BOT-PROTOKOLL ---
_44 usb_bulk_out(_43 dev_addr, _43 ep, void* buffer, _43 len) {
    _96 uhci_execute_td(0xE1, dev_addr, ep, buffer, len); /// 0xE1 = OUT PID
}
_44 usb_bulk_in(_43 dev_addr, _43 ep, void* buffer, _43 len) {
    _96 uhci_execute_td(0x69, dev_addr, ep, buffer, len); /// 0x69 = IN PID
}
/// ==========================================
/// BARE METAL FIX: USB HANDSCHLAG V2 (TIMING & DMA)
/// ==========================================
#pragma pack(push, 1)
struct USB_SETUP_PKT {
    _184 request_type;
    _184 request;
    _182 value;
    _182 index;
    _182 length;
};
#pragma pack(pop)
/// Wir legen das Paket auf eine sichere, feste RAM-Adresse für den USB-Chip!
USB_SETUP_PKT* global_setup = (USB_SETUP_PKT*)0x02002000;
_44 uhci_execute_control(_43 dev_addr) {
    /// WICHTIG: Dem Controller sagen, wo der Briefkasten im RAM liegt!
    outw(usb_io_base + 8, 0x02000000 & 0xFFFF);
    outw(usb_io_base + 10, 0x02000000 >> 16);
    /// PHASE 1: SETUP-Kommando schicken (PID 0x2D, Data0)
    current_td->link_ptr = 1;
    current_td->ctrl_status = (1 << 23) | (3 << 27);
    current_td->token = (7 << 21) | (0 << 19) | (0 << 15) | (dev_addr << 8) | 0x2D; 
    current_td->buffer_ptr = (_89)global_setup;
    _39(_43 i=0; i<1024; i++) uhci_frame_list[i] = (_89)current_td;
    outw(usb_io_base, inw(usb_io_base) | 0x0001); /// RUN
    _43 timeout = 1000000;
    _114((current_td->ctrl_status & (1 << 23)) AND timeout > 0) { __asm__ _192("pause"); timeout--; }
    outw(usb_io_base, inw(usb_io_base) & ~0x0001); /// STOP
    _15(timeout EQ 0 OR (current_td->ctrl_status & 0x7E0000)) _96 _86;
    /// PHASE 2: STATUS-Bestätigung abholen (PID 0x69, Data1)
    current_td->link_ptr = 1;
    current_td->ctrl_status = (1 << 23) | (3 << 27);
    current_td->token = (0x7FF << 21) | (1 << 19) | (0 << 15) | (dev_addr << 8) | 0x69; 
    current_td->buffer_ptr = 0;
    _39(_43 i=0; i<1024; i++) uhci_frame_list[i] = (_89)current_td;
    outw(usb_io_base, inw(usb_io_base) | 0x0001); /// RUN
    timeout = 1000000;
    _114((current_td->ctrl_status & (1 << 23)) AND timeout > 0) { __asm__ _192("pause"); timeout--; }
    outw(usb_io_base, inw(usb_io_base) & ~0x0001); /// STOP
    _15(timeout EQ 0 OR (current_td->ctrl_status & 0x7E0000)) _96 _86;
    _96 _128;
}
/// ==========================================
/// BARE METAL FIX: USB HANDSCHLAG V3 (QEMU SAFE)
/// ==========================================
/// Eine sichere Mikro-Pause, die die CPU nicht einfriert
_50 uhci_safe_delay(_43 loops) {
    _39(volatile _43 i = 0; i < loops; i++) {
        __asm__ _192("pause"); /// Sagt der CPU: "Atme kurz durch", verhindert Hitzestau/Stottern
    }
}
/// Die eigentliche Anmeldung
_44 usb_enumerate_device(_43 port_idx, _43 new_address) {
    _182 port_reg = usb_io_base + 0x10 + (port_idx * 2);
    /// 1. HARDWARE RESET (QEMU SAFE)
    /// 0xFFD5 verhindert, dass wir wichtige Status-Bits versehentlich löschen!
    _182 safe_p = inw(port_reg) & 0xFFD5;
    outw(port_reg, safe_p | 0x0200); 
    uhci_safe_delay(1000000);
    safe_p = inw(port_reg) & 0xFFD5;
    outw(port_reg, safe_p & ~0x0200);
    uhci_safe_delay(100000);
    safe_p = inw(port_reg) & 0xFFD5;
    outw(port_reg, safe_p | 0x0004); 
    uhci_safe_delay(100000);
    _15(!(inw(port_reg) & 0x0004)) _96 _86;
    /// 2. SET ADDRESS
    global_setup->request_type = 0x00; 
    global_setup->request = 0x05;      
    global_setup->value = new_address; 
    global_setup->index = 0;
    global_setup->length = 0;
    _15(!uhci_execute_control(0)) _96 _86;
    uhci_safe_delay(100000);
    /// 3. SET CONFIGURATION
    global_setup->request_type = 0x00;
    global_setup->request = 0x09;      
    global_setup->value = 1;           
    global_setup->index = 0;
    global_setup->length = 0;
    _15(!uhci_execute_control(new_address)) _96 _86;
    _96 _128;
}
_44 usb_bot_read_sectors(_43 dev_addr, _43 ep_out, _43 ep_in, _89 lba, _43 num_sectors, _184* buffer) {
    USB_CBW cbw;
    _39(_43 i=0; i<sizeof(cbw); i++) ((_184*)&cbw)[i] = 0;
    cbw.signature = 0x43425355; 
    cbw.tag = 0x00000001;       
    cbw.transfer_len = num_sectors * 512;
    cbw.flags = 0x80;           
    cbw.lun = 0;
    cbw.cmd_len = 10;
    cbw.scsi_cmd[0] = 0x28;
    cbw.scsi_cmd[2] = (lba >> 24) & 0xFF; 
    cbw.scsi_cmd[3] = (lba >> 16) & 0xFF;
    cbw.scsi_cmd[4] = (lba >> 8) & 0xFF;
    cbw.scsi_cmd[5] = lba & 0xFF;
    cbw.scsi_cmd[7] = (num_sectors >> 8) & 0xFF; 
    cbw.scsi_cmd[8] = num_sectors & 0xFF;
    /// PHASE 1: Command senden
    _15(!usb_bulk_out(dev_addr, ep_out, &cbw, sizeof(cbw))) _96 _86;
    /// PHASE 2: Daten empfangen 
    _15(!usb_bulk_in(dev_addr, ep_in, buffer, cbw.transfer_len)) _96 _86;
    /// PHASE 3: Status abfragen
    USB_CSW csw;
    _15(!usb_bulk_in(dev_addr, ep_in, &csw, sizeof(csw))) _96 _86;
    /// Prüfen, ob der Stick sich verschluckt hat
    _15(csw.signature NEQ 0x53425355 OR csw.status NEQ 0) _96 _86;
    _96 _128; 
}
/// ==========================================
/// BARE METAL FIX: READ CAPACITY (0x25)
/// ==========================================
_43 usb_bot_get_capacity(_43 dev_addr, _43 ep_out, _43 ep_in) {
    USB_CBW cbw;
    _39(_43 i=0; i<sizeof(cbw); i++) ((_184*)&cbw)[i] = 0;
    cbw.signature = 0x43425355; 
    cbw.tag = 0x00000002;       /// Tag 2 für diesen Befehl
    cbw.transfer_len = 8;       /// Wir erwarten exakt 8 Bytes Antwort!
    cbw.flags = 0x80;           /// IN: Wir wollen Daten haben
    cbw.lun = 0;
    cbw.cmd_len = 10;
    cbw.scsi_cmd[0] = 0x25;
    /// PHASE 1: Kommando abschicken
    _15(!usb_bulk_out(dev_addr, ep_out, &cbw, sizeof(cbw))) _96 0;
    /// PHASE 2: Die 8 Bytes Antwort lesen
    _184 capacity_data[8];
    _15(!usb_bulk_in(dev_addr, ep_in, capacity_data, 8)) _96 0;
    /// PHASE 3: Status abholen (CSW)
    USB_CSW csw;
    _15(!usb_bulk_in(dev_addr, ep_in, &csw, sizeof(csw))) _96 0;
    /// SCSI nutzt Big Endian! Wir müssen die ersten 4 Bytes umdrehen:
    _89 last_lba = (capacity_data[0] << 24) | (capacity_data[1] << 16) | (capacity_data[2] << 8) | capacity_data[3];
    /// Formel: (Sektoren * 512 Bytes) / 1024 / 1024 = Megabytes
    /// Gekürzt ist das einfach: Sektoren / 2048
    _43 size_mb = (last_lba + 1) / 2048;
    _96 size_mb;
}