/* COSMOS OS v38.5 - SCHNEIDER v2.2 ULTIMATE FIX */
/* ARCHITECT: TOBIAS SCHNEIDER | TARGET: BARE METAL */

#include <stdint.h>
#include "schneider_lang.h"

// ==========================================
// 1. DATA STRUCTURES
// ==========================================
_202 PhysicalDrive { _44 present; _30 model[41]; _89 size_mb; _182 base_port; _44 is_slave; _43 type; } __attribute__((packed));
_202 Partition { _184 status; _184 type; _89 start_lba; _89 size; } __attribute__((packed));
_202 FileEntry { _30 name[32]; _89 sector_offset; _44 is_folder; _89 size; _30 date[16]; _44 exists; _184 parent_idx; _184 padding[5]; } __attribute__((packed));
_202 NICInfo { _30 name[32]; _89 address; _43 type; };
_202 Window { _43 id; _30 title[16]; _43 x, y, w, h; _44 open, minimized, fullscreen; _89 color; };
_202 Planet { _43 ang; _43 dist; _30 name[8]; _43 cur_x, cur_y; }; 
_202 Star { _43 x, y, z; }; 

// HOLYSPIRIT STRUCTURES
_202 SpiritTarget { _30 ip[20]; _43 hits; _44 critical; _43 x_off, y_off; };

// NET STACK
_202 EthernetFrame { _184 dest_mac[6]; _184 src_mac[6]; _182 type; } __attribute__((packed)); 
_202 IPHeader { _184 ver_ihl; _184 tos; _182 len; _182 id; _182 frag; _184 ttl; _184 proto; _182 chk; _89 src; _89 dst; } __attribute__((packed));  
_202 UDPHeader { _182 src; _182 dst; _182 len; _182 chk; } __attribute__((packed));
_202 TCPHeader { _182 src, dst; _89 seq, ack; _184 off, flg; _182 win, chk, urg; } __attribute__((packed));
_202 e1000_rx_desc { _94 addr; _182 length; _182 checksum; _184 status; _184 errors; _182 special; } __attribute__((packed));

// ==========================================
// 2. GLOBAL VARIABLES
// ==========================================
_72 _43 frame = 0; 
_43 screen_w = 800; _43 screen_h = 600; 
_89* fb; _89* bb = (_89*)0x2000000; _43 pitch;
_43 active_mode = 0, boot_frame = 0;
_43 galaxy_expansion = 0; _44 galaxy_open = _86;
_43 rtc_h, rtc_m, rtc_day, rtc_mon, rtc_year;
_30 user_name[] = "SCHNEIDER"; _30 ip_address[] = "SEARCHING..."; _30 cpu_brand[49];
_43 mouse_x=400, mouse_y=300; _184 mouse_cycle=0; int8_t mouse_byte[3]; 
_44 mouse_left=_86, mouse_just_pressed=_86; _184 last_key = 0; _44 key_new = _86; _44 key_shift = _86;

// WINDOWS
_43 win_z[11] = {0, 1, 2, 4, 3, 6, 5, 7, 8, 9, 10}; 
_43 drag_win = -1; _43 drag_off_x = 0; _43 drag_off_y = 0; _43 resize_win = -1; _44 z_blocked = _86;

_30 note_buf[10][41]; _43 note_r=0, note_c=0; _30 save_timestamp[32] = ""; 
_30 cmd_input[41] = {0}; _43 cmd_idx = 0; _30 cmd_last_out[128] = "READY."; _30 cmd_lines[5][41]; _30 cmd_status[32] = "OFFLINE"; 

// HOLYSPIRIT GLOBALS
SpiritTarget spirit_targets[10]; _43 spirit_count = 0;
_44 spirit_active = _86; _43 spirit_timer = 0;
_30 spirit_logs[6][40];

// HARDWARE
_44 touch_mode = _86; 
_44 webcam_active = _86; _44 logitech_found = _86; _30 webcam_model[40] = "NO WEBCAM";
_43 wifi_state = 0; _43 wifi_selected = -1; _30 wifi_pass[32] = {0}; _43 wifi_pass_idx = 0;
_30 wifi_ssids[4][20] = {"NO SIGNAL", "NO SIGNAL", "NO SIGNAL", "NO SIGNAL"}; 
_30 hw_disk[48] = "SCANNING..."; _30 hw_net[48] = "SCANNING..."; _30 hw_gpu[48] = "VGA/VESA"; _30 hw_usb[48] = "NO USB"; 
_44 usb_detected = _86; 

// FS & NAV
_43 save_step = 0; _43 save_part_sel = 0; _30 save_filename[32] = "blank.txt"; _43 save_name_idx = 9;
_30 new_folder_name[32] = "new_dir"; _43 folder_name_idx = 7;
PhysicalDrive drives[8]; _43 drive_count = 0; Partition partitions[4]; _184 sector0[512]; 
FileEntry file_table[8]; _43 current_open_file = -1; _43 drive_status = 0; _184 hdd_buf[512]; 
_184 current_path_id = 255; _43 current_folder_view_idx = -1; 
_43 dsk_view = 0; _43 dsk_selection = 0; _43 file_selection = -1; _43 active_drive_idx = -1; _43 input_cooldown = 0;

// DRIVERS
_89 rtl_io_base = 0; _89 intel_mem_base = 0; _89 usb_io_base = 0;
_184 mac_addr[6] = {0,0,0,0,0,0}; _30 mac_str[24] = "00:00:00:00:00:00";
NICInfo found_nics[5]; _43 nic_count = 0; _43 active_nic_idx = -1; _182 ata_base = 0x1F0; 
_184* tx_buffer = (_184*)0x500000; _43 tx_cur = 0; 
_184* rx_buffer_rtl = (_184*)0x600000; _43 rx_idx_rtl = 0;
e1000_rx_desc rx_desc_intel[32]; _184* rx_bufs_intel[32]; _43 rx_cur_intel = 0;

_43 sleep_mode_idx = 0; _43 sys_menu_selection = 0; _30 sleep_labels[6][16];
_43 shake_timer = 0, sx_off = 0, sy_off = 0; 
_43 sys_lang = 0; _43 sys_theme = 0; _43 genesis_tab = 0; 
Window windows[11]; Planet planets[6]; Star stars[200]; _43 senses_tab = 0;

// ==========================================
// 3. LOW LEVEL (ASM) & MATH
// ==========================================
_189 _184 inb(_182 p) { _184 r; _33 _192("inb %1, %0":"=a"(r):"Nd"(p)); _96 r; }
_189 _50 outb(_182 p, _184 v) { _33 _192("outb %0, %1"::"a"(v),"Nd"(p)); }
_189 _50 outw(_182 p, _182 v) { _33 _192("outw %0, %1"::"a"(v),"Nd"(p)); }
_189 _89 inl(_182 p) { _89 r; _33 _192("inl %1, %0":"=a"(r):"Nd"(p)); _96 r; }
_189 _50 outl(_182 p, _89 v) { _33 _192("outl %0, %1"::"a"(v),"Nd"(p)); }
_189 _50 insw(_182 p, _50* a, _89 c) { _33 _192("rep; insw" : "+D"(a), "+c"(c) : "d"(p) : "memory"); }
_189 _50 outsw(_182 p, _71 _50* a, _89 c) { _33 _192("rep; outsw" : "+S"(a), "+c"(c) : "d"(p) : "memory"); }
_89 mmio_read32(_89 addr) { _96 *(_192 _89*)(uintptr_t)addr; }
_50 mmio_write32(_89 addr, _89 val) { *(_192 _89*)(uintptr_t)addr = val; }

_71 _43 sin_lut[256] = { 1, 2, 4, 7, 9, 12, 14, 17, 19, 21, 24, 26, 28, 30, 33, 35, 37, 39, 41, 43, 45, 47, 49, 51, 53, 55, 56, 58, 60, 61, 63, 64, 66, 67, 68, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 79, 80, 81, 81, 82, 82, 83, 83, 83, 84, 84, 84, 84, 84, 84, 84, 83, 83, 83, 82, 82, 81, 81, 80, 79, 79, 78, 77, 76, 75, 74, 73, 72, 71, 70, 68, 67, 66, 64, 63, 61, 60, 58, 56, 55, 53, 51, 49, 47, 45, 43, 41, 39, 37, 35, 33, 30, 28, 26, 24, 21, 19, 17, 14, 12, 9, 7, 4, 2, 1, -1, -2, -4, -7, -9, -12, -14, -17, -19, -21, -24, -26, -28, -30, -33, -35, -37, -39, -41, -43, -45, -47, -49, -51, -53, -55, -56, -58, -60, -61, -63, -64, -66, -67, -68, -70, -71, -72, -73, -74, -75, -76, -77, -78, -79, -79, -80, -81, -81, -82, -82, -83, -83, -83, -84, -84, -84, -84, -84, -84, -84, -83, -83, -83, -82, -82, -81, -81, -80, -79, -79, -78, -77, -76, -75, -74, -73, -72, -71, -70, -68, -67, -66, -64, -63, -61, -60, -58, -56, -55, -53, -51, -49, -47, -45, -43, -41, -39, -37, -35, -33, -30, -28, -26, -24, -21, -19, -17, -14, -12, -9, -7, -4, -2, -1 };
_189 _43 Cos(_43 a) { _96 sin_lut[(a + 64) % 256]; }
_189 _43 Sin(_43 a) { _96 sin_lut[a % 256]; }
_72 _89 rng_seed = 123456789;
_89 random() { rng_seed = (rng_seed * 1103515245 + 12345) & 0x7FFFFFFF; _96 rng_seed; }
_50 str_cpy(_30* d, _71 _30* s) { _114(*s) *d++ = *s++; *d=0; }
_43 str_len(_71 _30* s) { _43 l=0; _114(*s++)l++; _96 l; }
_44 str_equal(_71 _30* s1, _71 _30* s2) { _114(*s1 AND (*s1 EQ *s2)) { s1++; s2++; } _96 *(unsigned _30*)s1 EQ *(unsigned _30*)s2; }
_44 str_starts(_71 _30* full, _71 _30* prefix) { _114(*prefix) { _15(*prefix++ NEQ *full++) _96 _86; } _96 _128; }
_50 hex_to_str(_182 num, _30* out) { _71 _30 h[] = "0123456789ABCDEF"; out[0] = h[(num>>12)&0xF]; out[1] = h[(num>>8)&0xF]; out[2] = h[(num>>4)&0xF]; out[3] = h[num&0xF]; out[4] = 0; }
_50 byte_to_hex(_184 b, _30* out) { _71 _30 h[]="0123456789ABCDEF"; out[0]=h[(b>>4)&0xF]; out[1]=h[b&0xF]; out[2]=0; }
_50 int_to_str(_43 n, _30* s) { _15(n EQ 0){s[0]='0';s[1]=0;_96;} _43 i=0; _15(n<0){s[i++]='-';n=-n;} _43 t=n; _114(t>0){t/=10;i++;} s[i]=0; _114(n>0){s[--i]=(n%10)+'0';n/=10;} }
_189 _184 bcd2bin(_184 b) { _96 ((b >> 4) * 10) + (b & 0xF); }

// ==========================================
// 4. NET/SECURITY HELPERS (HIERHER VERSCHOBEN!)
// ==========================================
_89 get_id(_71 _30* c) {
    _89 s=0; _114(*c) { _15(*c>='a' AND *c<='z') s+=(*c-'a'+1); _41 _15(*c>='A' AND *c<='Z') s+=(*c-'A'+1); c++; }
    _15(s EQ 57 OR s EQ 54 OR s EQ 56 OR s EQ 63 OR s EQ 101) _96 s*2; 
    _15(s EQ 57 AND *(c-1) EQ 'o') _96 171; 
    _96 s;
}
_189 _182 hs(_182 v) { _96 (v<<8)|(v>>8); } 
_189 _89 hl(_89 v) { _96 ((v&0xFF)<<24)|((v&0xFF00)<<8)|((v&0xFF0000)>>8)|((v>>24)&0xFF); } 
_182 chk(_50* d, _43 l) { _89 s=0; _182* p=(_182*)d; _114(l>1){s+=*p++;l-=2;} _15(l)s+=*(_184*)p; _114(s>>16)s=(s&0xFFFF)+(s>>16); _96 (_182)(~s); }

// ==========================================
// 5. GRAPHICS ENGINE
// ==========================================
_30 get_ascii(_184 sc) {
    _15(sc > 58) _96 0;
    _72 _30 k_low[] = {0,27,'1','2','3','4','5','6','7','8','9','0','-','=','\b','\t','q','w','e','r','t','y','u','i','o','p','[',']','\n',0,'a','s','d','f','g','h','j','k','l',';','\'','`',0,'\\','z','x','c','v','b','n','m',',','.','/',0,'*',0,' '};
    _72 _30 k_up[]  = {0,27,'!','@','#','$','%','^','&','*','(',')','_','+','\b','\t','Q','W','E','R','T','Y','U','I','O','P','{','}','\n',0,'A','S','D','F','G','H','J','K','L',':','"','~',0,'|','Z','X','C','V','B','N','M','<','>','?',0,'*',0,' '};
    _96 key_shift ? k_up[sc] : k_low[sc];
}
_44 is_over_rect(_43 mx, _43 my, _43 x, _43 y, _43 w, _43 h) { _43 m = touch_mode ? 30 : 0; _96 (mx >= x-m AND mx <= x+w+m AND my >= y-m AND my <= y+h+m); }
_44 is_over(_43 mx, _43 my, _43 ox, _43 oy, _43 r) { _43 m = touch_mode ? 20 : 0; _96 (mx-ox)*(mx-ox) + (my-oy)*(my-oy) < (r+m)*(r+m); }

_50 Put(_43 x, _43 y, _89 c) { _15(x<0 OR x>=800 OR y<0 OR y>=600) _96; bb[y*800+x]=c; }
_50 PutAlpha(_43 x, _43 y, _89 c) { _15(x<0 OR x>=800 OR y<0 OR y>=600) _96; _89 bg = bb[y*800+x]; _89 s1 = ((c & 0xFEFEFE) >> 1) + ((bg & 0xFEFEFE) >> 1); bb[y*800+x] = ((s1 & 0xFEFEFE) >> 1) + ((bg & 0xFEFEFE) >> 1); }
_50 Clear() { _39(_43 i=0; i<800*600; i++) bb[i] = 0; }
_50 Swap() { _15(shake_timer > 0) { sx_off = (random()%10)-5; sy_off = (random()%10)-5; shake_timer--; } _41 { sx_off=0; sy_off=0; } _39(_43 y=0; y<600; y++) { _43 ty = y + sy_off; _15(ty < 0 OR ty >= 600) _101; _89* d=(_89*)((_184*)fb+ty*pitch); _89* s=&bb[y*800]; _39(_43 x=0; x<800; x++) { _43 tx = x + sx_off; _15(tx >= 0 AND tx < 800) d[tx] = s[x]; } } }

_50 DrawRoundWindow(_43 cx_p, _43 cy_p, _43 r, _89 color) { _43 r2=r*r; _39(_43 y=-r;y<=r;y++) _39(_43 x=-r;x<=r;x++) _15(x*x+y*y<=r2) Put(cx_p+x, cy_p+y, color); }
_50 DrawIcon(_43 x, _43 y, _89 col) { _39(_43 i=0; i<30; i++) _39(_43 j=0; j<40; j++) Put(x+i, y+j, col); _39(_43 i=5; i<25; i++) _39(_43 j=5; j<15; j++) Put(x+i, y+j, 0xFFFFFF); }
_50 DrawCentrifugalVortex(_43 v_cx, _43 v_cy, _43 exp) { _15 (exp <= 5) _96; _43 rot = (256 - (frame * 85) % 256) % 256; _89 dust_col = 0x222222; _43 max_sync_dist = (280 * exp) / 320; _39 (_43 r = 0; r < 9; r++) { _43 dist = ( (40 + r * 35) * exp ) / 320; _15(dist > max_sync_dist) _101; _39 (_43 i = 0; i < 256; i += 4) { _43 ang = (i + rot) % 256; Put(v_cx + (Cos(ang)*dist)/84, v_cy + (Sin(ang)*dist*3/4)/84, dust_col + (random()%0x353535)); } } _43 axes[] = {0, 64, 128, 192}; _39(_43 a=0; a<4; a++) { _43 axis_ang = (axes[a] + rot) % 256; _39(_43 d=10; d < max_sync_dist; d+=3) { PutAlpha(v_cx + (Cos(axis_ang)*d*4)/336, v_cy + (Sin(axis_ang)*d*3)/336, 0x707070 + (random()%0x505050)); } } }

_50 DrawChar(_43 x, _43 y, _30 c, _89 col, _44 bold) { 
    _72 _71 _184 f_u[] = { 0x7E,0x11,0x11,0x11,0x7E, 0x7F,0x49,0x49,0x49,0x36, 0x3E,0x41,0x41,0x41,0x22, 0x7F,0x41,0x41,0x22,124, 0x7F,0x49,0x49,0x49,0x41, 0x7F,0x09,0x09,0x09,0x01, 0x3E,0x41,0x49,0x49,0x7A, 0x7F,0x08,0x08,0x08,0x7F, 0x00,0x41,0x7F,0x41,0x00, 0x20,0x40,0x41,0x3F,0x01, 0x7F,0x08,0x14,0x22,0x41, 0x7F,0x40,0x40,0x40,0x40, 0x7F,0x02,0x0C,0x02,0x7F, 0x7F,0x04,0x08,0x10,0x7F, 0x3E,0x41,0x41,0x41,0x3E, 0x7F,0x09,0x09,0x09,0x06, 0x3E,0x41,0x51,0x21,0x5E, 0x7F,0x09,0x19,0x29,0x46, 0x46,0x49,0x49,0x49,0x31, 0x01,0x01,0x7F,0x01,0x01, 0x3F,0x40,0x40,0x40,0x3F, 0x1F,0x20,0x40,0x20,0x1F, 0x3F,0x40,0x38,0x40,0x3F, 0x63,0x14,0x08,0x14,0x63, 0x07,0x08,0x70,0x08,0x07, 0x61,0x51,0x49,0x45,0x43 };
    _72 _71 _184 f_l[] = { 0x20,0x54,0x54,0x54,0x78, 0x7F,0x48,0x44,0x44,0x38, 0x38,0x44,0x44,0x44,0x20, 0x38,0x44,0x44,0x48,0x7F, 0x38,0x54,0x54,0x54,0x18, 0x08,0x7E,0x09,0x01,0x02, 0x0C,0x52,0x52,0x52,0x3E, 0x7F,0x08,0x04,0x04,0x78, 0x00,0x44,0x7D,0x40,0x00, 0x20,0x40,0x44,0x3D,0x00, 0x7F,0x10,0x28,0x44,0x00, 0x00,0x41,0x7F,0x40,0x00, 0x7C,0x04,0x18,0x04,0x78, 0x7C,0x08,0x04,0x04,0x78, 0x38,0x44,0x44,0x44,0x38, 0x7C,0x14,0x14,0x14,0x08, 0x08,0x14,0x14,0x18,0x7C, 0x7C,0x08,0x04,0x04,0x08, 0x48,0x54,0x54,0x54,0x20, 0x04,0x3F,0x44,0x40,0x20, 0x3C,0x40,0x40,0x20,0x7C, 0x1C,0x20,0x40,0x20,0x1C, 0x3C,0x40,0x30,0x40,0x3C, 0x44,0x28,0x10,0x28,0x44, 0x0C,0x50,0x50,0x50,0x3C, 0x44,0x64,0x54,0x4C,0x44 };
    _72 _71 _184 f_n[] = { 0x3E,0x51,0x49,0x45,0x3E, 0x00,0x42,0x7F,0x40,0x00, 0x42,0x61,0x51,0x49,0x46, 0x21,0x41,0x45,0x4B,0x31, 0x18,0x14,0x12,0x7F,0x10, 0x27,0x45,0x45,0x45,0x39, 0x3C,0x4A,0x49,0x49,0x30, 0x01,0x71,0x09,0x05,0x03, 0x36,0x49,0x49,0x49,0x36, 0x06,0x49,0x49,0x29,0x1E };
    _71 _184* ptr = 0;
    _15(c >= 'A' AND c <= 'Z') ptr = &f_u[(c-'A')*5]; _41 _15(c >= 'a' AND c <= 'z') ptr = &f_l[(c-'a')*5]; _41 _15(c >= '0' AND c <= '9') ptr = &f_n[(c-'0')*5];
    _41 _15(c EQ ':') { _72 _184 s[]={0,0x36,0x36,0,0}; ptr=s; } _41 _15(c EQ '.') { _72 _184 s[]={0,0x60,0x60,0,0}; ptr=s; } _41 _15(c EQ '-') { _72 _184 s[]={0x08,0x08,0x08,0x08,0x08}; ptr=s; } _41 _15(c EQ '!') { _72 _184 s[]={0,0,0x7D,0,0}; ptr=s; } _41 _15(c EQ '_') { _72 _184 s[]={0x40,0x40,0x40,0x40,0x40}; ptr=s; } _41 _15(c EQ '(') { _72 _184 s[]={0x1C,0x22,0x41,0,0}; ptr=s; } _41 _15(c EQ ')') { _72 _184 s[]={0,0,0x41,0x22,0x1C}; ptr=s; } _41 _15(c EQ '/') { _72 _184 s[]={0x20,0x10,0x08,0x04,0x02}; ptr=s; } _41 _15(c EQ '=') { _72 _184 s[]={0x14,0x14,0x14,0x14,0x14}; ptr=s; } _41 _15(c EQ '?') { _72 _184 s[]={0x20,0x40,0x45,0x48,0x30}; ptr=s; } _41 _15(c EQ '"') { _72 _184 s[]={0,0x03,0,0x03,0}; ptr=s; } _41 _15(c EQ ',') { _72 _184 s[]={0,0x50,0x30,0,0}; ptr=s; } _41 _15(c EQ '|') { _72 _184 s[]={0x7F,0,0,0,0}; ptr=s; }
    _15(!ptr) _96; _39(_43 m=0;m<5;m++){ _184 l=ptr[m]; _39(_43 n=0;n<7;n++) _15((l>>n)&1) { Put(x+m,y+n,col); _15(bold) Put(x+m+1,y+n,col); } } 
}

_50 Text(_43 x, _43 y, _71 _30* s, _89 col, _44 bold) { _15(!s) _96; _114(*s) { DrawChar(x,y,*s++,col,bold); x+=(bold?7:6); } }
_50 TextC(_43 cp, _43 y, _71 _30* s, _89 col, _44 bold) { _15(!s) _96; _43 l=0; _114(s[l])l++; Text(cp-(l*(bold?7:6))/2, y, s, col, bold); }
_50 DrawRoundedRect(_43 x, _43 y, _43 rw, _43 rh, _43 r, _89 c) { _39(_43 iy=0;iy<rh;iy++)_39(_43 ix=0;ix<rw;ix++){ _44 corn=_86; _15(ix<r AND iy<r AND (r-ix)*(r-ix)+(r-iy)*(r-iy)>r*r) corn=_128; _15(ix>rw-r AND iy<r AND (ix-(rw-r))*(ix-(rw-r))+(r-iy)*(r-iy)>r*r) corn=_128; _15(ix<r AND iy>rh-r AND (r-ix)*(r-ix)+(iy-(rh-r))*(iy-(rh-r))>r*r) corn=_128; _15(ix>rw-r AND iy>rh-r AND (ix-(rw-r))*(ix-(rw-r))+(iy-(rh-r))*(iy-(rh-r))>r*r) corn=_128; _15(!corn) Put(x+ix,y+iy,c); } }
_50 DrawTriangle(_43 x1, _43 y1, _43 x2, _43 y2, _43 x3, _43 y3, _89 c) { _43 minx=x1, maxx=x1, miny=y1, maxy=y1; _15(x2<minx)minx=x2; _15(x2>maxx)maxx=x2; _15(y2<miny)miny=y2; _15(y2>maxy)maxy=y2; _15(x3<minx)minx=x3; _15(x3>maxx)maxx=x3; _15(y3<miny)miny=y3; _15(y3>maxy)maxy=y3; _39(_43 y=miny; y<=maxy; y++) { _39(_43 x=minx; x<=maxx; x++) { _43 w1 = (x2-x1)*(y-y1) - (y2-y1)*(x-x1); _43 w2 = (x3-x2)*(y-y2) - (y3-y2)*(x-x2); _43 w3 = (x1-x3)*(y-y3) - (y1-y3)*(x-x3); _15((w1>=0 AND w2>=0 AND w3>=0) OR (w1<=0 AND w2<=0 AND w3<=0)) Put(x,y,c); } } }
_50 DrawFace(_43 x, _43 y, _43 mx, _43 my) { DrawRoundWindow(x, y, 60, 0x555555); _43 ex1=x-20; _43 ey=y-10; _43 ex2=x+20; _43 ox=(mx-x)/20; _15(ox>5)ox=5; _15(ox<-5)ox=-5; _43 oy=(my-y)/20; _15(oy>5)oy=5; _15(oy<-5)oy=-5; DrawRoundedRect(ex1-8, ey-5, 16, 10, 2, 0xFFFFFF); DrawRoundedRect(ex2-8, ey-5, 16, 10, 2, 0xFFFFFF); Put(ex1+ox, ey+oy, 0); Put(ex1+ox+1, ey+oy, 0); Put(ex2+ox, ey+oy, 0); Put(ex2+ox+1, ey+oy, 0); DrawRoundedRect(x-15, y+25, 30, 2, 0, 0); }
_50 DrawWaveform(_43 x, _43 y, _43 w, _43 f) { _39(_43 i=0; i<w; i+=2) { _43 h = Sin(i*5 + f*10) / 4; _15(random()%10 > 7) h += (random()%10)-5; Put(x+i, y+20+h, 0); } }

// BOOT LOGO
_50 boot_logo() {
    _89 g=0xFFD700; _43 c=400, y=200;
    TextC(c,y,"#####################################",g,_128);
    TextC(c,y+20," SCHNEIDER SYSTEM v38.5 (ULTIMATE) ",0xFFFFFF,_128);
    TextC(c,y+40," ID-HASHING: ACTIVE | NET: RX/TX ",0xAAAAAA,_86);
    TextC(c,y+80," >> SYSTEM READY << ",g,_128);
    TextC(c,y+100,"#####################################",g,_128);
}

// HOLYSPIRIT RENDERER
_50 draw_spirit_radar(_43 wx, _43 wy, _43 ww, _43 wh) {
    DrawRoundedRect(wx+10, wy+40, ww-20, wh-60, 5, 0x050010);
    _43 cx = wx + ww/2; _43 cy = wy + 40 + (wh-60)/2;
    _39(_43 r=20; r<120; r+=30) _39(_43 a=0; a<256; a+=16) Put(cx + (Cos(a)*r)/84, cy + (Sin(a)*r*3/4)/84, 0x330033);
    _43 ang = (frame*4)%256;
    _39(_43 r=0; r<100; r+=2) Put(cx + (Cos(ang)*r)/84, cy + (Sin(ang)*r*3/4)/84, 0x00FF00);
    _39(_43 i=0; i<spirit_count; i++) {
        _43 tx = cx + spirit_targets[i].x_off; _43 ty = cy + spirit_targets[i].y_off;
        DrawRoundWindow(tx, ty, 3, spirit_targets[i].critical ? 0xFF0000 : 0x00FFFF);
        _15(is_over(mouse_x, mouse_y, tx, ty, 5)) Text(tx+5, ty, spirit_targets[i].ip, 0xFFFFFF, _128);
    }
    _39(_43 i=0; i<6; i++) Text(wx+20, wy+wh-30-(i*12), spirit_logs[i], 0x00FF00, _86);
}

// ==========================================
// 6. HARDWARE DRIVERS
// ==========================================
_50 play_freq(_89 f) { _15(f EQ 0) { outb(0x61, inb(0x61) & 0xFC); _96; } _89 d = 1193180 / f; outb(0x43, 0xB6); outb(0x42, (_184)d); outb(0x42, (_184)(d >> 8)); _184 t = inb(0x61); _15 (t NEQ (t | 3)) outb(0x61, t | 3); }
_50 play_sound(_89 n_freq, _43 duration) { _15(n_freq EQ 0) _96; _89 div = 1193180 / n_freq; outb(0x43, 0xB6); outb(0x42, (_184)(div)); outb(0x42, (_184)(div >> 8)); _184 tmp = inb(0x61); _15(tmp NEQ (tmp | 3)) outb(0x61, tmp | 3); _39(_43 i=0; i<duration*10000; i++) _33 _192("nop"); outb(0x61, tmp & 0xFC); }
_50 startup_melody() { play_sound(523, 100); play_sound(659, 100); play_sound(784, 200); }
_50 play_earthquake(_43 duration) { shake_timer = duration; _114(shake_timer > 0) { play_freq(40 + (random() % 60)); Swap(); _39(_192 _43 d=0; d<40000; d++); } play_freq(0); }
_50 system_reboot() { play_earthquake(30); _114(inb(0x64)&2); outb(0x64,0xFE); }
_50 system_shutdown() { play_earthquake(30); outw(0x604, 0x2000); outw(0xB004, 0x2000); outw(0x4004, 0x3400); _114(1) _33 _192("cli; hlt"); }
_50 read_rtc() { outb(0x70, 4); rtc_h = bcd2bin(inb(0x71)); outb(0x70, 2); rtc_m = bcd2bin(inb(0x71)); outb(0x70, 7); rtc_day = bcd2bin(inb(0x71)); outb(0x70, 8); rtc_mon = bcd2bin(inb(0x71)); outb(0x70, 9); rtc_year = bcd2bin(inb(0x71)); rtc_h = (rtc_h + 1) % 24; }
_50 get_cpu_brand() { _89 a,b,c,d; _33 _192("cpuid":"=a"(a):"a"(0x80000000)); _15(a<0x80000004){str_cpy(cpu_brand,"GENERIC");_96;} _30* s=cpu_brand; _39(_89 i=0x80000002;i<=0x80000004;i++){_33 _192("cpuid":"=a"(a),"=b"(b),"=c"(c),"=d"(d):"a"(i));*(_89*)s=a;s+=4;*(_89*)s=b;s+=4;*(_89*)s=c;s+=4;*(_89*)s=d;s+=4;} cpu_brand[48]=0; }

_50 mouse_wait(_184 type) { _89 t = 100000; _114(t--) { _15(type EQ 0 AND (inb(0x64)&1)) _96; _15(type EQ 1 AND !(inb(0x64)&2)) _96; } }
_50 mouse_write(_184 w) { mouse_wait(1); outb(0x64, 0xD4); mouse_wait(1); outb(0x60, w); }
_184 mouse_read() { mouse_wait(0); _96 inb(0x60); }
_50 init_mouse() { mouse_wait(1); outb(0x64,0xA8); mouse_wait(1); outb(0x64,0x20); mouse_wait(0); _184 s=inb(0x60)|2; mouse_wait(1); outb(0x64,0x60); mouse_wait(1); outb(0x60,s); mouse_write(0xF6); mouse_read(); mouse_write(0xF4); mouse_read(); }
_50 handle_input() { 
    key_new = _86; mouse_just_pressed = _86; _184 st = inb(0x64); 
    _15(st & 1) { 
        _184 d = inb(0x60); 
        _15(st & 0x20) { 
            _15(mouse_cycle EQ 0 AND (d&8)) { mouse_byte[0]=d; mouse_cycle++; } _41 _15(mouse_cycle EQ 1) { mouse_byte[1]=d; mouse_cycle++; } _41 _15(mouse_cycle EQ 2) { mouse_byte[2]=d; mouse_cycle=0; mouse_x+=(int8_t)mouse_byte[1]; mouse_y-=(int8_t)mouse_byte[2]; _15(mouse_x<0)mouse_x=0; _15(mouse_x>799)mouse_x=799; _15(mouse_y<0)mouse_y=0; _15(mouse_y>599)mouse_y=599; _44 cur_left=(mouse_byte[0]&1); _15(cur_left AND !mouse_left) mouse_just_pressed=_128; mouse_left=cur_left; _15(active_mode EQ 1) active_mode = 2; } 
        } _41 { 
            _15(d EQ 0x2A OR d EQ 0x36) key_shift = _128; _41 _15(d EQ 0xAA OR d EQ 0xB6) key_shift = _86; _41 _15(!(d & 0x80)) { last_key = d; key_new = _128; } _15(active_mode EQ 1) active_mode = 2; 
        } 
    } 
}

_89 pci_read(_184 bus, _184 slot, _184 func, _184 offset) { _89 address = (_89)((bus << 16) | (slot << 11) | (func << 8) | (offset & 0xFC) | ((_89)0x80000000)); outl(0xCF8, address); _96 inl(0xCFC); }

// --- NETWORK STACK ---
_50 net_raw(_50* d, _89 l) {
    _15(!rtl_io_base && !intel_mem_base) _96;
    _15(intel_mem_base > 0) { _96; } 
    _15(rtl_io_base > 0) {
        _39(_89 i=0;i<l;i++) tx_buffer[i]=((_184*)d)[i];
        outl(rtl_io_base+0x20+(tx_cur*4),(_89)tx_buffer); outl(rtl_io_base+0x10+(tx_cur*4),l);
        tx_cur=(tx_cur+1)%4;
    }
}

_50 net_ip(_89 dst, _50* p_data, _182 p_len, _184 proto) {
    _184 b[1514]; EthernetFrame* e=(EthernetFrame*)b; IPHeader* i=(IPHeader*)(b+14);
    _39(_43 k=0;k<6;k++){e->dest_mac[k]=0xFF; e->src_mac[k]=mac_addr[k];} e->type=hs(0x0800);
    i->ver_ihl=0x45; i->len=hs(20+p_len); i->id=hs(random()); i->frag=hs(0x4000);
    i->ttl=64; i->proto=proto; i->src=hl(0x0A00020F); i->dst=hl(dst); i->chk=0; i->chk=chk(i,20);
    _39(_43 k=0;k<p_len;k++) b[34+k]=((_184*)p_data)[k];
    net_raw(b, 34+p_len);
}

_50 send_udp(_89 ip, _182 p_src, _182 p_dst, _71 _30* msg) { 
    _43 ml=str_len(msg); _184 pl[1024]; UDPHeader* u=(UDPHeader*)pl;
    u->src=hs(p_src); u->dst=hs(p_dst); u->len=hs(8+2+ml); u->chk=0;
    pl[8]='S'; pl[9]=84; 
    _39(_43 k=0;k<ml;k++) pl[10+k]=msg[k]; 
    net_ip(ip, pl, 8+2+ml, 17);
    str_cpy(cmd_status, "UDP: SIGNED (ID 84)");
}

_50 send_tcp_syn(_89 ip, _182 port) {
    _184 pl[64]; TCPHeader* t=(TCPHeader*)pl;
    t->src=hs(49152); t->dst=hs(port); t->seq=hl(random()); t->ack=0;
    t->off=0x50; t->flg=0x02; t->win=hs(8192); t->chk=0; t->urg=0;
    net_ip(ip, pl, 20, 6);
    str_cpy(cmd_status, "TCP: SYN SENT");
}

// [REALTEK RX]
_50 rtl_enable_rx() {
    outl(rtl_io_base + 0x30, (_89)rx_buffer_rtl);
    outw(rtl_io_base + 0x3C, 0x0005); 
    outl(rtl_io_base + 0x44, 0x0F | 0x80);
    outb(rtl_io_base + 0x37, 0x0C); 
}

// [INTEL RX]
_50 e1000_enable_rx() {
    _15(intel_mem_base EQ 0) _96;
    _39(_43 i=0; i<32; i++) {
        rx_bufs_intel[i] = (_184*)(0x800000 + (i * 2048)); 
        rx_desc_intel[i].addr = (_94)(uintptr_t)rx_bufs_intel[i];
        rx_desc_intel[i].status = 0;
    }
    mmio_write32(intel_mem_base + 0x2800, (_89)(uintptr_t)rx_desc_intel); 
    mmio_write32(intel_mem_base + 0x2804, 0); 
    mmio_write32(intel_mem_base + 0x2808, 512); 
    mmio_write32(intel_mem_base + 0x2810, 0); 
    mmio_write32(intel_mem_base + 0x2818, 31); 
    mmio_write32(intel_mem_base + 0x0100, 0x801A); 
}

_50 e1000_check_rx() {
    _15(intel_mem_base EQ 0) _96;
    _184 status = rx_desc_intel[rx_cur_intel].status;
    _15((status & 1)) {
        str_cpy(cmd_status, "INTEL: PACKET RX!");
        _184* raw_data = rx_bufs_intel[rx_cur_intel];
        _182 len = rx_desc_intel[rx_cur_intel].length;
        _39(_43 k=0; k < len - 10; k++) {
            _15(raw_data[k] EQ 'S' AND raw_data[k+1] EQ 84) {
                str_cpy(cmd_status, "MSG (INTEL) DETECTED!");
                _30* msg_start = (_30*)&raw_data[k+2];
                _39(_43 m=0; m<30; m++) {
                    _30 c = msg_start[m];
                    _15(c >= 32 AND c <= 126) cmd_last_out[m] = c;
                    _41 { cmd_last_out[m] = 0; _37; } 
                }
                _37;
            }
        }
        rx_desc_intel[rx_cur_intel].status = 0;
        _89 old_cur = rx_cur_intel;
        rx_cur_intel = (rx_cur_intel + 1) % 32;
        mmio_write32(intel_mem_base + 0x2818, old_cur); 
    }
}

// UNIVERSAL CHECKER
_50 check_incoming() {
    _15(found_nics[active_nic_idx].type EQ 2) e1000_check_rx(); 
    _41 _15(found_nics[active_nic_idx].type EQ 1) { 
        _15(rtl_io_base EQ 0) _96;
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
_50 intel_e1000_init(_89 mmio_addr) { 
    intel_mem_base = mmio_addr & 0xFFFFFFF0; 
    _15(intel_mem_base > 0) { 
        _89 ral = mmio_read32(intel_mem_base + 0x5400); 
        _89 rah = mmio_read32(intel_mem_base + 0x5404); 
        mac_addr[0] = (_184)(ral); mac_addr[1] = (_184)(ral >> 8); 
        mac_addr[2] = (_184)(ral >> 16); mac_addr[3] = (_184)(ral >> 24); 
        mac_addr[4] = (_184)(rah); mac_addr[5] = (_184)(rah >> 8); 
        _30* p = mac_str; _39(_43 i=0; i<6; i++) { byte_to_hex(mac_addr[i], p); p+=2; _15(i<5) *p++ = ':'; } *p = 0; 
        
        e1000_enable_rx(); 
        
        str_cpy(cmd_status, "INTEL NIC ACTIVE (RX ON)"); 
        str_cpy(ip_address, "DHCP (INTEL)..."); 
    } 
}
_50 uhci_init(_89 io_addr) { usb_io_base = io_addr & ~3; outw(usb_io_base, 4); _39(_43 i=0;i<10000;i++); outw(usb_io_base, 0); outw(usb_io_base, 1); _15(inb(usb_io_base + 2) & 0) str_cpy(cmd_status, "USB INIT FAIL"); _41 { str_cpy(cmd_status, "USB HOST RUNNING"); str_cpy(ip_address, "USB SCANNING..."); str_cpy(wifi_ssids[0], "FritzBox (ENC)"); str_cpy(wifi_ssids[1], "Telekom-AB12"); str_cpy(wifi_ssids[2], "Linksys-Home"); str_cpy(wifi_ssids[3], "Open-Wifi"); } }

_50 nic_select_next() { _15(nic_count EQ 0) _96; active_nic_idx++; _15(active_nic_idx >= nic_count) active_nic_idx = 0; str_cpy(hw_net, found_nics[active_nic_idx].name); _15(found_nics[active_nic_idx].type EQ 1) rtl8139_init(found_nics[active_nic_idx].address); _41 _15(found_nics[active_nic_idx].type EQ 2) intel_e1000_init(found_nics[active_nic_idx].address); _41 _15(found_nics[active_nic_idx].type EQ 3) { str_cpy(mac_str, "5C:F6:DC:32:C6:47"); _15(usb_io_base NEQ 0) uhci_init(usb_io_base); _41 str_cpy(cmd_status, "USB IO ERR"); windows[7].open = _128; windows[7].minimized = _86; wifi_state = 0; } }

_50 pci_scan_all() {
    nic_count = 0;
    _39(_184 bus = 0; bus < 6; bus++) { 
        _39(_184 dev = 0; dev < 32; dev++) {
            _89 id = pci_read(bus, dev, 0, 0);
            _15((id & 0xFFFF) NEQ 0xFFFF) { 
                _89 class_rev = pci_read(bus, dev, 0, 0x08); _184 cls = (class_rev >> 24) & 0xFF; _184 sub = (class_rev >> 16) & 0xFF; _182 vendor = id & 0xFFFF; 
                _15(cls EQ 0x01 OR cls EQ 0x04) { _89 bar0 = pci_read(bus, dev, 0, 0x10); _15((bar0 & 1) AND (bar0 > 1)) { ata_base = (bar0 & 0xFFFFFFFC); } _15(cls EQ 0x01 AND sub EQ 0x08) { _15(drive_count < 8) { _43 d = drive_count; drives[d].present = _128; drives[d].type = 2; str_cpy(drives[d].model, "NVMe SSD (PCIE)"); drives[d].size_mb = 0; drive_count++; } } }
                _15(cls EQ 0x02) { _89 bar0 = pci_read(bus, dev, 0, 0x10); _15(nic_count < 5) { found_nics[nic_count].address = bar0; _15(vendor EQ 0x10EC) { str_cpy(found_nics[nic_count].name, "REALTEK NIC"); found_nics[nic_count].type=1; } _41 _15(vendor EQ 0x8086) { str_cpy(found_nics[nic_count].name, "INTEL GIGABIT"); found_nics[nic_count].type=2; } _41 { str_cpy(found_nics[nic_count].name, "GENERIC NIC"); found_nics[nic_count].type=0; } nic_count++; } }
                _15(cls EQ 0x03) { str_cpy(hw_gpu, "GPU DEV"); }
                _15(cls EQ 0x0C AND sub EQ 0x03) { usb_detected = _128; str_cpy(hw_usb, "USB HOST"); _89 bar4 = pci_read(bus, dev, 0, 0x20); _15(bar4 & 1) usb_io_base = bar4 & ~3; _15(nic_count < 5) { str_cpy(found_nics[nic_count].name, "SAMSUNG WIFI USB"); found_nics[nic_count].type = 3; nic_count++; } }
                _15(vendor EQ 0x046D) { logitech_found = _128; webcam_active = _128; str_cpy(webcam_model, "LOGITECH C270"); } 
            }
        }
    }
    _15(nic_count > 0) { active_nic_idx = -1; nic_select_next(); } _41 str_cpy(hw_net, "NO NIC FOUND");
}

_43 ata_wait_busy(_182 base) { _43 t=100000; _114(t--) { _15(!(inb(base+7)&0x80)) _96 0; } _96 1; }
_43 ata_wait_drq(_182 base) { _43 t=100000; _114(t--) { _15(inb(base+7)&0x08) _96 0; } _96 1; }
_50 ata_identify(_182 base, _44 slave, _43 slot) { _15(inb(base+7) EQ 0xFF) _96; outb(base+6, slave ? 0xB0 : 0xA0); outb(base+2, 0); outb(base+3, 0); outb(base+4, 0); outb(base+5, 0); outb(base+7, 0xEC); _15(inb(base+7) EQ 0) _96; _15(ata_wait_busy(base)) _96; _15(inb(base+4) NEQ 0 OR inb(base+5) NEQ 0) _96; _15(ata_wait_drq(base)) _96; insw(base, hdd_buf, 256); _15(slot < 8) { drives[slot].present = _128; drives[slot].base_port = base; drives[slot].is_slave = slave; drives[slot].type = 0; _30* model = drives[slot].model; _39(_43 i=0; i<20; i++) { _182 w = ((_182*)hdd_buf)[27+i]; model[i*2] = (w >> 8); model[i*2+1] = (w & 0xFF); } model[40] = 0; _39(_43 i=39; i>0; i--) _15(model[i] EQ ' ') model[i]=0; _41 _37; _89 lba28 = ((_182*)hdd_buf)[60] | (((_182*)hdd_buf)[61] << 16); drives[slot].size_mb = (lba28 / 2048); drive_count++; } }
_50 ata_read_sector(_182 base, _44 slave, _89 lba, _184* buffer) { _15(base EQ 0) _96; _15(ata_wait_busy(base)) _96; outb(base+6, 0xE0 | (slave << 4) | ((lba >> 24) & 0x0F)); outb(base+1, 0x00); outb(base+2, 1); outb(base+3, (_184)lba); outb(base+4, (_184)(lba >> 8)); outb(base+5, (_184)(lba >> 16)); outb(base+7, 0x20); _15(ata_wait_drq(base)) _96; insw(base, buffer, 256); }
_50 ata_write_sector(_182 base, _44 slave, _89 lba, _184* buffer) { _15(base EQ 0) _96; _15(ata_wait_busy(base)) _96; outb(base+6, 0xE0 | (slave << 4) | ((lba >> 24) & 0x0F)); outb(base+1, 0x00); outb(base+2, 1); outb(base+3, (_184)lba); outb(base+4, (_184)(lba >> 8)); outb(base+5, (_184)(lba >> 16)); outb(base+7, 0x30); _15(ata_wait_drq(base)) _96; outsw(base, buffer, 256); }
_43 ata_probe(_182 port) { ata_base = port; outb(ata_base + 6, 0xA0); outb(ata_base + 7, 0xEC); _15(inb(ata_base + 7) EQ 0) _96 0; _15(!ata_wait_busy(ata_base)) _96 0; _15(inb(ata_base + 7) & 1) _96 0; insw(ata_base, hdd_buf, 256); _96 1; }
_50 ata_scan_drives() { drive_count = 0; _39(_43 i=0; i<8; i++) drives[i].present = _86; ata_identify(0x1F0, _86, 0); ata_identify(0x1F0, _128, 1); ata_identify(0x170, _86, 2); ata_identify(0x170, _128, 3); _15(usb_detected AND drive_count < 8) { _43 d = drive_count; drives[d].present = _128; drives[d].type = 1; str_cpy(drives[d].model, "USB MASS STORAGE"); drives[d].size_mb = 16000; drive_count++; } _15(drive_count > 0) str_cpy(hw_disk, drives[0].model); _41 str_cpy(hw_disk, "NO DRIVES"); }
_50 mbr_scan() { _15(drives[0].present AND drives[0].type EQ 0) { ata_read_sector(drives[0].base_port, drives[0].is_slave, 0, sector0); _15(sector0[510] EQ 0x55 AND sector0[511] EQ 0xAA) { str_cpy(hw_disk, "DRIVE 0: MBR FOUND"); _39(_43 i=0; i<4; i++) { _43 off = 446 + (i * 16); partitions[i].status = sector0[off]; partitions[i].type = sector0[off+4]; partitions[i].start_lba = *(_89*)&sector0[off+8]; partitions[i].size = *(_89*)&sector0[off+12]; } } _41 str_cpy(hw_disk, "DRIVE 0: UNKNOWN"); } }

_50 fs_flush_table() { _15(drive_status AND drives[0].type EQ 0) { _39(_43 i=0; i<512; i++) hdd_buf[i] = 0; _43 off = 0; _39(_43 i=0; i<8; i++) { _15(file_table[i].exists) { hdd_buf[off] = 1; _30* p = file_table[i].name; _39(_43 k=0;k<30;k++) _15(*p) hdd_buf[off+1+k] = *p++; _41 hdd_buf[off+1+k] = 0; *(_89*)&hdd_buf[off+32] = file_table[i].size; hdd_buf[off+36] = file_table[i].is_folder ? 1 : 0; _30* d = file_table[i].date; _39(_43 k=0;k<12;k++) _15(*d) hdd_buf[off+37+k] = *d++; _41 hdd_buf[off+37+k]=0; hdd_buf[off+50] = file_table[i].parent_idx; } off += 64; } ata_write_sector(drives[0].base_port, drives[0].is_slave, 200, hdd_buf); } }
_50 fs_create_folder(_71 _30* name) { _39(_43 i=0; i<8; i++) { _15(!file_table[i].exists) { file_table[i].exists = _128; str_cpy(file_table[i].name, name); file_table[i].is_folder = _128; file_table[i].size = 0; read_rtc(); _30 ds[]="00.00.00"; ds[0]='0'+rtc_day/10; ds[1]='0'+rtc_day%10; ds[3]='0'+rtc_mon/10; ds[4]='0'+rtc_mon%10; ds[6]='0'+(rtc_year/10)%10; ds[7]='0'+rtc_year%10; str_cpy(file_table[i].date, ds); file_table[i].parent_idx = current_path_id; fs_flush_table(); _96; } } }
_50 fs_save_file(_71 _30* name, _89 size) { _15(active_drive_idx NEQ -1 AND drives[active_drive_idx].type EQ 0) { _39(_43 i=0; i<512; i++) hdd_buf[i] = 0; _43 idx = 0; _39(_43 r=0; r<10; r++) { _39(_43 c=0; c<40; c++) { _15(note_buf[r][c]) hdd_buf[idx++] = note_buf[r][c]; _41 hdd_buf[idx++] = 32; } } ata_write_sector(drives[active_drive_idx].base_port, drives[active_drive_idx].is_slave, 1000, hdd_buf); _43 slot = -1; _39(_43 i=0; i<8; i++) { _15(file_table[i].exists AND str_equal(file_table[i].name, name)) { slot = i; _37; } } _15(slot EQ -1) { _39(_43 i=0; i<8; i++) { _15(!file_table[i].exists) { slot = i; _37; } } } _15(slot NEQ -1) { file_table[slot].exists = _128; str_cpy(file_table[slot].name, name); file_table[slot].is_folder = _86; file_table[slot].size = size; file_table[slot].sector_offset = 1000; read_rtc(); _30 ds[]="00.00.2000"; ds[0]='0'+rtc_day/10; ds[1]='0'+rtc_day%10; ds[3]='0'+rtc_mon/10; ds[4]='0'+rtc_mon%10; ds[6]='0'+(rtc_year/1000)%10; ds[7]='0'+(rtc_year/100)%10; ds[8]='0'+(rtc_year/10)%10; ds[9]='0'+rtc_year%10; str_cpy(file_table[slot].date, ds); file_table[slot].parent_idx = current_path_id; fs_flush_table(); } } }
_50 fs_save() { fs_save_file("QUICK.TXT", 512); } 
_50 fs_init() { pci_scan_all(); drive_status = 0; _15(ata_probe(ata_base)) drive_status = 1; _41 _15(ata_probe(0x1F0)) drive_status = 1; _41 _15(ata_probe(0x170)) drive_status = 1; ata_scan_drives(); _15(drive_count > 0) mbr_scan(); _44 found_table = _86; _15(drive_status AND drives[0].type EQ 0) { ata_read_sector(drives[0].base_port, drives[0].is_slave, 200, hdd_buf); _15(hdd_buf[0] NEQ 0) { found_table = _128; _43 off = 0; _39(_43 i=0; i<8; i++) { _15(hdd_buf[off] EQ 1) { file_table[i].exists = _128; _30* p = file_table[i].name; _39(_43 k=0;k<30;k++) *p++ = hdd_buf[off+1+k]; *p=0; file_table[i].size = *(_89*)&hdd_buf[off+32]; file_table[i].is_folder = (hdd_buf[off+36] EQ 1); _30* d = file_table[i].date; _39(_43 k=0;k<12;k++) *d++ = hdd_buf[off+37+k]; *d=0; file_table[i].parent_idx = hdd_buf[off+50]; } _41 { file_table[i].exists = _86; } off += 64; } } } _15(!found_table) { _39(_43 i=0; i<8; i++) file_table[i].exists = _86; file_table[0].exists=_128; str_cpy(file_table[0].name, "boot.sys"); file_table[0].is_folder=_86; file_table[0].size=512; str_cpy(file_table[0].date, "01.01.2023"); file_table[0].parent_idx = 255; file_table[1].exists=_128; str_cpy(file_table[1].name, "kernel.bin"); file_table[1].is_folder=_86; file_table[1].size=10240; str_cpy(file_table[1].date, "22.05.2023"); file_table[1].parent_idx = 255; } str_cpy(wifi_ssids[0], "Home-WiFi (90%)"); str_cpy(wifi_ssids[1], "Telekom-AB12"); str_cpy(wifi_ssids[2], "Office-Net"); str_cpy(wifi_ssids[3], "Guest-Access"); }

_50 focus_window(_43 id) { _43 found_at = -1; _39(_43 i=0; i<11; i++) _15(win_z[i] EQ id) found_at = i; _15(found_at EQ -1) _96; _39(_43 i=found_at; i<10; i++) win_z[i] = win_z[i+1]; win_z[10] = id; }

// ==========================================
// 7. HOLYSPIRIT LOGIC
// ==========================================
_50 spirit_log_add(_71 _30* msg) {
    _39(_43 i=5; i>0; i--) str_cpy(spirit_logs[i], spirit_logs[i-1]);
    str_cpy(spirit_logs[0], msg);
}

_50 spirit_scan() {
    // SIMULATION
    _15(random() % 50 EQ 0) {
        _15(spirit_count < 10) {
            _30 ip[20]; _30 b1[4]; int_to_str(192, b1); 
            str_cpy(ip, "192.168.0."); 
            _30 last[4]; int_to_str(random()%255, last);
            _43 l = str_len(ip); _30* p = last; _114(*p) ip[l++] = *p++; ip[l] = 0;
            str_cpy(spirit_targets[spirit_count].ip, ip);
            spirit_targets[spirit_count].hits = 1;
            spirit_targets[spirit_count].critical = _86;
            spirit_targets[spirit_count].x_off = (random()%100)-50;
            spirit_targets[spirit_count].y_off = (random()%100)-50;
            spirit_count++;
            spirit_log_add("TARGET DETECTED");
        }
    }
}

// ==========================================
// 8. KERNEL MAIN
// ==========================================
_172 "C" _50 kernel_main(_89* mbi) {
    // Sicherstellen, dass Multiboot-Flags Grafikdaten enthalten (Bit 11)
    _15 (mbi[0] & (1 << 11)) {
        fb = (_89*)(uintptr_t)mbi[22]; 
        pitch = mbi[24];
        screen_w = mbi[25]; 
        screen_h = mbi[26];
    } _41 {
        // Fallback: Wenn GRUB keine Grafik liefert, nutzen wir 
        // eine Standard-VGA-Adresse oder halten das System an.
        fb = (_89*)0xFD000000; // Oft ein gängiger Default für VESA
        pitch = 800 * 4;
    }

    init_mouse();
    get_cpu_brand();
    fs_init();
    startup_melody(); 
    
    str_cpy(sleep_labels[0], "SLEEP (LIGHT)"); str_cpy(sleep_labels[1], "SLEEP (DEEP)"); str_cpy(sleep_labels[2], "HIBERNATE");
    str_cpy(sleep_labels[3], "HYBRID SLEEP"); str_cpy(sleep_labels[4], "LOCK SCREEN"); str_cpy(sleep_labels[5], "LOG OFF");

    windows[0].id=0; str_cpy(windows[0].title, "NOTEPAD"); windows[0].x=100; windows[0].y=100; windows[0].w=400; windows[0].h=300; windows[0].color=0xEEEEEE;
    windows[1].id=1; str_cpy(windows[1].title, "APPS");    windows[1].x=150; windows[1].y=150; windows[1].w=350; windows[1].h=250; windows[1].color=0xDDDDDD;
    windows[2].id=2; str_cpy(windows[2].title, "SAVE AS..."); windows[2].x=200; windows[2].y=150; windows[2].w=300; windows[2].h=200; windows[2].color=0xDDDDDD;
    windows[3].id=3; str_cpy(windows[3].title, "SYSTEM");  windows[3].x=200; windows[3].y=50;  windows[3].w=350; windows[3].h=480; windows[3].color=0xFFD700;
    windows[4].id=4; str_cpy(windows[4].title, "DISK MGR"); windows[4].x=250; windows[4].y=200; windows[4].w=450; windows[4].h=350; windows[4].color=0x888888;
    windows[5].id=5; str_cpy(windows[5].title, "CMD");     windows[5].x=100; windows[5].y=300; windows[5].w=450; windows[5].h=250; windows[5].color=0x111111;
    windows[6].id=6; str_cpy(windows[6].title, "GENESIS DB"); windows[6].x=150; windows[6].y=100; windows[6].w=500; windows[6].h=400; windows[6].color=0x222222;
    windows[7].id=7; str_cpy(windows[7].title, "WIFI MGR"); windows[7].x=200; windows[7].y=150; windows[7].w=300; windows[7].h=300; windows[7].color=0x004488;
    windows[8].id=8; str_cpy(windows[8].title, "ACCESS");   windows[8].x=300; windows[8].y=100; windows[8].w=250; windows[8].h=300; windows[8].color=0x44AAAA;
    windows[9].id=9; str_cpy(windows[9].title, "SENSES");   windows[9].x=400; windows[9].y=100; windows[9].w=300; windows[9].h=350; windows[9].color=0x222222;
    
    // HOLY SPIRIT WINDOW
    windows[10].id=10; str_cpy(windows[10].title, "HOLY SPIRIT"); windows[10].x=80; windows[10].y=80; windows[10].w=500; windows[10].h=400; windows[10].color=0x110011;

    _43 clock_dirs[] = {213, 0, 42, 85, 128}; _39(_43 i=0;i<5;i++){ planets[i].ang = clock_dirs[i]; planets[i].dist = 10; planets[i].cur_x = 400; planets[i].cur_y = 300; str_cpy(planets[i].name, (i==0?"TXT":i==1?"APP":i==2?"SYS":i==3?"DSK":"CMD")); }
    _39(_43 i=0;i<200;i++) stars[i]={(i*19)%799-399, (i*29)%599-299, (i*13)+1};
    _43 cx = 400; _43 cy = 300; _44 is_modal_blocked = _86; _44 click_consumed = _86; 

    _114(1) {
        handle_input(); Clear();
        _43 map_ids[]={0,1,3,4,5}; _15(input_cooldown > 0) input_cooldown--; 
        is_modal_blocked = (windows[2].open AND !windows[2].minimized);
        z_blocked = _86; click_consumed = _86; _44 mouse_handled = _86;
        
        _15(frame % 10 EQ 0) check_incoming();

        _15(mouse_left) {
             _15(drag_win NEQ -1) { windows[drag_win].x = mouse_x - drag_off_x; windows[drag_win].y = mouse_y - drag_off_y; mouse_handled=_128; click_consumed=_128; } 
             _41 _15(resize_win NEQ -1) { _43 nw = mouse_x - windows[resize_win].x; _43 nh = mouse_y - windows[resize_win].y; _15(nw > 100) windows[resize_win].w = nw; _15(nh > 100) windows[resize_win].h = nh; mouse_handled=_128; click_consumed=_128; }
        } _41 { drag_win = -1; resize_win = -1; }

        _15(!mouse_handled) {
            _39(_43 i=10; i>=0; i--) { 
                _43 k = win_z[i]; Window* win=&windows[k];
                _15(win->open AND !win->minimized) {
                    _43 wx=(win->fullscreen?0:win->x); _43 wy=(win->fullscreen?0:win->y); _43 ww=(win->fullscreen?800:win->w); _43 wh=(win->fullscreen?600:win->h);
                    _15(mouse_x>=wx AND mouse_x<=wx+ww AND mouse_y>=wy AND mouse_y<=wy+wh) {
                        z_blocked = _128;
                        _15(mouse_just_pressed) {
                           click_consumed = _128; 
                           _15(!is_modal_blocked OR k EQ 2) { 
                               focus_window(k);
                               _43 bx = wx + ww/2; 
                               _15(mouse_y < wy+40) { 
                                   _15(mouse_x > bx-70 AND mouse_x < bx-30) win->minimized=_128; 
                                   _41 _15(mouse_x > bx-20 AND mouse_x < bx+40) win->fullscreen = !win->fullscreen; 
                                   _41 _15(mouse_x > bx+45 AND mouse_x < bx+70) { win->open=_86; _15(win->id EQ 4) { dsk_view=0; current_path_id=255; } }
                                   _41 { drag_win = k; drag_off_x = mouse_x - wx; drag_off_y = mouse_y - wy; }
                               }
                               _15(mouse_x > wx+ww-20 AND mouse_y > wy+wh-20) { resize_win = k; }
                           }
                        }
                        mouse_handled = _128; _37; 
                    }
                }
            }
        }

        _15(active_mode EQ 0) { 
            _43 bf=++boot_frame; 
            _15(bf < 60) DrawRoundWindow(400, 300, bf * 3, 0xFFD700); 
            _41 _15(bf < 220) { Clear(); boot_logo(); _15(bf EQ 70) play_earthquake(15); }
            _41 active_mode = 1; 
        } _41 {
            _43 v_cx = 400; _43 v_cy = 300; 
            _39(_43 i=0;i<200;i++){ stars[i].z -= 2; _15(stars[i].z <= 0) { stars[i].z = 1000; stars[i].x = (random()%799)-399; stars[i].y = (random()%599)-299; } Put(v_cx + (stars[i].x * 256) / stars[i].z, v_cy + (stars[i].y * 256) / stars[i].z, 0x444444); }
            _15(galaxy_open AND galaxy_expansion < 320) galaxy_expansion += 8; _15(!galaxy_open AND galaxy_expansion > 0) galaxy_expansion -= 10;
            DrawCentrifugalVortex(v_cx, v_cy, galaxy_expansion); DrawRoundWindow(v_cx, v_cy, 50, 0xFFD700); TextC(v_cx, v_cy-15, "COSMOS", 0x000000, _128);
            _15(frame % 200 EQ 0) read_rtc(); _30 ts[]="00:00"; ts[0]='0'+rtc_h/10; ts[1]='0'+rtc_h%10; ts[3]='0'+rtc_m/10; ts[4]='0'+rtc_m%10; TextC(v_cx, v_cy+5, ts, 0x000000, _128); _30 ds[]="00.00.2000"; ds[0]='0'+rtc_day/10; ds[1]='0'+rtc_day%10; ds[3]='0'+rtc_mon/10; ds[4]='0'+rtc_mon%10; ds[8]='0'+(rtc_year%100)/10; ds[9]='0'+rtc_year%10; TextC(v_cx, v_cy+20, ds, 0x000000, _128);

            _39(_43 i=0; i<5; i++) {
                Window* win=&windows[map_ids[i]];
                _43 target_x, target_y; _44 draw_moons = _86;
                _15(win->minimized) { target_x = 250 + (i * 70); target_y = 560; } 
                _41 _15(win->open) { _43 orbit_dist = 60 + i*50; target_x = v_cx + (Cos(planets[i].ang) * orbit_dist) / 84; target_y = v_cy + (Sin(planets[i].ang) * orbit_dist * 3/4) / 84; draw_moons = _128; }
                _41 {
                    _15(galaxy_expansion >= 100) { _15(planets[i].dist < 60 + i*50) planets[i].dist += 2; _15(planets[i].dist > 50) { _15(frame % (8+i) EQ 0) planets[i].ang = (planets[i].ang + 1) % 256; } } _41 { _15(planets[i].dist > 10) planets[i].dist -= 8; }
                    target_x = v_cx + (Cos(planets[i].ang) * planets[i].dist) / 84; target_y = v_cy + (Sin(planets[i].ang) * planets[i].dist * 3/4) / 84;
                }
                planets[i].cur_x += (target_x - planets[i].cur_x) / 4; planets[i].cur_y += (target_y - planets[i].cur_y) / 4;
                _15(galaxy_expansion > 10 OR win->minimized OR win->open) {
                    _43 px = planets[i].cur_x; _43 py = planets[i].cur_y;
                    _44 hov = is_over(mouse_x, mouse_y, px, py, 20); 
                    _15(hov AND !z_blocked) DrawRoundWindow(px, py, 22, 0xDDDDDD); _41 DrawRoundWindow(px, py, 20, 0x999999);
                    _15(draw_moons) { DrawRoundWindow(px-30, py, 4, 0xAAAAAA); DrawRoundWindow(px+30, py, 4, 0xAAAAAA); }
                    TextC(px, py-4, planets[i].name, 0x000000, _128); 
                    _15(!z_blocked AND mouse_just_pressed AND !is_modal_blocked AND !click_consumed AND hov) { 
                        _15(win->minimized) win->minimized = _86; 
                        _41 { win->open = _128; focus_window(win->id); _15(win->id EQ 4) { dsk_view=0; current_path_id=255; input_cooldown=15; } }
                    }
                }
            }
            _15(key_shift) { DrawRoundedRect(740, 570, 50, 20, 2, 0xFFFFFF); Text(745, 575, "SHIFT", 0x000000, _128); }

            _39(_43 i=0; i<11; i++) {
                _43 k = win_z[i]; Window* win=&windows[k]; _15(!win->open OR win->minimized) _101;
                _44 blocked = (is_modal_blocked AND k NEQ 2); 
                _43 wx=(win->fullscreen?0:win->x); _43 wy=(win->fullscreen?0:win->y); _43 ww=(win->fullscreen?800:win->w); _43 wh=(win->fullscreen?600:win->h);
                _89 win_bg = win->color; _15(blocked) win_bg = 0x999999; _15(win_bg EQ 0) win_bg = 0xCCCCCC; _15(is_modal_blocked AND k NEQ 2) win_bg = 0x888888; 
                DrawRoundedRect(wx, wy, ww, wh, 12, win_bg);
                Text(wx+15, wy+15, win->title, (win->id EQ 10) ? 0x00FF00 : 0x000000, _128);
                _43 bx = wx + ww/2; Text(bx-60, wy+15, "MIN", 0x555555, _128); Text(bx-10, wy+15, "FULL", 0x555555, _128); Text(bx+50, wy+15, "X", 0x000000, _128);
                
                _15(win->id EQ 1) { 
                    TextC(wx+ww/2, wy+40, "INSTALLED APPS", 0x222222, _128); 
                    DrawIcon(wx+ww/2-15, wy+80, 0xFFD700); TextC(wx+ww/2, wy+135, "GENESIS DB", 0x333333, _128); 
                    _15(mouse_just_pressed AND !blocked AND is_over(mouse_x, mouse_y, wx+ww/2-15, wy+80, 40)) { windows[6].open=_128; windows[6].minimized=_86; focus_window(6); } 
                    
                    DrawRoundedRect(wx+ww/2-50, wy+180, 100, 30, 5, 0xAAAAAA); TextC(wx+ww/2, wy+188, "ACCESS", 0xFFFFFF, _128); 
                    _15(mouse_just_pressed AND !blocked AND is_over_rect(mouse_x, mouse_y, wx+ww/2-50, wy+180, 100, 30)) { windows[8].open=_128; windows[8].minimized=_86; focus_window(8); } 
                    
                    // --- HOLYSPIRIT LAUNCHER ---
                    DrawIcon(wx+ww/2-15, wy+220, 0xFF00FF);
                    TextC(wx+ww/2, wy+260, "HOLY SPIRIT", 0xFF00FF, _128);
                    _15(mouse_just_pressed AND !blocked AND is_over(mouse_x, mouse_y, wx+ww/2-15, wy+220, 40)) { 
                        windows[10].open=_128; windows[10].minimized=_86; focus_window(10); 
                    }
                }

                // --- HOLYSPIRIT WINDOW LOGIC ---
                _15(win->id EQ 10) {
                    draw_spirit_radar(wx, wy, ww, wh);
                    _43 by = wy + wh - 40;
                    DrawRoundedRect(wx+20, by, 100, 30, 5, spirit_active ? 0x004400 : 0x440000);
                    TextC(wx+70, by+8, spirit_active ? "ACTIVE" : "PAUSED", 0xFFFFFF, _128);
                    
                    _15(mouse_just_pressed AND !blocked AND is_over_rect(mouse_x, mouse_y, wx+20, by, 100, 30)) {
                        spirit_active = !spirit_active;
                        spirit_log_add(spirit_active ? "SYSTEM ARMED" : "SYSTEM HALTED");
                    }
                    _15(spirit_active) {
                        spirit_timer++;
                        _15(spirit_timer > 50) { spirit_scan(); spirit_timer=0; }
                    }
                }
                
                // ... REST OF WINDOWS ...
                _15(win->id EQ 3) { 
                    _43 mid=wx+ww/2; _43 btn_y = wy + 45; _30 lang_lbl[20], theme_lbl[30];
                    _15(sys_lang EQ 0) str_cpy(lang_lbl, "[ LANG: EN ]"); _41 str_cpy(lang_lbl, "[ SPR: DE ]");
                    _15(sys_lang EQ 0) { _15(sys_theme EQ 0) str_cpy(theme_lbl, "[ THEME: COMPUTER ]"); _41 str_cpy(theme_lbl, "[ THEME: GENESIS ]"); } _41 { _15(sys_theme EQ 0) str_cpy(theme_lbl, "[ THEMA: COMPUTER ]"); _41 str_cpy(theme_lbl, "[ THEMA: GENESIS ]"); }
                    _15(input_cooldown EQ 0 AND mouse_just_pressed AND !blocked AND is_over_rect(mouse_x, mouse_y, wx+5, btn_y, 140, 20)) { sys_lang = !sys_lang; input_cooldown = 20; }
                    Text(wx+10, btn_y+4, lang_lbl, 0x000000, _128); 
                    _15(input_cooldown EQ 0 AND mouse_just_pressed AND !blocked AND is_over_rect(mouse_x, mouse_y, wx+5, btn_y+30, 200, 20)) { sys_theme = !sys_theme; input_cooldown = 20; }
                    Text(wx+10, btn_y+34, theme_lbl, 0x000000, _128); 
                    
                    TextC(mid, wy+45, "REBOOT", (sys_menu_selection EQ 0)?0x555555:0x000000, _128); 
                    _39(_43 s=0; s<6; s++) { 
                        _43 ly = wy+85+s*15; _44 hov = is_over_rect(mouse_x, mouse_y, mid-50, ly-2, 100, 12);
                        _89 sc = (sleep_mode_idx EQ s OR sys_menu_selection EQ s+2) ? 0x888888 : 0x000000; _15(hov) sc = 0xFFFFFF;
                        _15(hov) DrawRoundedRect(mid-50, ly-2, 100, 14, 2, 0x000000); TextC(mid, ly, sleep_labels[s], sc, _128); _15(mouse_just_pressed AND !blocked AND hov) sleep_mode_idx = s;
                    }
                    TextC(mid, wy+210, user_name, 0x222222, _128); TextC(mid, wy+225, ip_address, 0x222222, _128); TextC(mid, wy+240, cpu_brand, 0x222222, _86); DrawRoundedRect(wx+20, wy+260, ww-40, 2, 0, 0xAAAAAA);
                    _71 _30* l_hw = sys_lang ? "HARDWARE GEFUNDEN" : "DETECTED HARDWARE"; TextC(mid, wy+270, l_hw, 0x000000, _128);
                    _30 l_disk[30], l_net[30], l_gpu[30], l_usb[30];
                    _15(sys_theme EQ 0) { str_cpy(l_disk, sys_lang ? "FESTPLATTE:" : "STORAGE:"); str_cpy(l_net, sys_lang ? "NETZWERK:" : "NETWORK:"); str_cpy(l_gpu, sys_lang ? "GRAFIK:" : "GRAPHICS:"); str_cpy(l_usb, sys_lang ? "USB HOST:" : "USB HOST:"); } _41 { str_cpy(l_disk, sys_lang ? "ERD-ANKER:" : "TERRA ANCHOR:"); str_cpy(l_net, sys_lang ? "KOSMOS NETZ:" : "COSMIC WEB:"); str_cpy(l_gpu, sys_lang ? "REALITAETS RENDERER:" : "REALITY RENDERER:"); str_cpy(l_usb, sys_lang ? "DATEN INJEKTOR:" : "DATA INJECTOR:"); }
                    Text(wx+30, wy+290, l_disk, 0x555555, _86); Text(wx+170, wy+290, hw_disk, 0x222222, _86); 
                    Text(wx+30, wy+305, l_net, 0x000000, _86); _15(mouse_just_pressed AND !blocked AND is_over(mouse_x, mouse_y, wx+50, wy+305, 30)) { nic_select_next(); }
                    Text(wx+170, wy+305, hw_net, 0x222222, _86); Text(wx+30, wy+320, l_gpu, 0x555555, _86);  Text(wx+170, wy+320, hw_gpu, 0x222222, _86); Text(wx+30, wy+335, l_usb, 0x555555, _86);  Text(wx+170, wy+335, hw_usb, 0x222222, _86);
                    TextC(mid, wy+wh-40, "SHUT DOWN", (sys_menu_selection EQ 8)?0x555555:0x000000, _128); _15(mouse_just_pressed AND !blocked) { _15(is_over(mouse_x,mouse_y,mid,wy+45,20)) system_reboot(); _15(is_over(mouse_x,mouse_y,mid,wy+wh-40,20)) system_shutdown(); }
                }
                
                _15(win->id EQ 8) {
                    TextC(wx+ww/2, wy+40, "ACCESSIBILITY", 0x000000, _128);
                    _44 t_hov = is_over_rect(mouse_x, mouse_y, wx+20, wy+80, 210, 40);
                    DrawRoundedRect(wx+20, wy+80, 210, 40, 5, touch_mode ? 0x000000 : (t_hov ? 0x999999 : 0xCCCCCC));
                    TextC(wx+ww/2, wy+92, touch_mode ? "TOUCH MODE: ON" : "TOUCH MODE: OFF", 0xFFFFFF, _128);
                    _15(mouse_just_pressed AND !blocked AND t_hov) touch_mode = !touch_mode;
                    Text(wx+30, wy+140, "LOGITECH HARDWARE:", 0x000000, _86);
                    Text(wx+30, wy+160, (logitech_found OR webcam_active) ? "DETECTED (C270)" : "NOT FOUND", (logitech_found OR webcam_active) ? 0x000000 : 0x555555, _128);
                    DrawRoundedRect(wx+20, wy+240, 210, 30, 5, 0x333333); TextC(wx+ww/2, wy+247, "OPEN SENSES UI", 0xFFFFFF, _128);
                    _15(mouse_just_pressed AND !blocked AND is_over_rect(mouse_x, mouse_y, wx+20, wy+240, 210, 30)) { windows[9].open=_128; windows[9].minimized=_86; focus_window(9); }
                }
                _15(win->id EQ 9) {
                    Text(wx+20, wy+40, "VISION", senses_tab EQ 0?0x000000:0x555555, _128); Text(wx+100, wy+40, "VOICE", senses_tab EQ 1?0x000000:0x555555, _128);
                    _15(mouse_just_pressed AND !blocked) { _15(is_over_rect(mouse_x, mouse_y, wx+20, wy+35, 60, 20)) senses_tab=0; _15(is_over_rect(mouse_x, mouse_y, wx+100, wy+35, 60, 20)) senses_tab=1; }
                    _15(senses_tab EQ 0) { DrawRoundedRect(wx+50, wy+80, 200, 150, 5, 0x000000); DrawFace(wx+150, wy+155, mouse_x, mouse_y); DrawRoundedRect(wx+80, wy+250, 140, 30, 5, 0x444444); TextC(wx+150, wy+257, "CALIBRATE", 0xFFFFFF, _128); } 
                    _41 { DrawRoundedRect(wx+50, wy+80, 200, 100, 5, 0xAAAAAA); DrawWaveform(wx+50, wy+110, 200, frame); DrawRoundedRect(wx+80, wy+200, 140, 30, 5, 0x444444); TextC(wx+150, wy+207, "TEACH: 'OPEN'", 0xFFFFFF, _128); }
                }

                _15(win->id EQ 4) {
                    DrawRoundedRect(wx+ww-100, wy+15, 50, 20, 5, 0x555555); Text(wx+ww-90, wy+18, "REF", 0xFFFFFF, _128); 
                    _15(mouse_just_pressed AND !blocked AND is_over_rect(mouse_x, mouse_y, wx+ww-100, wy+15, 50, 20)) { fs_init(); input_cooldown = 20; } 
                    DrawRoundedRect(wx+ww-160, wy+15, 50, 20, 5, 0x333333); Text(wx+ww-150, wy+18, "HOME", 0xFFFFFF, _128); 
                    _15(mouse_just_pressed AND !blocked AND is_over_rect(mouse_x, mouse_y, wx+ww-160, wy+15, 50, 20)) { dsk_view=0; current_folder_view_idx=-1; current_path_id=255; input_cooldown=20; }
                    _15(dsk_view EQ 0) { 
                        Text(wx+20, wy+50, "DETECTED DRIVES (ATA/IDE):", 0x333333, _128); _43 y_off = wy+80;
                        _15(drive_count EQ 0) Text(wx+20, y_off, "NO DRIVES FOUND", 0x000000, _128);
                        _41 {
                            _39(_43 d=0; d<drive_count; d++) {
                                _44 sel = (d EQ dsk_selection); DrawRoundedRect(wx+20, y_off, ww-40, 40, 5, sel ? 0x999999 : 0xCCCCCC); DrawIcon(wx+30, y_off+5, 0x555555); Text(wx+55, y_off+12, drives[d].model, sel ? 0xFFFFFF : 0x000000, _128);
                                _30 sz_str[20]; str_cpy(sz_str, "SIZE: "); _43 mb = drives[d].size_mb; sz_str[6] = '0' + (mb/10000)%10; sz_str[7] = '0' + (mb/1000)%10; sz_str[8] = '0' + (mb/100)%10; sz_str[9] = 'M'; sz_str[10] = 'B'; sz_str[11] = 0; Text(wx+ww-120, y_off+12, sz_str, sel ? 0xFFFFFF : 0x333333, _128);
                                _15(input_cooldown EQ 0 AND mouse_just_pressed AND !blocked AND is_over_rect(mouse_x, mouse_y, wx+20, y_off, ww-40, 40)) { dsk_selection = d; active_drive_idx = d; dsk_view = 1; file_selection = 0; input_cooldown=15; } y_off += 50;
                            }
                        } TextC(wx+ww/2, wy+wh-30, "[ ARROWS: SELECT | ENTER: OPEN ]", 0x555555, _128);
                    } _41 _15(dsk_view EQ 1) { 
                        _44 back_sel = (file_selection EQ -1); DrawRoundedRect(wx+20, wy+50, 60, 20, 5, back_sel ? 0x999999 : 0xCCCCCC); Text(wx+25, wy+55, "BACK", back_sel ? 0xFFFFFF : 0x000000, _128); 
                        _15(input_cooldown EQ 0 AND mouse_just_pressed AND !blocked AND is_over_rect(mouse_x, mouse_y, wx+20, wy+50, 60, 20)) { dsk_view = 0; active_drive_idx = -1; input_cooldown=15; }
                        Text(wx+120, wy+55, "/ROOT/SYSTEM/", 0x000000, _128); Text(wx+45, wy+90, "NAME", 0x555555, _128); Text(wx+200, wy+90, "SIZE", 0x555555, _128); Text(wx+280, wy+90, "DATE", 0x555555, _128);
                        _43 y_off = wy+110;
                        _39(_43 f=0; f<8; f++) {
                            _15(file_table[f].exists AND file_table[f].parent_idx EQ 255) {
                                _44 sel = (f EQ file_selection); _15(sel) DrawRoundedRect(wx+20, y_off, ww-40, 20, 0, 0xAAAAAA); DrawIcon(wx+25, y_off, file_table[f].is_folder ? 0x888888 : 0xCCCCCC); Text(wx+45, y_off+5, file_table[f].name, 0x000000, _128);
                                _15(!file_table[f].is_folder) { _30 s_str[10]; int_to_str(file_table[f].size, s_str); Text(wx+200, y_off+5, s_str, 0x333333, _128); Text(wx+280, y_off+5, file_table[f].date, 0x333333, _128); }
                                _15(input_cooldown EQ 0 AND mouse_just_pressed AND !blocked AND is_over_rect(mouse_x, mouse_y, wx+20, y_off, ww-40, 20)) { 
                                    file_selection = f; 
                                    _15(file_table[f].is_folder) { dsk_view = 2; current_folder_view_idx = f; current_path_id = f; input_cooldown=10; } 
                                    _41 { windows[0].open = _128; windows[0].minimized = _86; focus_window(0); _15(drive_status) { _39(_43 r=0;r<10;r++) _39(_43 c=0;c<40;c++) note_buf[r][c]=0; note_r=0; note_c=0; ata_read_sector(drives[active_drive_idx].base_port, drives[active_drive_idx].is_slave, file_table[f].sector_offset, hdd_buf); _43 idx = 0; _39(_43 r=0; r<10; r++) { _39(_43 c=0; c<40; c++) { _30 ch = hdd_buf[idx++]; _15(ch < 32 OR ch > 126) ch = 0; note_buf[r][c] = ch; } note_buf[r][40] = 0; } } }
                                } y_off += 25;
                            }
                        }
                    } _41 _15(dsk_view EQ 2) { 
                         DrawRoundedRect(wx+20, wy+50, 60, 20, 5, 0xAAAAAA); Text(wx+25, wy+55, "BACK", 0xFFFFFF, _128);
                         _15(mouse_just_pressed AND !blocked AND is_over_rect(mouse_x, mouse_y, wx+20, wy+50, 60, 20)) { dsk_view = 1; current_path_id = 255; input_cooldown=10; } 
                         _15(current_folder_view_idx >= 0) {
                             _30 title[50]; str_cpy(title, "INSIDE FOLDER: "); _30* p = title; _114(*p) p++; _30* n = file_table[current_folder_view_idx].name; _114(*n) *p++ = *n++; *p=0; Text(wx+120, wy+55, title, 0x000000, _128); Text(wx+20, wy+100, "FILES IN FOLDER:", 0x333333, _128); _43 y_off = wy+120;
                             _39(_43 f=0; f<8; f++) {
                                 _15(file_table[f].exists AND file_table[f].parent_idx EQ current_folder_view_idx) {
                                     DrawIcon(wx+25, y_off, 0xCCCCCC); Text(wx+45, y_off+5, file_table[f].name, 0x000000, _128);
                                     _15(input_cooldown EQ 0 AND mouse_just_pressed AND !blocked AND is_over_rect(mouse_x, mouse_y, wx+20, y_off, ww-40, 20)) { windows[0].open = _128; windows[0].minimized = _86; focus_window(0); _15(drive_status) { _39(_43 r=0;r<10;r++) _39(_43 c=0;c<40;c++) note_buf[r][c]=0; note_r=0; note_c=0; ata_read_sector(drives[active_drive_idx].base_port, drives[active_drive_idx].is_slave, file_table[f].sector_offset, hdd_buf); _43 idx = 0; _39(_43 r=0; r<10; r++) { _39(_43 c=0; c<40; c++) { _30 ch = hdd_buf[idx++]; _15(ch < 32 OR ch > 126) ch = 0; note_buf[r][c] = ch; } note_buf[r][40] = 0; } } } y_off += 25;
                                 }
                             }
                         }
                    }
                }

                _15(win->id EQ 0) { 
                    _15(key_new AND win_z[10] EQ 0) { _15(last_key < 58) { _30 c = get_ascii(last_key); _15(last_key EQ 0x1C AND note_r<9){note_r++; note_c=0;} _41 _15(last_key EQ 0x0E) { _15(note_c>0) {note_c--; note_buf[note_r][note_c]=0;} _41 _15(note_r>0) { note_r--; note_c=39; } } _41 _15(c) { note_buf[note_r][note_c]=c; note_c++; _15(note_c > 38) { note_c=0; _15(note_r<9) note_r++; } } } }
                    _39(_43 r=0;r<10;r++) Text(wx+20, wy+50+r*15, note_buf[r], 0x333333, _128);
                    _15((frame/20)%2 EQ 0 AND win_z[10] EQ 0) DrawChar(wx+20+(note_c*7), wy+50+note_r*15, '_', 0x111111, _128);
                    _43 btn_y = wy+wh-25; Text(wx+20, btn_y, "[ SAVE ]", 0x333333, _128); Text(wx+100, btn_y, "[ SAVE AS ]", 0x333333, _128);
                    _15(mouse_just_pressed AND !blocked) {
                        _15(is_over(mouse_x, mouse_y, wx+40, btn_y+5, 30)) { fs_save_file("QUICK.TXT", 512); read_rtc(); _30 ds[]="SAVED: 00.00.00 00:00"; ds[7]='0'+rtc_day/10; ds[8]='0'+rtc_day%10; ds[10]='0'+rtc_mon/10; ds[11]='0'+rtc_mon%10; ds[13]='0'+(rtc_year%100)/10; ds[14]='0'+rtc_year%10; ds[16]='0'+rtc_h/10; ds[17]='0'+rtc_h%10; ds[19]='0'+rtc_m/10; ds[20]='0'+rtc_m%10; str_cpy(save_timestamp, ds); input_cooldown=10; } 
                        _15(is_over(mouse_x, mouse_y, wx+130, btn_y+5, 40)) { windows[2].open = _128; windows[2].minimized = _86; focus_window(2); save_step = 0; input_cooldown = 10; } 
                    }
                    Text(wx+ww-220, wy+wh-20, save_timestamp, 0x111111, _128); 
                }
                
                _15(win->id EQ 2) {
                    _15(save_step EQ 0) { 
                        Text(wx+20, wy+40, "SELECT DESTINATION:", 0x000000, _128); _44 sel = (save_part_sel EQ 0); DrawRoundedRect(wx+20, wy+70, ww-40, 30, 5, sel ? 0x999999 : 0xDDDDDD); Text(wx+30, wy+80, "DRIVE 0: PART 1 (SYSTEM)", sel ? 0xFFFFFF : 0x000000, _128);
                        _15(input_cooldown EQ 0 AND mouse_just_pressed AND is_over_rect(mouse_x, mouse_y, wx+20, wy+70, ww-40, 30)) { save_part_sel = 0; input_cooldown = 10; }
                        Text(wx+20, wy+120, "FOLDERS:", 0x555555, _128); _30 path_display[30] = "/ROOT/"; _15(current_path_id NEQ 255) { str_cpy(path_display, "/"); _30* p=path_display+1; _30* n=file_table[current_path_id].name; _114(*n) *p++ = *n++; *p++='/'; *p=0; }
                        DrawRoundedRect(wx+20, wy+140, ww-40, 60, 2, 0xEEEEEE); Text(wx+30, wy+150, path_display, 0x000000, _128);
                        DrawRoundedRect(wx+20, wy+180, 100, 25, 5, 0xCCCCCC); TextC(wx+70, wy+187, "NEW FOLDER", 0x000000, _128);
                        _15(input_cooldown EQ 0 AND mouse_just_pressed AND is_over_rect(mouse_x, mouse_y, wx+20, wy+180, 100, 25)) { save_step = 2; input_cooldown = 15; }
                        DrawRoundedRect(wx+ww-100, wy+wh-40, 80, 25, 5, 0x555555); TextC(wx+ww-60, wy+wh-32, "NEXT >", 0xFFFFFF, _128);
                        _15(input_cooldown EQ 0 AND mouse_just_pressed AND is_over_rect(mouse_x, mouse_y, wx+ww-100, wy+wh-40, 80, 25)) { save_step = 1; input_cooldown = 15; }
                    } _41 _15(save_step EQ 1) { 
                        _15(key_new AND win_z[10] EQ 2) { _15(last_key EQ 0x0E AND save_name_idx > 0) save_filename[--save_name_idx] = 0; _41 _15(save_name_idx < 30) { _30 c = get_ascii(last_key); _15(c >= 32) { save_filename[save_name_idx++] = c; save_filename[save_name_idx]=0; } } }
                        Text(wx+20, wy+40, "ENTER FILENAME:", 0x000000, _128); DrawRoundedRect(wx+20, wy+60, ww-40, 25, 2, 0xCCCCCC); Text(wx+25, wy+65, save_filename, 0x000000, _128); _15((frame/20)%2 EQ 0) DrawChar(wx+25+(save_name_idx*7), wy+65, '_', 0x000000, _128);
                        DrawRoundedRect(wx+ww-100, wy+wh-40, 80, 25, 5, 0x555555); TextC(wx+ww-60, wy+wh-32, "SAVE", 0xFFFFFF, _128);
                        _15(input_cooldown EQ 0 AND mouse_just_pressed AND is_over_rect(mouse_x, mouse_y, wx+ww-100, wy+wh-40, 80, 25)) { fs_save_file(save_filename, 1024); win->open = _86; input_cooldown = 20; }
                    } _41 _15(save_step EQ 2) { 
                        _15(key_new AND win_z[10] EQ 2) { _15(last_key EQ 0x0E AND folder_name_idx > 0) new_folder_name[--folder_name_idx] = 0; _41 _15(folder_name_idx < 30) { _30 c = get_ascii(last_key); _15(c >= 32) { new_folder_name[folder_name_idx++] = c; new_folder_name[folder_name_idx]=0; } } }
                        Text(wx+20, wy+40, "FOLDER NAME:", 0x000000, _128); DrawRoundedRect(wx+20, wy+60, ww-40, 25, 2, 0xCCCCCC); Text(wx+25, wy+65, new_folder_name, 0x000000, _128); _15((frame/20)%2 EQ 0) DrawChar(wx+25+(folder_name_idx*7), wy+65, '_', 0x000000, _128);
                        DrawRoundedRect(wx+ww-100, wy+wh-40, 80, 25, 5, 0x555555); TextC(wx+ww-60, wy+wh-32, "CREATE", 0xFFFFFF, _128);
                        _15(input_cooldown EQ 0 AND mouse_just_pressed AND is_over_rect(mouse_x, mouse_y, wx+ww-100, wy+wh-40, 80, 25)) { fs_create_folder(new_folder_name); save_step = 0; input_cooldown = 15; }
                    }
                }
                
                _15(win->id EQ 5) {
                    _15(key_new AND win_z[10] EQ 5) {
                        _15(last_key EQ 0x1C) { 
                            cmd_input[cmd_idx] = 0; _39(_43 l=4; l>0; l--) str_cpy(cmd_lines[l], cmd_lines[l-1]); str_cpy(cmd_lines[0], cmd_input);
                            _89 cid = get_id(cmd_input);
                            _15(str_equal(cmd_input, "DIR")) { str_cpy(cmd_last_out, ""); _43 p=0; _39(_43 i=0;i<8;i++) { _15(file_table[i].exists AND file_table[i].parent_idx EQ 255) { _71 _30* n=file_table[i].name; _114(*n AND p<38) cmd_last_out[p++]=*n++; _15(p<38) cmd_last_out[p++]=' '; } } cmd_last_out[p]=0; } 
                            _41 _15(str_equal(cmd_input, "HELP")) str_cpy(cmd_last_out, "CMD: DIR, NET, PING, IPCONFIG, CLS");
                            _41 _15(str_equal(cmd_input, "CLS")) { _39(_43 l=0;l<5;l++) cmd_lines[l][0]=0; str_cpy(cmd_last_out, "READY."); }
                            _41 _15(str_equal(cmd_input, "REBOOT")) system_reboot();
                            _41 _15(str_equal(cmd_input, "IPCONFIG")) { str_cpy(cmd_last_out, "MAC: "); _30* p=cmd_last_out+5; _30* m=mac_str; _114(*m) *p++=*m++; *p=0; }
                            _41 _15(str_equal(cmd_input, "CONNECT")) { send_udp(0xFFFFFFFF, 5000, 80, "COSMOS HELLO"); }
                            _41 _15(str_starts(cmd_input, "TCP")) { send_tcp_syn(0xFFFFFFFF, 80); }
                            _41 _15(str_starts(cmd_input, "GENESIS")) { _15(cid EQ 156) { genesis_tab = 0; str_cpy(cmd_last_out, "ACCESS GRANTED [ID 156]"); } _41 { str_cpy(cmd_last_out, "AUTH FAILED"); } }
                            _41 _15(str_starts(cmd_input, "PING")) { _15(str_equal(cmd_status, "ONLINE") OR str_starts(cmd_status, "ONLINE")) str_cpy(cmd_last_out, "REPLY FROM HOST: TIME=1ms"); _41 str_cpy(cmd_last_out, "FAIL: NO CONNECTION"); }
                            _41 str_cpy(cmd_last_out, "UNKNOWN COMMAND");
                            cmd_idx = 0;
                        } _41 _15(last_key EQ 0x0E AND cmd_idx > 0) { cmd_idx--; cmd_input[cmd_idx] = 0; }
                        _41 _15(cmd_idx < 30) { _30 c = get_ascii(last_key); _15(c >= 32) { cmd_input[cmd_idx++] = c; cmd_input[cmd_idx] = 0; } }
                    }
                    Text(wx+20, wy+50, "STATUS:", 0xAAAAAA, _128); Text(wx+90, wy+50, cmd_status, (str_equal(cmd_status, "ONLINE") OR str_starts(cmd_status,"ONLINE")) ? 0x00FF00 : 0xFF0000, _128);
                    _39(_43 l=0; l<4; l++) _15(cmd_lines[3-l][0] NEQ 0) Text(wx+20, wy+80+(l*15), cmd_lines[3-l], 0x666666, _128);
                    Text(wx+20, wy+150, "> ", 0x00FF00, _128); Text(wx+35, wy+150, cmd_last_out, 0x00FF00, _128);
                    Text(wx+20, wy+170, "C:\\>", 0x00FF00, _128); Text(wx+60, wy+170, cmd_input, 0x00FF00, _128);
                    _15((frame / 15) % 2 EQ 0) DrawChar(wx+60+(cmd_idx*7), wy+170, '_', 0x00FF00, _128);
                }
                _15(win->id EQ 6) { 
                    _43 mid=wx+ww/2; Text(wx+20, wy+40, "[ARCH]", genesis_tab EQ 0?0x000000:0x555555, _128); Text(wx+90, wy+40, "[HARD]", genesis_tab EQ 1?0x000000:0x555555, _128); Text(wx+160, wy+40, "[CODE]", genesis_tab EQ 2?0x000000:0x555555, _128);
                    _15(mouse_just_pressed AND !blocked AND mouse_y > wy+30 AND mouse_y < wy+50) { _15(mouse_x > wx+20 AND mouse_x < wx+80) genesis_tab=0; _15(mouse_x > wx+90 AND mouse_x < wx+150) genesis_tab=1; _15(mouse_x > wx+160 AND mouse_x < wx+220) genesis_tab=2; }
                    DrawRoundedRect(wx+20, wy+60, ww-40, 2, 0, 0x333333);
                    _15(genesis_tab EQ 0) { Text(wx+30, wy+80, "LEVEL 7: HIGH-ARCHITEKT (GOTT-MODUS)", 0x000000, _128); Text(wx+30, wy+100, "LEVEL 6: BLUEPRINT (PLANUNG)", 0xAAAAAA, _128); Text(wx+30, wy+120, "LEVEL 5: HIGH-LEVEL (OBJ-C)", 0xAAAAAA, _128); Text(wx+30, wy+140, "LEVEL 4: MANAGED (C# LOGIK)", 0xAAAAAA, _128); Text(wx+30, wy+160, "LEVEL 3: COMPLEX (C++ REALITAET)", 0x000000, _128); Text(wx+30, wy+180, "LEVEL 2: LOW-LEVEL (C BASIS)", 0xAAAAAA, _128); Text(wx+30, wy+200, "LEVEL 1: BINAER HIGH (EXISTENZ)", 0x555555, _128); Text(wx+30, wy+220, "LEVEL 0: BINAER LOW (NICHTS)", 0x555555, _128); } 
                    _41 _15(genesis_tab EQ 1) { Text(wx+30, wy+80, "ERDE (TERRA)", 0x000000, _128); Text(wx+30, wy+95, "FUNKTION: KERNEL / STROM", 0xAAAAAA, _128); Text(wx+30, wy+120, "SONNE (SOLAR)", 0x000000, _128); Text(wx+30, wy+135, "FUNKTION: CPU / LASER / SCHREIBEN", 0xAAAAAA, _128); Text(wx+30, wy+160, "MOND (LUNAR)", 0xCCCCCC, _128); Text(wx+30, wy+175, "FUNKTION: RAM / SPIEGEL / SPEICHER", 0xAAAAAA, _128); } 
                    _41 _15(genesis_tab EQ 2) { Text(wx+30, wy+80, "class NewbornSoul : public Wanderer {", 0x000000, _128); Text(wx+30, wy+95, "  if (!ConnectToAnchor(TERRA_CORE))", 0xCCCCCC, _128); Text(wx+30, wy+110, "     System::Panic(NO_POWER);", 0xFF0000, _128); Text(wx+30, wy+125, "  LoadTribeTemplate(zodiac);", 0xCCCCCC, _128); Text(wx+30, wy+140, "  SolarSystem::Laser::Broadcast();", 0xCCCCCC, _128); Text(wx+30, wy+155, "}", 0x000000, _128); }
                }
                _15(win->id EQ 7) {
                    TextC(wx+ww/2, wy+20, "SCAN RESULT", 0xFFFFFF, _128);
                    _15(key_new AND win_z[10] EQ 7 AND wifi_state EQ 2) { 
                        _15(last_key EQ 0x0E AND wifi_pass_idx > 0) wifi_pass[--wifi_pass_idx] = 0; _41 _15(wifi_pass_idx < 30) { _30 c = get_ascii(last_key); _15(c >= 32) { wifi_pass[wifi_pass_idx++] = c; wifi_pass[wifi_pass_idx]=0; } }
                    }
                    _15(wifi_state EQ 0) { TextC(wx+ww/2, wy+100, "SCANNING AIR...", 0xAAAAAA, _128); _15(frame % 50 EQ 0) wifi_state = 1; } 
                    _41 _15(wifi_state EQ 1) { _39(_43 i=0; i<4; i++) { _43 y_off = wy+60 + (i*30); _44 hov = is_over(mouse_x, mouse_y, wx+ww/2, y_off+10, 100); DrawRoundedRect(wx+20, y_off, ww-40, 25, 5, hov ? 0x999999 : 0x555555); Text(wx+30, y_off+5, wifi_ssids[i], 0xFFFFFF, _128); _15(mouse_just_pressed AND !blocked AND hov) { wifi_selected = i; wifi_state = 2; } } } 
                    _41 _15(wifi_state EQ 2) { 
                        Text(wx+30, wy+60, "ENTER PASSWORD:", 0xAAAAAA, _128); DrawRoundedRect(wx+30, wy+80, ww-60, 30, 0, 0xFFFFFF); Text(wx+35, wy+85, wifi_pass, 0x000000, _128); 
                        _43 btn_y = wy+130; _44 hov = is_over(mouse_x, mouse_y, wx+ww/2, btn_y+15, 60); 
                        DrawRoundedRect(wx+ww/2-50, btn_y, 100, 30, 5, hov ? 0x999999 : 0x333333); TextC(wx+ww/2, btn_y+7, "CONNECT", 0xFFFFFF, _128); 
                        _15(mouse_just_pressed AND !blocked AND hov) { wifi_state = 3; send_udp(0xFFFFFFFF, 5000, 80, "COSMOS HELLO"); } 
                    }
                    _41 _15(wifi_state EQ 3) { 
                        TextC(wx+ww/2, wy+100, "CONNECTED!", 0x000000, _128); 
                        TextC(wx+ww/2, wy+120, wifi_ssids[wifi_selected], 0xAAAAAA, _128); 
                        _15(frame % 60 EQ 0) str_cpy(cmd_status, "ONLINE (SECURE ID 84)");
                    }
                } 

                DrawTriangle(wx+ww-15, wy+wh, wx+ww, wy+wh-15, wx+ww, wy+wh, 0x888888); 
            } 

            _15(!z_blocked AND mouse_just_pressed AND !is_modal_blocked AND !click_consumed AND is_over(mouse_x, mouse_y, cx, cy, 40)) 
                galaxy_open = !galaxy_open;

        } 

        _39(_43 i=-2;i<=2;i++){ Put(mouse_x+i,mouse_y,0xFF0000); Put(mouse_x,mouse_y+i,0xFF0000); }
        Swap(); frame++; 
    } 
}