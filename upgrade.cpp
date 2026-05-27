OS1 Oracle Hochziehen 


_15(win->id EQ 3) {

					_44 oracle_active = (windows[10].open AND !windows[10].minimized);

                    _43 mid=wx+ww/2; _43 btn_y = wy + 45; _30 lang_lbl[20], theme_lbl[30];

                    _15(sys_lang EQ 0) str_cpy(lang_lbl, "[ LANG: EN ]"); _41 str_cpy(lang_lbl, "[ SPR: DE ]");

                    _15(sys_lang EQ 0) { _15(sys_theme EQ 0) str_cpy(theme_lbl, "[ THEME: COMPUTER ]"); _41 str_cpy(theme_lbl, "[ THEME: GENESIS ]"); } 

					_41 { _15(sys_theme EQ 0) str_cpy(theme_lbl, "[ THEMA: COMPUTER ]"); 

					_41 str_cpy(theme_lbl, "[ THEMA: GENESIS ]"); }

                    _15(input_cooldown EQ 0 AND mouse_just_pressed AND !oracle_active AND is_over_rect(mouse_x, mouse_y, wx+5, btn_y, 140, 20)) { 

					sys_lang = !sys_lang; input_cooldown = 10; }

                    Text(wx+10, btn_y+4, lang_lbl, 0x000000, _128);

                    _15(input_cooldown EQ 0 AND mouse_just_pressed AND !oracle_active AND is_over_rect(mouse_x, mouse_y, wx+5, btn_y+30, 200, 20)) { 

					sys_theme = !sys_theme; input_cooldown = 10; }

                    Text(wx+10, btn_y+34, theme_lbl, 0x000000, _128);

                    TextC(mid, wy+45, "REBOOT", (sys_menu_selection EQ 0)?0x555555:0x000000, _128);

                    _39(_43 s=0; s<6; s++) { 

                        _43 ly = wy+85+s*15; _44 hov = is_over_rect(mouse_x, mouse_y, mid-50, ly-2, 100, 12);

                        _89 sc = (sleep_mode_idx EQ s OR sys_menu_selection EQ s+2) ? 0x888888 : 0x000000; _15(hov) sc = 0xFFFFFF;

                        _15(hov) DrawRoundedRect(mid-50, ly-2, 100, 14, 2, 0x000000); 

						TextC(mid, ly, sleep_labels[s], sc, _128); 

						_15(mouse_just_pressed AND  !blocked AND hov) sleep_mode_idx = s;

                    }

					/// --- BUTTON ZUM GERÄTE MANAGER (ORAKEL) ---

                    DrawRoundedRect(wx+20, wy+wh-80, ww-40, 25, 4, 0x00FF00); 

                    TextC(mid, wy+wh-73, "OPEN HARDWARE ORACLE", 0x000000, _128);

                    _15(mouse_just_pressed AND !oracle_active AND is_over_rect(mouse_x, mouse_y, wx+20, wy+wh-80, ww-40, 25)) {

                        windows[10].open = _128; windows[10].minimized = _86; focus_window(10);

                        input_cooldown = 10;

                    }

                    /// User und CPU bleiben mittig

                    TextC(mid, wy+190, user_name, 0x222222, _128); 

                    TextC(mid, wy+205, cpu_brand, 0x222222, _86);

                    /// Netzdaten am rechten Rand

                    _43 right_x = wx + ww - 160;

                    Text(right_x, wy+190, ip_address, 0x222222, _128); 

                    Text(right_x, wy+205, net_mask, 0x555555, _86); 

                    Text(right_x, wy+220, gateway_ip, 0x555555, _86);

                    DrawRoundedRect(wx+20, wy+240, ww-40, 2, 0, 0xAAAAAA);

                    _71 _30* l_hw = sys_lang ? "HARDWARE GEFUNDEN" : "DETECTED HARDWARE"; 

                    TextC(mid, wy+250, l_hw, 0x000000, _128);

                    _30 l_disk[30], l_net[30], l_gpu[30], l_usb[30];

                    _15(sys_theme EQ 0) { 

                        str_cpy(l_disk, sys_lang ? "FESTPLATTE:" : "STORAGE:"); 

                        str_cpy(l_net, sys_lang ? "NETZWERK:" : "NETWORK:"); 

                        str_cpy(l_gpu, sys_lang ? "GRAFIK:" : "GRAPHICS:"); 

                        str_cpy(l_usb, sys_lang ? "USB HOST:" : "USB HOST:"); 

                    } _41 { 

                        str_cpy(l_disk, sys_lang ? "ERD-ANKER:" : "TERRA ANCHOR:"); 

                        str_cpy(l_net, sys_lang ? "KOSMOS NETZ:" : "COSMIC WEB:"); 

                        str_cpy(l_gpu, sys_lang ? "REALITAETS RENDERER:" : "REALITY RENDERER:"); 

                        str_cpy(l_usb, sys_lang ? "DATEN INJEKTOR:" : "DATA INJECTOR:"); 

                    }

                    /// ==========================================

                    /// BARE METAL FIX: STORAGE KLICK & RENDER LOGIK

                    /// ==========================================

                    _72 _43 view_disk_idx = 0;

                    /// 1. KLICK ABFANGEN: Durch Festplatten schalten

                    _15(mouse_just_pressed AND !blocked AND is_over_rect(mouse_x, mouse_y, wx+30, wy+275, 250, 20)) { 

                        _15(drive_count > 0) {

                            view_disk_idx++;

                            _15(view_disk_idx >= drive_count) view_disk_idx = 0;

                            str_cpy(hw_disk, drives[view_disk_idx].model);

                        }

                        input_cooldown = 10;

                    }

                    /// 2. RENDERN: Label und Festplatten-Name

                    Text(wx+30, wy+280, l_disk, 0xAAAAAA, _128); 

                    _15(drive_count > 0) {

                        /// Sicherheits-Check

                        _15(view_disk_idx >= drive_count) view_disk_idx = 0;

                        /// Pfeil bei mehreren Platten

                        _15(drive_count > 1) { Text(wx+150, wy+280, ">", 0xFFFFFF, _128); }

                        /// Plattenname in Ozeanblau

                        Text(wx+165, wy+280, drives[view_disk_idx].model, 0x0044CC, _128);

                        /// --- SCAN BUTTON FÜR FAT32 ---

                        DrawRoundedRect(wx+30, wy+300, 60, 20, 4, 0x444444);

                        Text(wx+42, wy+305, "SCAN", 0xFFFFFF, _128);

                        /// Klick auf den SCAN-Button

                        _15(mouse_just_pressed AND !blocked AND is_over_rect(mouse_x, mouse_y, wx+30, wy+300, 60, 20)) { 

                            _30* mod = drives[view_disk_idx].model;

                            _44 is_fat32 = _86;

                            _44 is_cfs = _86;

                            /// Dateisystem am Namen erkennen

                            _39(_43 c=0; c<40; c++) {

                                _15(mod[c] EQ 'F' AND mod[c+1] EQ 'A' AND mod[c+2] EQ 'T') is_fat32 = _128;

                                _15(mod[c] EQ 'C' AND mod[c+1] EQ 'F' AND mod[c+2] EQ 'S') is_cfs = _128;

                            }

                            _15(is_fat32) {

                                /// FAT32 Scanner abfeuern

                                fat32_read_root_dir(drives[view_disk_idx].base_port);

                            } _41 _15(is_cfs) {

                                /// BARE METAL FIX: CFS Dateinamen auslesen!

                                _184 cfs_buf[512];

                                ahci_read_sectors(drives[view_disk_idx].base_port, 1002, 1, (_89)cfs_buf);

                                _39(_192 _43 wait=0; wait<500000; wait++) __asm__ _192("nop");

                                CFS_DIR_ENTRY* entries = (CFS_DIR_ENTRY*)cfs_buf;

                                _44 found = _86;

                                /// Inhaltsverzeichnis nach der ersten echten Datei durchsuchen

                                _39(_43 i=0; i<8; i++) {

                                    _15(entries[i].type EQ 1) {

                                        str_cpy(cmd_status, entries[i].filename);

                                        found = _128;

                                        _37; /// Erste gefundene Datei anzeigen und Schleife abbrechen

                                    }

                                }

                                _15(!found) {

                                    str_cpy(cmd_status, "CFS: DIR IS EMPTY");

                                }

                            } _41 {

                                /// Weder FAT32 noch CFS gefunden

                                str_cpy(cmd_status, "ERR: RAW UNFORMATTED DRIVE");

                            }

                            click_consumed = _128;

                            input_cooldown = 10;

                        }

                        /// Gefundene Dateien anzeigen

                        Text(wx+100, wy+305, "FILES:", 0xAAAAAA, _86);

                        Text(wx+150, wy+305, cmd_status, 0x00FFFF, _86); 



                    } _41 {

                        Text(wx+165, wy+280, "NO DRIVES MOUNTED", 0xFF0000, _86);

                    }

					/// ==========================================

                    /// BOOT KERNEL V2 BUTTON (GRUB MODULE EDITION)

                    /// ==========================================

                    _15(mouse_just_pressed AND !blocked AND is_over_rect(mouse_x, mouse_y, wx+30, wy+450, 250, 20)) {

                        _15(os2_ram_address NEQ 0) {

                            str_cpy(cmd_status, "GRUB MODULE FOUND! JUMPING...");

                            Swap();

                            /// Zündung OHNE Parameter!

                            execute_kernel(); 

                        } _41 {

                            str_cpy(cmd_status, "ERR: GRUB DID NOT LOAD KERNEL.BIN!");

                        }

                        input_cooldown = 30;

                    }

                    Text(wx+30, wy+450, "[ >>> KERNEL V2 STARTEN <<< ]", 0x00FF00, _128);

                    /// ==========================================

                    /// NETZWERK

                    /// ==========================================

                    _15(mouse_just_pressed AND !blocked AND is_over_rect(mouse_x, mouse_y, wx+30, wy+335, 250, 20)) {

                        str_cpy(cmd_status, "NETZWERK: CHECKING LINK...");

                        input_cooldown = 10;

                    }

                    Text(wx+30, wy+340, l_net, 0xAAAAAA, _128);  

                    Text(wx+190, wy+340, hw_net, 0x0044CC, _128);

                    /// ==========================================

                    /// GPU

                    /// ==========================================

                    _15(mouse_just_pressed AND !blocked AND is_over_rect(mouse_x, mouse_y, wx+30, wy+360, 250, 20)) {

                        _15(gpu_count > 0) {

                            _72 _43 view_gpu_idx = 0;

                            view_gpu_idx++;

                            _15(view_gpu_idx >= gpu_count) view_gpu_idx = 0;

                            str_cpy(hw_gpu, hw_gpu_list[view_gpu_idx]);

                            str_cpy(cmd_status, "GPU: CYCLE INFO");

                        }

                        input_cooldown = 10;

                    }

                    Text(wx+30, wy+365, l_gpu, 0xAAAAAA, _128);  

                    Text(wx+190, wy+365, hw_gpu, 0x0044CC, _128);



                    /// ==========================================

                    /// USB (HARDWARE DEBUGGER) - BRUTE FORCE SWEEPER

                    /// ==========================================

                    _15(mouse_just_pressed AND !blocked AND is_over_rect(mouse_x, mouse_y, wx+30, wy+385, 250, 20)) {

                        uint32_t usb_buffer_addr = 0x0C001000; 

                        uint8_t* usb_buffer = (uint8_t*)usb_buffer_addr;

                        _43 stick_found = 0;

                        /// Wir scannen Gerät 1 bis 4 und Endpoints 1 bis 4

                        for(_43 dev = 1; dev <= 4; dev++) {

                            for(_43 ep_in = 1; ep_in <= 4; ep_in++) {

                                for(_43 ep_out = 1; ep_out <= 4; ep_out++) {

                                    /// IN und OUT sind nie derselbe Endpoint

                                    _15(ep_in == ep_out) continue;

                                    /// Puffer leeren

                                    for(int i = 0; i < 512; i++) usb_buffer[i] = 0;

                                    /// Feuer frei!

                                    if(usb_bot_read_sectors(dev, ep_in, ep_out, 0, 1, usb_buffer_addr) == 0) {

                                        /// Hat der Stick geantwortet?

                                        if(usb_buffer[510] == 0x55 && usb_buffer[511] == 0xAA) {

                                            str_cpy(cmd_status, "USB JACKPOT! MBR GEFUNDEN!");

                                            stick_found = 1;

                                            break;

                                        }

                                    }

                                }

                                if(stick_found) break;

                            }

                            if(stick_found) break;

                        }

                        if(!stick_found) {

                            str_cpy(cmd_status, "USB SWEEP: KEIN STICK ANTWORTET (EHCI/XHCI?)");

                        }

                        input_cooldown = 20;

                    }

                    Text(wx+30, wy+390, l_usb, 0xAAAAAA, _128);  

                    Text(wx+190, wy+390, hw_usb, 0x0044CC, _128);

                    /// ==========================================

                    /// AUDIO (AC97 TESTER)

                    /// ==========================================

                    _15(mouse_just_pressed AND !blocked AND is_over_rect(mouse_x, mouse_y, wx+30, wy+410, 250, 20)) {

                        _15(ac97_bus_port != 0) {

                            play_ac97_flare(); /// Zündet deinen Sonnen-Sound als Test!

                            

                            _30 port_str[15];

                            hex_to_str(ac97_bus_port, port_str);

                            str_cpy(cmd_status, "AUDIO: AC97 DMA FIRE @ PORT ");

                            str_cat(cmd_status, port_str);

                        } _41 {

                            str_cpy(cmd_status, "AUDIO: CHIP SLEEPING OR NOT FOUND");

                        }

                        input_cooldown = 15;

                    }

                    Text(wx+30, wy+415, "AUDIO", 0xAAAAAA, _128);  

                    

                    /// Zeigt blaues "ONLINE", wenn der Orakel-Scanner den Port gefunden hat

                    _15(ac97_bus_port != 0) Text(wx+190, wy+415, "AC97 ONLINE", 0x0044CC, _128);

                    _41 Text(wx+190, wy+415, "OFFLINE", 0x555555, _128);

                    /// ==========================================

                    /// SHUT DOWN / REBOOT LOGIK

                    /// ==========================================

                    TextC(mid, wy+wh-40, "SHUT DOWN", (sys_menu_selection EQ 8)?0x555555:0x000000, _128); 

                    _15(mouse_just_pressed AND !blocked) { 

                        _15(is_over(mouse_x,mouse_y,mid,wy+45,20)) system_reboot(); 

                        _15(is_over(mouse_x,mouse_y,mid,wy+wh-40,20)) system_shutdown(); 

                    }

                }





//////////////



/// =========================================================

                /// APP: HARDWARE ORACLE / FORENSICS (ID 10)

                /// =========================================================

                _15(win->id EQ 10) {

                    static _43 oracle_view = 0;

                    /// --- DIE SEITENLEISTE (TOOLS) ---

                    DrawRoundedRect(wx, wy+20, 150, wh-20, 0, 0x222222);

                    Text(wx+10, wy+35, "ORACLE TOOLS", 0xAAAAAA, _128);

                    /// Tool 1: PCI Scanner

                    _89 col_t1 = (oracle_view EQ 0) ? 0x00FF00 : 0x555555;

                    DrawRoundedRect(wx+10, wy+60, 130, 25, 4, col_t1);

                    TextC(wx+75, wy+68, "PCI SCANNER", 0x000000, _128);

                    _15(mouse_just_pressed AND !blocked AND is_over_rect(mouse_x, mouse_y, wx+10, wy+60, 130, 25)) {

                        oracle_view = 0; input_cooldown = 10;

                    }

                    /// Tool 2: CFS Forensik (Geisterdaten-Suche)

                    _89 col_t2 = (oracle_view EQ 1) ? 0x00FF00 : 0x555555;

                    DrawRoundedRect(wx+10, wy+90, 130, 25, 4, col_t2);

                    TextC(wx+75, wy+98, "CFS FORENSICS", 0x000000, _128);

                    _15(mouse_just_pressed AND !blocked AND is_over_rect(mouse_x, mouse_y, wx+10, wy+90, 130, 25)) {

                        oracle_view = 1; input_cooldown = 10;

                    }

                    /// --- DER HAUPTBEREICH (RECHTS) ---

                    _43 hx = wx + 160; 

                    _43 hw = ww - 160;

                    /// ---------------------------------------------------------

                    /// VIEW 0: DEIN ORIGINALER SPIEGEL-SCAN: HYBRID ENGINE

                    /// ---------------------------------------------------------

                    _15(oracle_view EQ 0) {

                        TextC(hx+hw/2, wy+40, "SPIEGEL-SCAN: HYBRID ENGINE", 0x00FF00, _128);

                        /// BUTTON 1: STUFE 1 (Oberflächen-Scan + BAR0)

                        DrawRoundedRect(hx+20, wy+60, 120, 25, 3, 0x333333);

                        TextC(hx+80, wy+68, "1. INIT SCAN", 0xFFFFFF, _128);

                        /// BUTTON 2: STUFE 2 (Nur Nullen untersuchen!)

                        DrawRoundedRect(hx+150, wy+60, 120, 25, 3, 0x333333);

                        TextC(hx+210, wy+68, "2. DEEP READ", 0xFFFFFF, _128);

                        /// ----------------------------------------------------

                        /// KLICK 1: INIT SCAN (Sicher für alle Mainboards)

                        /// ----------------------------------------------------

                        _15(input_cooldown EQ 0 AND mouse_just_pressed AND !blocked AND is_over_rect(mouse_x, mouse_y, hx+20, wy+60, 120, 25)) {

                            mirror_count = 0;

                            _39(_43 b=0; b<10; b++) {

                                _39(_184 d=0; d<32; d++) {

                                    _39(_184 f=0; f<8; f++) {

                                        _89 id = sys_pci_read(b, d, f, 0);

                                        _182 ven = id & 0xFFFF;

                                        _15(ven NEQ 0xFFFF AND ven NEQ 0x0000 AND mirror_count < 20) {

                                            mirror_list[mirror_count].bus = b;

                                            mirror_list[mirror_count].dev = d;

                                            mirror_list[mirror_count].func = f;

                                            mirror_list[mirror_count].vendor = ven;

                                            mirror_list[mirror_count].device = id >> 16;

                                            _89 bar0_raw = sys_pci_read(b, d, f, 0x10);

                                            _15(bar0_raw NEQ 0 AND bar0_raw NEQ 0xFFFFFFFF AND (bar0_raw & 1) EQ 0) {

                                                mirror_list[mirror_count].bar0 = bar0_raw & 0xFFFFFFF0;

                                            } _41 {

                                                mirror_list[mirror_count].bar0 = 0; 

                                            }

                                            _89 class_rev = sys_pci_read(b, d, f, 0x08);

                                            _184 cls = (class_rev >> 24) & 0xFF;

                                            _184 sub = (class_rev >> 16) & 0xFF;

                                            _15(cls EQ 0x01 AND sub EQ 0x06) str_cpy(mirror_list[mirror_count].name, "AHCI CONTROLLER");

                                            _41 _15(cls EQ 0x01 AND sub EQ 0x01) str_cpy(mirror_list[mirror_count].name, "IDE CONTROLLER");

                                            _41 _15(cls EQ 0x02) str_cpy(mirror_list[mirror_count].name, "NETWORK CHIP");

                                            _41 _15(cls EQ 0x03) str_cpy(mirror_list[mirror_count].name, "GRAPHICS GPU");

                                            _41 _15(cls EQ 0x0C AND sub EQ 0x03) str_cpy(mirror_list[mirror_count].name, "USB HUB (xHCI)");

                                            _41 _15(cls EQ 0x06) str_cpy(mirror_list[mirror_count].name, "PCI BRIDGE");

                                            _41 str_cpy(mirror_list[mirror_count].name, "SYSTEM DEVICE");

                                            mirror_count++;

                                        }

                                    }

                                }

                            }

                            mirror_scanned = _128;

                            input_cooldown = 10;

                        }

                        /// ----------------------------------------------------

                        /// KLICK 2: DEEP READ (Der Brücken-Inspektor)

                        /// ----------------------------------------------------

                        _15(input_cooldown EQ 0 AND mouse_just_pressed AND !blocked AND is_over_rect(mouse_x, mouse_y, hx+150, wy+60, 120, 25) AND mirror_scanned) {

                            _39(_43 i=0; i<mirror_count; i++) {

                                _89 b = mirror_list[i].bus;

                                _89 d = mirror_list[i].dev;

                                _89 f = mirror_list[i].func;

                                _89 class_rev = sys_pci_read(b, d, f, 0x08);

                                _184 cls = (class_rev >> 24) & 0xFF;

                                _15(cls EQ 0x06) {

                                    _89 mem_base_limit = sys_pci_read(b, d, f, 0x20);

                                    _89 base_addr = (mem_base_limit & 0xFFFF) << 16;

                                    _15(base_addr NEQ 0) { mirror_list[i].bar0 = base_addr; }

                                }

                                _15(cls EQ 0x01 AND mirror_list[i].bar0 EQ 0) {

                                    _89 bar5 = sys_pci_read(b, d, f, 0x24);

                                    _15(bar5 NEQ 0 AND bar5 NEQ 0xFFFFFFFF) { mirror_list[i].bar0 = bar5 & 0xFFFFFFF0; }

                                }

                            }

                            input_cooldown = 10;

                        }

                        /// --- ERGEBNISSE ZEICHNEN ---

                        _15(mirror_scanned) {

                            Text(hx+20, wy+100, "B:D:F", 0xAAAAAA, _128);

                            Text(hx+80, wy+100, "VENDOR DEVICE", 0xAAAAAA, _128);

                            Text(hx+190, wy+100, "BASE ADDR", 0xAAAAAA, _128);

                            Text(hx+280, wy+100, "CLASS", 0xAAAAAA, _128);

                            _43 ry = wy+120;

                            _39(_43 i=0; i<mirror_count; i++) {

                                _30 s_b[10], s_v[10], s_d[10], s_bar[20];

                                int_to_str(mirror_list[i].bus, s_b);

                                hex_to_str(mirror_list[i].vendor, s_v);

                                hex_to_str(mirror_list[i].device, s_d);

                                _30 h1[10], h2[10];

                                hex_to_str((_182)(mirror_list[i].bar0 >> 16), h1); 

                                hex_to_str((_182)(mirror_list[i].bar0 & 0xFFFF), h2);

                                str_cpy(s_bar, h1); str_cpy(s_bar+4, h2);

                                Text(hx+20, ry, s_b, 0x888888, _86);

                                Text(hx+80, ry, s_v, 0x00FF00, _86); 

                                Text(hx+125, ry, s_d, 0x00FF00, _86);

                                _89 text_col_bar = (mirror_list[i].bar0 EQ 0) ? 0xFF0000 : 0xFFFFFF;

                                Text(hx+190, ry, s_bar, text_col_bar, _86);

                                Text(hx+280, ry, mirror_list[i].name, 0xCCCCCC, _128);

                                ry += 15;

                            }

                        }

                    }

                    /// ---------------------------------------------------------

                    /// VIEW 1: DAS NEUE FORENSIK TOOL (UNDELETE)

                    /// ---------------------------------------------------------

                    _41 _15(oracle_view EQ 1) {

                        Text(hx+20, wy+40, "CFS GHOST DATA RECOVERY", 0x00FF00, _128);

                        _15(active_drive_idx < 0 OR active_drive_idx >= drive_count) {

                            Text(hx+20, wy+80, "NO DRIVE SELECTED IN DISK MGR!", 0xFF0000, _128);

                        } _41 {

                            _30 title[60] = "SCANNING: ";

                            str_cat(title, drives[active_drive_idx].model);

                            Text(hx+20, wy+65, title, 0xAAAAAA, _128);

                            /// 1. Verzeichnis per DMA lesen

                            _43 port = drives[active_drive_idx].base_port;

                            _89 dir_ram_addr = 0x08000000; 

                            ahci_read_sectors(port, 1002, 1, dir_ram_addr);

                            _39(_192 _43 wait=0; wait<500000; wait++) __asm__ _192("nop");

                            CFS_DIR_ENTRY* entries = (CFS_DIR_ENTRY*)dir_ram_addr;

                            _43 y_off = wy + 100;

                            _44 found_ghost = _86;

                            /// 2. Geisterdaten suchen

                            _39(_43 i=0; i<8; i++) {

                                _15(entries[i].type EQ 0 AND entries[i].filename[0] NEQ 0) {

                                    found_ghost = _128;

                                    DrawRoundedRect(hx+20, y_off, hw-40, 25, 3, 0x222222);

                                    Text(hx+30, y_off+5, entries[i].filename, 0x888888, _128);

                                    Text(hx+150, y_off+5, "[DELETED]", 0xCC0000, _128);

                                    /// RECOVER BUTTON

                                    DrawRoundedRect(hx+hw-100, y_off+3, 70, 19, 3, 0x00AA00);

                                    Text(hx+hw-85, y_off+5, "RECOVER", 0xFFFFFF, _86);

                                    _15(input_cooldown EQ 0 AND mouse_just_pressed AND !blocked AND is_over_rect(mouse_x, mouse_y, hx+hw-100, y_off+3, 70, 19)) {

                                        entries[i].type = (entries[i].start_lba > 0) ? 1 : 2; 

                                        ahci_write_sectors(port, 1002, 1, dir_ram_addr);

                                        _39(_192 _43 wait=0; wait<500000; wait++) __asm__ _192("nop");

                                        input_cooldown = 20;

                                        click_consumed = _128;

                                    }

                                    y_off += 30;

                                }

                            }

                            _15(!found_ghost) {

                                Text(hx+20, y_off, "NO RECOVERABLE GHOST DATA FOUND.", 0x555555, _128);

                            }

                        }

                    }

                }
