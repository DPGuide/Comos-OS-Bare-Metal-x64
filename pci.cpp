#include "pci.h"
#include "schneider_lang.h"
/// --- BARE METAL ORAKEL IMPORT ---
_202 OracleEntry {
    _30 type[8];     
    _182 vendor;     
    _182 device;     
    _89 bar_addr;    
};
_172 OracleEntry oracle_db[16];
_172 _43 oracle_entry_count;
/// ------------------------------------------
/// _172E REFERENZEN (liegen in kernel.cpp)
/// ------------------------------------------
_172 _89 mmio_read32(_89 addr);
_172 _50 int_to_str(_43 val, _30* str);
_172 _30 hw_storage[48];
_172 NICInfo found_nics[5];
_172 _43 nic_count;
_172 _43 active_nic_idx;
_172 _89 ata_base;
_172 _44 usb_detected;
_172 _89 usb_io_base;
_172 _44 logitech_found;
_172 _44 webcam_active;
_172 _30 hw_gpu[48];
_172 _30 hw_usb[48];
_172 _30 webcam_model[40];
_172 _30 hw_net[48];
/// _172e Funktionen aus kernel.cpp
_172 _50 outl(_182 p, _89 v);
_172 _89 inl(_182 p);
_172 _50 str_cpy(_30* d, _71 _30* s);
_172 _50 nic_select_next();
_172 _50 ahci_init(_89 abar_address);
_172 _50 byte_to_hex(_184 b, _30* out);
/// Um den NVMe-Scan nicht zu brechen
_202 PhysicalDrive; /// Forward declaration
_172 PhysicalDrive drives[8];
_172 _43 drive_count;
/// --- STRUKTUR FÜR DAS GFX-ORAKEL ---
_202 GraphicsCardInfo {
    _182 vendor_id;
    _182 device_id;
    _184 bus, slot, func;
    _94  framebuffer_addr; /// Die heilige Adresse (32-Bit)
};
GraphicsCardInfo main_gfx_card;
/// --- DER SCANNER ---
_50 pci_find_gfx_card() {
    main_gfx_card.framebuffer_addr = 0;
    /// BARE METAL FIX: Auch hier auf _43 (int) ändern!
    _39(_43 bus = 0; bus < 256; bus++) {
        _39(_184 slot = 0; slot < 32; slot++) {
            _39(_184 func = 0; func < 8; func++) {
                /// Nutzt DEINE pci_read Funktion (Offset 0x00 für IDs)
                _89 id = pci_read(bus, slot, func, 0);
                _182 vendor = id & 0xFFFF;
                _15(vendor EQ 0xFFFF) _37;
                /// Klasse und Subklasse liegen bei Offset 0x08
                _89 class_rev = pci_read(bus, slot, func, 0x08);
                _184 cls = (class_rev >> 24) & 0xFF;
                _184 sub = (class_rev >> 16) & 0xFF;
				_184 prog_if = (class_rev >> 8) & 0xFF;
                /// 0x03 = Display Controller, 0x00 = VGA Compatible
                _15(cls EQ 0x03 AND sub EQ 0x00) {
                    main_gfx_card.vendor_id = vendor;
                    main_gfx_card.device_id = (id >> 16) & 0xFFFF;
                    main_gfx_card.bus = bus;
                    main_gfx_card.slot = slot;
                    main_gfx_card.func = func;
                    /// HIER IST DAS GOLD: Lese BAR 0 (Offset 0x10)
                    _89 bar0 = pci_read(bus, slot, func, 0x10);
                    /// Bereinige die Speichertyp-Flags (unterste 4 Bits abschneiden)
                    main_gfx_card.framebuffer_addr = bar0 & 0xFFFFFFF0;
                    /// Rückkehr zum Kernel - wir haben das Ziel!
                    _96; 
                }
            }
        }
    }
}
/// ------------------------------------------
/// PCI IMPLEMENTIERUNG
/// ------------------------------------------
_89 pci_read(_184 bus, _184 slot, _184 func, _184 offset) { 
    _89 address = (_89)((bus << 16) | (slot << 11) | (func << 8) | (offset & 0xFC) | ((_89)0x80000000)); 
    outl(0xCF8, address); 
    _96 inl(0xCFC); 
}
_50 pci_write(_184 bus, _184 slot, _184 func, _184 offset, _89 value) { 
    _89 address = (_89)((bus << 16) | (slot << 11) | (func << 8) | (offset & 0xFC) | ((_89)0x80000000)); 
    outl(0xCF8, address); 
    outl(0xCFC, value); 
}
_50 pci_scan_all() {
    nic_count = 0;
    _39(_43 bus = 0; bus < 256; bus++) { 
        _39(_184 dev = 0; dev < 32; dev++) {
            /// BARE METAL FIX: Die 8 Funktionen eines PCIe-Geräts scannen!
            /// Mainboards verstecken die I219-V fast immer auf Funktion 6.
            _39(_184 func = 0; func < 8; func++) {
                _89 id = pci_read(bus, dev, func, 0);
                _15((id & 0xFFFF) NEQ 0xFFFF) { 
                    _89 class_rev = pci_read(bus, dev, func, 0x08); 
                    _184 cls = (class_rev >> 24) & 0xFF; 
                    _184 sub = (class_rev >> 16) & 0xFF;
					_184 prog_if = (class_rev >> 8) & 0xFF;
                    _182 vendor = id & 0xFFFF; 
                    _182 device_id = (id >> 16) & 0xFFFF; /// Für das Orakel und AHCI
                    /// ==========================================
                    /// DAS ORAKEL WIRD BEFRAGT
                    /// ==========================================
                    _44 oracle_match = _86;
                    _39(_43 o = 0; o < oracle_entry_count; o++) {
                        _15(oracle_db[o].vendor EQ vendor AND oracle_db[o].device EQ device_id) {
                            oracle_match = _128; /// Treffer in der DRIVERS.CFG!
                            /// 1. Ist es eine GPU?
                            _15(oracle_db[o].type[0] EQ 'G' AND oracle_db[o].type[1] EQ 'P' AND oracle_db[o].type[2] EQ 'U') {
                                str_cpy(hw_gpu, "ORACLE GPU");
                                main_gfx_card.framebuffer_addr = oracle_db[o].bar_addr;
                            }
                            /// 2. Ist es eine NETZWERKKARTE?
                            _15(oracle_db[o].type[0] EQ 'N' AND oracle_db[o].type[1] EQ 'E' AND oracle_db[o].type[2] EQ 'T') {
                                _15(nic_count < 5) { 
                                    found_nics[nic_count].address = oracle_db[o].bar_addr; 
                                    str_cpy(found_nics[nic_count].name, "ORACLE NIC"); 
                                    found_nics[nic_count].type = (vendor EQ 0x8086) ? 2 : 1; 
                                    nic_count++; 
                                }
                            }
                        }
                    }
                    /// Alte IDE Controller
                    _15(cls EQ 0x01 OR cls EQ 0x04) { 
                        _89 bar0 = pci_read(bus, dev, func, 0x10); 
                        _15((bar0 & 1) AND (bar0 > 1)) { ata_base = (bar0 & 0xFFFFFFFC); } 
                        _15(cls EQ 0x01 AND sub EQ 0x08) { 
                            _15(drive_count < 8) { 
                                _43 d = drive_count; 
                            } 
                        } 
                    }
                    /// --- NETZWERKKARTEN (ORAKEL WIRD IGNORIERT!) ---
                    _15(cls EQ 0x02) { 
                        _89 bar0 = pci_read(bus, dev, func, 0x10);
                        
                        /// DMA und Memory Space erzwingen
                        _89 cmd_reg = pci_read(bus, dev, func, 0x04);
                        pci_write(bus, dev, func, 0x04, cmd_reg | 0x07);
                        
                        _15(nic_count < 5) { 
                            found_nics[nic_count].address = bar0; 
                            
                            /// BARE METAL FIX: Vendor, Device UND BAR0 in einem String!
                            str_cpy(hw_net, "ID:");
                            _30* stat = hw_net + 3;
                            byte_to_hex(vendor >> 8, stat); stat+=2; byte_to_hex(vendor & 0xFF, stat); stat+=2;
                            *stat++ = '-';
                            byte_to_hex(device_id >> 8, stat); stat+=2; byte_to_hex(device_id & 0xFF, stat); stat+=2;
                            *stat++ = ' '; *stat++ = 'B'; *stat++ = ':';
                            byte_to_hex((bar0 >> 24) & 0xFF, stat); stat+=2;
                            byte_to_hex((bar0 >> 16) & 0xFF, stat); stat+=2;
                            byte_to_hex((bar0 >> 8) & 0xFF, stat); stat+=2;
                            byte_to_hex(bar0 & 0xFF, stat); stat+=2;
                            *stat = 0;

                            _15(vendor EQ 0x10EC) { str_cpy(found_nics[nic_count].name, "REALTEK"); found_nics[nic_count].type=1; } 
                            _41 _15(vendor EQ 0x8086) { str_cpy(found_nics[nic_count].name, "INTEL"); found_nics[nic_count].type=2; } 
                            _41 { str_cpy(found_nics[nic_count].name, "GENERIC"); found_nics[nic_count].type=0; } 
                            
                            nic_count++; 
                        } 
                    }
                    _15(cls EQ 0x03 AND !oracle_match) { str_cpy(hw_gpu, "GPU DEV"); }
                    /// --- BARE METAL FIX: XHCI / USB 3.0 HOST (64-BIT) ---
                    _15(cls EQ 0x0C AND sub EQ 0x03 AND prog_if EQ 0x30) { 
                        usb_detected = _128; 
                        
                        _89 cmd_reg = pci_read(bus, dev, func, 0x04);
                        pci_write(bus, dev, func, 0x04, cmd_reg | 0x07);

                        _89 bar0 = pci_read(bus, dev, func, 0x10);
                        
                        /// Prüfe, ob es eine 64-Bit BAR ist (Bit 1 und 2 = 10)
                        _182 bar_type = (bar0 >> 1) & 0x03;
                        _89 xhci_base = bar0 & 0xFFFFFFF0;

                        /// Wenn 64-Bit, lese BAR1 aus. Da wir im 32-Bit Modus sind,
                        /// können wir den oberen Teil meist ignorieren (wenn < 4GB),
                        /// aber das Auslesen "weckt" den Controller oft erst richtig auf.
                        _15(bar_type EQ 2) {
                            _89 bar1 = pci_read(bus, dev, func, 0x14);
                            /// Hier könntest du eine 64-Bit Variable befüllen, falls dein OS das unterstützt.
                        }
                        
                        _15(xhci_base > 0 AND xhci_base NEQ 0xFFFFFFF0) {
                            _89 cap_reg = mmio_read32(xhci_base);
                            
                            /// Sicherheits-Check: Wenn der RAM geblockt ist, lesen wir 0xFFFFFFFF
                            _15(cap_reg NEQ 0xFFFFFFFF) {
                                _184 caplength = cap_reg & 0xFF; 
                                _89 hcsparams1 = mmio_read32(xhci_base + 0x04);
                                _43 max_ports = hcsparams1 & 0xFF; 

                                _89 op_base = xhci_base + caplength;
                                _43 active_port = 0;
                                
                                _15(max_ports > 0 AND max_ports < 64) {
                                    _39(_43 i = 1; i <= max_ports; i++) {
                                        _89 portsc_addr = op_base + 0x400 + ((i - 1) * 0x10);
                                        _89 portsc = mmio_read32(portsc_addr);
                                        _15(portsc & 0x01) {
                                            active_port = i;
                                            _37; 
                                        }
                                    }
                                }

                                str_cpy(hw_usb, "USB: ");
                                _30* p = hw_usb + 5;
                                int_to_str(max_ports, p); _114(*p) p++;
                                *p++ = ' '; *p++ = 'P'; *p++ = 'O'; *p++ = 'R'; *p++ = 'T'; *p++ = 'S';
                                
                                _15(active_port > 0) {
                                    *p++ = ' '; *p++ = '|'; *p++ = ' ';
                                    *p++ = 'I'; *p++ = 'N'; *p++ = ':';
                                    int_to_str(active_port, p); _114(*p) p++;
                                }
                                *p = 0;
                            } _41 {
                                str_cpy(hw_usb, "USB HOST (MEM BLOCKED)");
                            }
                        }
                    }
                    /// ==========================================
                    /// BARE METAL FIX: CATCH-ALL STORAGE SCANNER
                    /// ==========================================
                    _15(cls EQ 0x01) { 
                        /// Wir fangen ALLES ab, was Klasse 0x01 (Speicher) ist.
                        str_cpy(hw_storage, "DRV: ");
                        _30* p = hw_storage + 5;
                        byte_to_hex(vendor >> 8, p); p+=2; byte_to_hex(vendor & 0xFF, p); p+=2;
                        *p++ = '-';
                        byte_to_hex(device_id >> 8, p); p+=2; byte_to_hex(device_id & 0xFF, p); p+=2;
                        *p++ = ' '; *p++ = 'T'; *p++ = ':';
                        byte_to_hex(sub, p); p+=2; /// T = Typ (04=RAID, 06=AHCI, 08=NVMe)
                        *p = 0;
                        
                        /// Wenn wir echtes AHCI (0x06) gefunden haben, 
                        /// brechen wir ab, damit es nicht überschrieben wird.
                        _15(sub EQ 0x06) {
                            /// Bus Mastering aktivieren
                            _89 cmd_reg = pci_read(bus, dev, func, 0x04);
                            pci_write(bus, dev, func, 0x04, cmd_reg | 0x07);
                        }
                    }
                    /// --- LOGITECH WEBCAM ---
                    _15(vendor EQ 0x046D) { 
                        logitech_found = _128; 
                        webcam_active = _128; 
                        str_cpy(webcam_model, "LOGITECH C270"); 
                    } 
                }
            }
        }
    }
    _15(nic_count > 0) { 
        active_nic_idx = -1; 
        nic_select_next(); 
    } _41 {
        str_cpy(hw_net, "NO NIC FOUND");
    }
}