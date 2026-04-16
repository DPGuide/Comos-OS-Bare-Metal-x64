#include "net.h"
#define BIG_BLOCK_SIZE 256
#include "schneider_lang.h"
_172 _50 mmio_write32(_89 addr, _89 val);
_172 _89 mmio_read32(_89 addr);
/// --- GLOBALE NETZWERK-POINTER (DIE EINZIG WAHREN) ---
_192 _89* rx_desc_intel = (_192 _89*)0x3800000;
_43 rx_cur_intel = 0;
_192 _89* tx_desc_intel = (_192 _89*)0x4800000;
_43 tx_cur_intel = 0;
_172 _50 tba_master_stream(_184* network_payload);
/// --- PROTOTYPEN ---
_50 e1000_enable_rx();
_50 e1000_enable_tx();
_50 e1000_check_rx();
_202 undi_transmit_t {
    _182 status;
    _182 protocol;
    _182 len;
    _89  buffer_ptr;
} __attribute__((packed));
undi_transmit_t undi_tx_pkg;
_172 _89 rtl_io_base; 
_172 _89 intel_mem_base;
_172 _184 mac_addr[6];
_172 _30 mac_str[24];
_172 _184* tx_buffer;
_172 _43 tx_cur;
_172 _184* rx_buffer_rtl;
_172 _43 rx_idx_rtl;
_172 _30 cmd_status[32];
_172 _30 ip_address[32];
_30 net_mask[32] = "255.255.255.0";
_30 gateway_ip[32] = "0.0.0.0";
_172 _30 cmd_last_out[128];
_172 NICInfo found_nics[5];
_172 _43 active_nic_idx;
_172 _44 str_starts(_71 _30* s1, _71 _30* s2);
_172 _89 random();
_172 _182 chk(_50* d, _43 l);
_172 _50 outl(_182 p, _89 v);
_172 _50 outw(_182 p, _182 v);
_172 _50 outb(_182 p, _184 v);
_172 _184 inb(_182 p);
_172 _50 str_cpy(_30* d, _71 _30* s);
_172 _43 str_len(_71 _30* s);
_172 _50 byte_to_hex(_184 b, _30* out);
_172 _182 hs(_182 v);
_172 _89 hl(_89 v);
_172 _50 int_to_str(_43 val, _30* str);
/// ==========================================
/// FUNKTIONEN
/// ==========================================
_202 PXE_Struct {
    _184 signature[4];
    _184 length;
    _184 checksum;
    _184 revision;
    _184 reserved1;
    _89  entry_point_sp;
    _89  entry_point_esp;
} __attribute__((packed));
PXE_Struct* global_pxe = 0;
_81 _182 (*UNDI_ENTRY)(_182, _50*);
_50 find_undi_entry() {
    _39(_89 addr = 0x10000; addr < 0x9FFFF; addr += 16) {
        _184* p = (_184*)addr;
        _15(p[0] EQ '!' AND p[1] EQ 'P' AND p[2] EQ 'X' AND p[3] EQ 'E') {
            global_pxe = (PXE_Struct*)addr;
            str_cpy(cmd_status, "UNDI INTERFACE: FOUND");
            _96;
        }
    }
    str_cpy(cmd_status, "UNDI INTERFACE: NOT FOUND");
}
_50 net_raw(_50* d, _89 l) {
    _15(rtl_io_base > 0) {
        _39(_89 i=0;i<l;i++) tx_buffer[i]=((_184*)d)[i];
        outl(rtl_io_base+0x20+(tx_cur*4),(_89)(uintptr_t)tx_buffer); 
        outl(rtl_io_base+0x10+(tx_cur*4),l);
        str_cpy(cmd_status, "RTL: NATIVE TX FIRED!");
    }
    _15(intel_mem_base > 0) {
        _89 mac_status = mmio_read32(intel_mem_base + 0x0008);
        _15((mac_status & 0x02) EQ 0) {
            str_cpy(cmd_status, "INTEL ERR: LINK DOWN (WAIT!)");
            _96; 
        }

        _89 phys_addr = 0x4000000 + (tx_cur_intel * 2048);
        _184* target_buf = (_184*)(uintptr_t)phys_addr;
        
        _39(_89 i = 0; i < l; i++) target_buf[i] = ((_184*)d)[i];
        _182 send_len = l;
        _15(send_len < 60) {
            _39(_89 i = l; i < 60; i++) target_buf[i] = 0;
            send_len = 60; 
        }

        _192 _89* desc_ptr = (_192 _89*)(uintptr_t)(0x4800000 + (tx_cur_intel * 16));
        desc_ptr[0] = phys_addr; 
        desc_ptr[1] = 0;         
        
        /// --- BARE METAL FIX: DER MAGISCHE DESKRIPTOR ---
        /// Bit 24 (0x01000000) = EOP (End of Packet)
        /// Bit 25 (0x02000000) = IFCS (Insert Frame Check Sequence)
        /// Bit 27 (0x08000000) = RS (Report Status)
        /// = 0x0B000000 | Länge
        desc_ptr[2] = send_len | 0x0B000000; 
        
        /// WICHTIG: Das Status-Feld (Bits 32-35) MUSS genullt werden!
        desc_ptr[3] = 0;

        __asm__ _192("wbinvd" ::: "memory");

        tx_cur_intel = (tx_cur_intel + 1) % 32;
        mmio_write32(intel_mem_base + 0x3818, tx_cur_intel);
        
        str_cpy(cmd_status, "INTEL: TX PUSHED");
    }
}
_50 net_ip(_89 dst, _50* p_data, _182 p_len, _184 proto) {
    _184 b[1514]; EthernetFrame* e=(EthernetFrame*)b; IPHeader* i=(IPHeader*)(b+14);
    _39(_43 k=0;k<6;k++){e->dest_mac[k]=0xFF; e->src_mac[k]=mac_addr[k];} e->type=hs(0x0800);
    i->ver_ihl=0x45; i->len=hs(20+p_len); i->id=hs(random()); i->frag=hs(0x4000);
    i->ttl=64; i->proto=proto;
    _15(dst EQ 0xFFFFFFFF) i->src = 0; 
    _41 i->src = hl(0x0A00020F);
    i->dst=hl(dst);
    /// Setze Checksumme sauber mit DEINER originalen chk-Funktion
    i->chk = 0; 
    i->chk = chk((_50*)i, 20);
    _39(_43 k=0;k<p_len;k++) b[34+k]=((_184*)p_data)[k];
    net_raw(b, 34+p_len);
}
_50 send_big_cosmos_block(_89 ip, _184* block) {
    _184 pl[512];
    UDPHeader* u = (UDPHeader*)pl;
    u->src = hs(COSMOS_PORT);
    u->dst = hs(COSMOS_PORT);
    u->len = hs(8 + BIG_BLOCK_SIZE);
    u->chk = 0;
    _39(_43 k=0; k < BIG_BLOCK_SIZE; k++) {
        pl[8 + k] = block[k];
    }
    net_ip(ip, pl, 8 + BIG_BLOCK_SIZE, 17);
    str_cpy(cmd_status, "BIG BLOCK DISPATCHED");
}
_50 send_cosmos_block(_89 ip, _184* block) {
    _184 pl[512]; 
    UDPHeader* u = (UDPHeader*)pl;
    u->src = hs(COSMOS_PORT);
    u->dst = hs(COSMOS_PORT);
    u->len = hs(8 + BLOCK_SIZE);
    u->chk = 0;
    _39(_43 k=0; k < BLOCK_SIZE; k++) {
        pl[8 + k] = block[k];
    }
    net_ip(ip, pl, 8 + BLOCK_SIZE, 17);
    str_cpy(cmd_status, "COSMOS BLOCK SENT");
}
_50 net_handle_cosmos_packet(_184* data, _182 len) {
    _39(_43 k=0; k < len - 255; k++) {
        _15(data[k] EQ 0x2A AND data[k+255] EQ 0xFF) {
            _184* big_block = &data[k];
            _15(cb_validate(big_block)) {
                str_cpy(cmd_status, "BIG BLOCK RX: VALID");
                tba_master_stream(big_block);
                _37;
            }
        }
    }
}
_50 send_arp_ping() {
    _184 frame[60]; 
    _39(_43 i = 0; i < 60; i++) frame[i] = 0;
    /// 1. Ethernet Header (Ziel: Broadcast)
    _39(_43 i = 0; i < 6; i++) frame[i] = 0xFF; 
    _39(_43 i = 0; i < 6; i++) frame[6+i] = mac_addr[i]; 
    frame[12] = 0x08; frame[13] = 0x06; /// ARP
    /// 2. ARP Header
    frame[14] = 0x00; frame[15] = 0x01; /// Ethernet
    frame[16] = 0x08; frame[17] = 0x00; /// IPv4
    frame[18] = 0x06; frame[19] = 0x04; /// HW len=6, Proto len=4
    frame[20] = 0x00; frame[21] = 0x01; /// Opcode: 1 (Request)
    /// 3. Sender MAC & Sender IP (Wir sind die 192.168.14.99)
    _39(_43 i = 0; i < 6; i++) frame[22+i] = mac_addr[i]; 
    frame[28] = 192; frame[29] = 168; frame[30] = 14; frame[31] = 99;
    /// 4. Target MAC (Null) & Target IP (Deine FritzBox: 192.168.14.14)
    _39(_43 i = 0; i < 6; i++) frame[32+i] = 0x00; 
    frame[38] = 192; frame[39] = 168; frame[40] = 14; frame[41] = 14;
    /// Ab dafür!
    net_raw(frame, 60);
}
_50 send_udp(_89 ip, _182 p_src, _182 p_dst, _71 _30* msg) { 
    _43 ml=str_len(msg); _184 pl[1024]; UDPHeader* u=(UDPHeader*)pl;
    u->src=hs(p_src); u->dst=hs(p_dst); u->len=hs(8+2+ml); u->chk=0;
    pl[8]='S'; pl[9]=84; 
    _39(_43 k=0;k<ml;k++) pl[10+k]=msg[k]; 
    net_ip(ip, pl, 8+2+ml, 17);
    str_cpy(cmd_status, "UDP: SIGNED (ID 84)");
}
_50 send_udp_raw(_89 ip, _182 p_src, _182 p_dst, _184* payload, _182 payload_len) { 
    _184 pl[1500];
    UDPHeader* u = (UDPHeader*)pl;
    u->src = hs(p_src); 
    u->dst = hs(p_dst); 
    u->len = hs(8 + payload_len);
    /// HIER IST DEIN CHECKSUMMEN-HACK (Perfekt gelöst!)
    u->chk = 0;
    _39(_43 k = 0; k < payload_len; k++) {
        pl[8 + k] = payload[k]; 
    }
    net_ip(ip, pl, 8 + payload_len, 17);
}
_50 send_tcp_syn(_89 ip, _182 port) {
    _184 pl[64]; TCPHeader* t=(TCPHeader*)pl;
    t->src=hs(49152); t->dst=hs(port); t->seq=hl(random()); t->ack=0;
    t->off=0x50; t->flg=0x02; t->win=hs(8192); t->chk=0; t->urg=0;
    net_ip(ip, pl, 20, 6);
    str_cpy(cmd_status, "TCP: SYN SENT");
}
_50 send_tcp_ack(_89 ip, _182 p_src, _182 p_dst, _89 seq, _89 ack_num) {
    _184 pl[64]; TCPHeader* t = (TCPHeader*)pl;
    t->src = hs(p_src); 
    t->dst = hs(p_dst); 
    t->seq = hl(seq); 
    t->ack = hl(ack_num);
    t->off = 0x50; 
    t->flg = 0x10;
    t->win = hs(8192); 
    t->chk = 0; 
    t->urg = 0;
    net_ip(ip, pl, 20, 6);
    str_cpy(cmd_status, "TCP: CONNECTED (ACK)");
}
_50 rtl_enable_rx() {
    outl(rtl_io_base + 0x30, (_89)(uintptr_t)rx_buffer_rtl);
    outw(rtl_io_base + 0x3C, 0x0005); 
    outl(rtl_io_base + 0x44, 0x0F | 0x80);
    outb(rtl_io_base + 0x37, 0x0C); 
}
/// =======================================================
/// DIE NETZWERK-FUNKTIONEN
/// =======================================================
_50 e1000_enable_rx() {
    _15(intel_mem_base EQ 0) _96;
    _39(_43 i=0; i<32; i++) {
        rx_desc_intel[i*4 + 0] = 0x3000000 + (i * 2048); /// NEU: 0x3000000
        rx_desc_intel[i*4 + 1] = 0;                     
        rx_desc_intel[i*4 + 2] = 0;                     
        rx_desc_intel[i*4 + 3] = 0;                     
    }
    mmio_write32(intel_mem_base + 0x2800, 0x3800000); /// NEU: 0x3800000 
    mmio_write32(intel_mem_base + 0x2804, 0);        
    mmio_write32(intel_mem_base + 0x2808, 512);      
    mmio_write32(intel_mem_base + 0x2810, 0);        
    mmio_write32(intel_mem_base + 0x2818, 31);
    /// --- BARE METAL FIX: RX QUEUE ENABLE ---
    /// Moderne I219-V / 82579LM verlangen ZWINGEND, dass die Queue separat startet!
    mmio_write32(intel_mem_base + 0x2828, 0x02000000); /// Bit 25 (Enable) setzen
    _89 timeout = 0;
    _114((mmio_read32(intel_mem_base + 0x2828) & 0x02000000) EQ 0 AND timeout < 100000) { timeout++; }
    _39(_43 i = 0; i < 128; i++) {
        mmio_write32(intel_mem_base + 0x5200 + (i * 4), 0);
    }
    /// 0x0400801E = SECRC | BAM | MPE | UPE | SBP | EN
    mmio_write32(intel_mem_base + 0x0100, 0x0400801E);
}
_50 e1000_enable_tx() {
    _15(intel_mem_base EQ 0) _96;

    /// 1. Den Ring-Puffer auf feste 64 MB (0x4000000) legen
    _39(_43 i = 0; i < 32; i++) {
        _192 _89* desc_ptr = (_192 _89*)(uintptr_t)(0x4800000 + (i * 16));
        desc_ptr[0] = 0x4000000 + (i * 2048); 
        desc_ptr[1] = 0;                     
        desc_ptr[2] = 0;                     
        desc_ptr[3] = 0;                     
    }

    /// 2. Hardware sagen, wo der Puffer liegt
    mmio_write32(intel_mem_base + 0x3800, 0x4800000); 
    mmio_write32(intel_mem_base + 0x3804, 0);         
    mmio_write32(intel_mem_base + 0x3808, 512);      

    /// 3. Head und Tail auf 0 setzen
    mmio_write32(intel_mem_base + 0x3810, 0);         
    mmio_write32(intel_mem_base + 0x3818, 0);
    tx_cur_intel = 0;

    /// 4. Senden aktivieren (Bit 1 = EN, Bit 3 = PSP) -> 0x0A
    /// Bit 15, 22, 24 weglassen, da sie moderne PHYs verwirren können
    mmio_write32(intel_mem_base + 0x0400, 0x0000000A); 
    mmio_write32(intel_mem_base + 0x0410, 0x0060200A);
}
_50 e1000_check_rx() {
    _15(intel_mem_base EQ 0) _96;
    /// BARE METAL FIX 3: Cache hart leeren! (Write-Back and Invalidate Cache)
    /// Zwingt die CPU, die echten RAM-Daten der Hardware zu lesen.
    __asm__ _192("wbinvd" ::: "memory");
    _43 limit = 8; 
    _44 processed_any = _86;
    _89 last_processed = rx_cur_intel;
    /// SCHLEIFE START
    _114((rx_desc_intel[rx_cur_intel*4 + 3] & 1) EQ 1) {
        _182 len = rx_desc_intel[rx_cur_intel*4 + 2] & 0xFFFF;
        _184* raw_data = (_184*)(uintptr_t)(0x3000000 + (rx_cur_intel * 2048));
        /// --- VLAN-FILTER ---
        _43 off = 0;
        _15(raw_data[12] EQ 0x81 AND raw_data[13] EQ 0x00) off = 4;
        _44 packet_was_important = _86;
        /// --- 1. WEICHE: ARP ---
        _15(len >= 42+off AND raw_data[12+off] EQ 0x08 AND raw_data[13+off] EQ 0x06) {
            _15(raw_data[21+off] EQ 0x02) { 
                str_cpy(cmd_status, "ONLINE (ARP OK)!");
                _30* ip_ptr = ip_address;
                int_to_str(raw_data[28+off], ip_ptr); _114(*ip_ptr) ip_ptr++; *ip_ptr++ = '.';
                int_to_str(raw_data[29+off], ip_ptr); _114(*ip_ptr) ip_ptr++; *ip_ptr++ = '.';
                int_to_str(raw_data[30+off], ip_ptr); _114(*ip_ptr) ip_ptr++; *ip_ptr++ = '.';
                int_to_str(raw_data[31+off], ip_ptr); _114(*ip_ptr) ip_ptr++; *ip_ptr = 0;
                packet_was_important = _128;
            }
        }
        /// --- 2. WEICHE: DHCP (THE ULTIMATE PARSER) ---
        _41 _15(len > 280+off AND raw_data[12+off] EQ 0x08 AND raw_data[13+off] EQ 0x00 AND raw_data[23+off] EQ 0x11) {
            /// Check: UDP Ziel-Port 68 (0x0044) und BOOTP Opcode 2 (Reply)
            _15(raw_data[36+off] EQ 0x00 AND raw_data[37+off] EQ 68 AND raw_data[42+off] EQ 0x02) {
                /// Das "Magic Cookie": Der absolut sichere Beweis, dass es DHCP ist
                _15(raw_data[278+off] EQ 0x63 AND raw_data[279+off] EQ 0x82 AND raw_data[280+off] EQ 0x53 AND raw_data[281+off] EQ 0x63) {
                    /// 1. IP-ADRESSE (yiaddr = Your IP) ab Byte 58
                    _30* ip_ptr = ip_address;
                    int_to_str(raw_data[58+off], ip_ptr); _114(*ip_ptr) ip_ptr++; *ip_ptr++ = '.';
                    int_to_str(raw_data[59+off], ip_ptr); _114(*ip_ptr) ip_ptr++; *ip_ptr++ = '.';
                    int_to_str(raw_data[60+off], ip_ptr); _114(*ip_ptr) ip_ptr++; *ip_ptr++ = '.';
                    int_to_str(raw_data[61+off], ip_ptr); _114(*ip_ptr) ip_ptr++; *ip_ptr = 0;
                    /// 2. DHCP-OPTIONEN DYNAMISCH LESEN (Start ab Byte 282)
                    _43 i = 282 + off;
                    _43 msg_type = 0;
                    /// Wir lesen, bis das Paket zu Ende ist oder Option 255 (End) kommt
                    _114(i < len AND raw_data[i] NEQ 255) {
                        _184 opt = raw_data[i];
                        _15(opt EQ 0) { i++; _101; } /// Padding ignorieren
                        _184 opt_len = raw_data[i+1];
                        /// Option 53: DHCP Message Type (Offer = 2, ACK = 5)
                        _15(opt EQ 53) msg_type = raw_data[i+2];
                        /// Option 1: Subnetzmaske (Immer 4 Bytes lang)
                        _15(opt EQ 1 AND opt_len EQ 4) {
                            _30* m_ptr = net_mask;
                            int_to_str(raw_data[i+2], m_ptr); _114(*m_ptr) m_ptr++; *m_ptr++ = '.';
                            int_to_str(raw_data[i+3], m_ptr); _114(*m_ptr) m_ptr++; *m_ptr++ = '.';
                            int_to_str(raw_data[i+4], m_ptr); _114(*m_ptr) m_ptr++; *m_ptr++ = '.';
                            int_to_str(raw_data[i+5], m_ptr); _114(*m_ptr) m_ptr++; *m_ptr = 0;
                        }
                        /// Option 3: Router / Gateway
                        _15(opt EQ 3 AND opt_len >= 4) {
                            _30* g_ptr = gateway_ip;
                            int_to_str(raw_data[i+2], g_ptr); _114(*g_ptr) g_ptr++; *g_ptr++ = '.';
                            int_to_str(raw_data[i+3], g_ptr); _114(*g_ptr) g_ptr++; *g_ptr++ = '.';
                            int_to_str(raw_data[i+4], g_ptr); _114(*g_ptr) g_ptr++; *g_ptr++ = '.';
                            int_to_str(raw_data[i+5], g_ptr); _114(*g_ptr) g_ptr++; *g_ptr = 0;
                        }
                        i += 2 + opt_len; /// Zum nächsten Block springen
                    }
                    /// Den Status je nach Antwort anpassen
                    _15(msg_type EQ 2) str_cpy(cmd_status, "DHCP OFFER RX (IP GEFUNDEN!)");
                    _41 _15(msg_type EQ 5) str_cpy(cmd_status, "DHCP ACK RX (ONLINE!)");
                    _41 str_cpy(cmd_status, "DHCP RX: PROTOCOL OK");
                    packet_was_important = _128;
                }
            }
        }
        /// --- HEX-DEBUGGER MIT NOISE-FILTER ---
        _15(!packet_was_important AND !str_starts(cmd_status, "ONLINE")) {
            _44 is_noise = _86;
            _15(raw_data[12+off] EQ 0x08 AND raw_data[13+off] EQ 0x06 AND raw_data[21+off] EQ 0x01) is_noise = _128;
            _15(raw_data[12+off] EQ 0x89 AND raw_data[13+off] EQ 0x12) is_noise = _128;
            _15(raw_data[12+off] EQ 0x86 AND raw_data[13+off] EQ 0xDD) is_noise = _128;
            _15(raw_data[12+off] EQ 0x88 AND raw_data[13+off] EQ 0xE1) is_noise = _128;
            _15(!is_noise) {
                _30 hex_buf[40] = "RX: L="; _30* p = hex_buf + 6;
                int_to_str(len, p); _114(*p) p++;
                *p++ = ' '; *p++ = 'T'; *p++ = '=';
                byte_to_hex(raw_data[12+off], p); p+=2;
                byte_to_hex(raw_data[13+off], p); p+=2;
                *p++ = ' '; *p++ = 'O'; *p++ = '=';
                byte_to_hex(raw_data[21+off], p); p+=2; *p = 0;
                str_cpy(cmd_status, hex_buf);
            }
        }
        /// --- CLEANUP ---
        rx_desc_intel[rx_cur_intel*4 + 3] = 0; 
        last_processed = rx_cur_intel;
        rx_cur_intel = (rx_cur_intel + 1) % 32; 
    }
    _15(processed_any) {
        mmio_write32(intel_mem_base + 0x2818, last_processed);
    }
}
_50 check_incoming() {
    _15(intel_mem_base > 0) e1000_check_rx();
    _15(rtl_io_base > 0) {
        _30 cmd = inb(rtl_io_base + 0x37);
        _15((cmd & 1) EQ 0) {
            _89* hdr = (_89*)(rx_buffer_rtl + rx_idx_rtl);
            _89 rx_stat = hdr[0]; 
            _89 rx_len = (rx_stat >> 16) & 0xFFFF;
            _15(rx_len EQ 0 OR (rx_stat & 1) EQ 0) { outw(rtl_io_base + 0x38, rx_idx_rtl - 16); _96; }
            _30* raw_data = (_30*)(rx_buffer_rtl + rx_idx_rtl + 4); 
            _39(_43 k=0; k < rx_len - 10; k++) {
                _15(raw_data[k] EQ 'S' AND raw_data[k+1] EQ 84) {
                    str_cpy(cmd_status, "MSG (RTL) DETECTED!");
                    _30* msg_start = &raw_data[k+2];
                    _39(_43 m=0; m<30; m++) {
                        _30 c = msg_start[m];
                        _15(c >= 32 AND c <= 126) cmd_last_out[m] = c;
                        _41 { cmd_last_out[m] = 0; _37; } 
                    }
                    _37; 
                }
            }
            rx_idx_rtl = (rx_idx_rtl + rx_len + 4 + 3) & ~3; 
            _15(rx_idx_rtl > 8192) rx_idx_rtl = 0; 
            outw(rtl_io_base + 0x38, rx_idx_rtl - 16); 
        }
    }
}
_50 rtl8139_init(_89 io_addr) { 
    rtl_io_base = io_addr & ~3; outb(rtl_io_base + 0x52, 0); outb(rtl_io_base + 0x37, 0x10); 
    _114((inb(rtl_io_base + 0x37) & 0x10) NEQ 0) { } 
    outb(rtl_io_base + 0x37, 0x0C); 
    _30* p = mac_str; _39(_43 i=0; i<6; i++) { mac_addr[i] = inb(rtl_io_base + i); byte_to_hex(mac_addr[i], p); p+=2; _15(i<5) *p++ = ':'; } *p = 0; 
    rtl_enable_rx(); 
    str_cpy(cmd_status, "RTL8139 READY"); str_cpy(ip_address, "DHCP (RTL)..."); 
}
_50 send_dhcp_discover() {
    _184 dhcp[300]; 
    _39(_43 i=0; i<300; i++) dhcp[i] = 0;
    dhcp[0] = 1; dhcp[1] = 1; dhcp[2] = 6; dhcp[3] = 0;
    /// Transaktions-ID
    _89 xid = 0x12345678;
    dhcp[4] = (xid >> 24) & 0xFF; dhcp[5] = (xid >> 16) & 0xFF;
    dhcp[6] = (xid >> 8) & 0xFF; dhcp[7] = xid & 0xFF;
    dhcp[8] = 0; dhcp[9] = 0;
    dhcp[10] = 0x80; dhcp[11] = 0x00; /// BROADCAST-FLAG für den Router
    /// MAC eintragen
    _39(_43 i=0; i<6; i++) dhcp[28+i] = mac_addr[i];
    /// Magic Cookie (Pflicht für DHCP)
    dhcp[236] = 0x63; dhcp[237] = 0x82; dhcp[238] = 0x53; dhcp[239] = 0x63;
    /// DHCP Optionen
    _43 opt = 240;
    dhcp[opt++] = 53; dhcp[opt++] = 1; dhcp[opt++] = 1; /// Discover
    dhcp[opt++] = 61; dhcp[opt++] = 7; dhcp[opt++] = 1; /// Client ID
    _39(_43 i=0; i<6; i++) dhcp[opt++] = mac_addr[i];
    /// --- BARE METAL FIX: Dynamischer Hostname ---
    dhcp[opt++] = 12; dhcp[opt++] = 8; /// Länge jetzt 8!
    dhcp[opt++] = 'C'; dhcp[opt++] = 'O'; dhcp[opt++] = 'S'; 
    dhcp[opt++] = 'M'; dhcp[opt++] = 'O'; dhcp[opt++] = 'S';
    dhcp[opt++] = '-'; dhcp[opt++] = 'A' + (mac_addr[5] % 26); /// z.B. COSMOS-X
    dhcp[opt++] = 55; dhcp[opt++] = 3; /// Request List
    dhcp[opt++] = 1; dhcp[opt++] = 3; dhcp[opt++] = 6;
    dhcp[opt++] = 255; /// End-Tag
    /// Abschicken
    send_udp_raw(0xFFFFFFFF, 68, 67, dhcp, 300);
    /// --- BARE METAL FIX: Fehler nicht verdecken! ---
    /// Überschreibt den Status nur, wenn das Kabel wirklich verbunden ist.
    _15(!str_starts(cmd_status, "INTEL ERR")) {
        str_cpy(cmd_status, "DHCP DISCOVER FIRED!");
    }
}
/// =======================================================
/// DAS STETHOSKOP: LINK-STATUS PRÜFEN
/// =======================================================
_50 net_check_link() {
    _15(intel_mem_base EQ 0) {
        str_cpy(cmd_status, "ERR: INTEL NIC NOT INIT");
        _96;
    }
    /// Register 0x0008 ist das Device Status Register
    _89 status = mmio_read32(intel_mem_base + 0x0008);
    
    /// Bit 1 (0x02) steht für "Link Up"
    _15((status & 0x02) NEQ 0) {
        str_cpy(cmd_status, "LINK UP! CABLE DETECTED.");
    } _41 {
        str_cpy(cmd_status, "LINK DOWN! PHY ASLEEP / NO CABLE.");
    }
}

/// =======================================================
/// BARE METAL FIX: SANFTER START DER KARTE
/// =======================================================
_50 intel_e1000_init(_89 mmio_addr) {
    intel_mem_base = mmio_addr & 0xFFFFFFF0;
    _15(intel_mem_base EQ 0) _96;

    /// --- SCHRITT 1: SANFTER STOPP ---
    mmio_write32(intel_mem_base + 0x0100, 0); 
    mmio_write32(intel_mem_base + 0x0400, 0); 
    _39(_192 _89 delay = 0; delay < 1000000; delay++) { __asm__ _192("pause"); }

    /// Link Up (SLU) zur Sicherheit erzwingen
    _89 ctrl = mmio_read32(intel_mem_base + 0x0000);
    mmio_write32(intel_mem_base + 0x0000, ctrl | 0x00000060);

    /// Alte Interrupts löschen
    mmio_read32(intel_mem_base + 0x00C0);

    /// --- SCHRITT 2: MAC-ADRESSE ÜBERNEHMEN ---
    _89 ral = mmio_read32(intel_mem_base + 0x5400);
    _89 rah = mmio_read32(intel_mem_base + 0x5404);
    
    _15(ral NEQ 0 AND ral NEQ 0xFFFFFFFF) {
        mac_addr[0] = (_184)(ral); mac_addr[1] = (_184)(ral >> 8); 
        mac_addr[2] = (_184)(ral >> 16); mac_addr[3] = (_184)(ral >> 24); 
        mac_addr[4] = (_184)(rah); mac_addr[5] = (_184)(rah >> 8);
    } _41 {
        mac_addr[0] = 0x52; mac_addr[1] = 0x54; mac_addr[2] = 0x00;
        mac_addr[3] = 0x12; mac_addr[4] = 0x34; mac_addr[5] = 0x56;
    }

    _30* p = mac_str;
    _39(_43 i=0; i<6; i++) {
        byte_to_hex(mac_addr[i], p); p+=2;
        _15(i<5) *p++ = ':';
    }
    *p = 0;

    /// --- SCHRITT 3: KERNEL-QUEUES SCHARFSCHALTEN ---
    e1000_enable_rx();
    e1000_enable_tx();
    
    str_cpy(cmd_status, "INTEL: READY & LISTENING");
}