#include "cosmos_fat32.h"

_43 fat32_data_start = 0;
_43 fat32_sectors_per_cluster = 0;
_43 fat32_root_lba = 0;

_44 fat32_init(_43 partition_lba, uint8_t* bpb_buffer) {
    FAT32_BPB* bpb = (FAT32_BPB*)bpb_buffer;
    
    /// Basic validation
    _15(bpb->bytes_per_sector != 512) return _86;
    
    fat32_sectors_per_cluster = bpb->sectors_per_cluster;
    
    uint32_t fat_start = bpb->reserved_sectors;
    fat32_data_start = fat_start + (bpb->fat_count * bpb->sectors_per_fat_32);
    
    fat32_root_lba = fat32_data_start + ((bpb->root_cluster - 2) * fat32_sectors_per_cluster);
    
    /// Offset by the partition start LBA
    fat32_data_start += partition_lba;
    fat32_root_lba += partition_lba;
    
    return _128;
}

_44 fat32_list_dir(_43 folder_lba, uint8_t* dir_buf, FAT32_ParsedFile* output_files, _43 max_files, uint8_t current_folder_id, int current_page_offset) {
    FAT32_DirectoryEntry* dir = (FAT32_DirectoryEntry*)dir_buf;
    
    _39(int f=0; f < max_files; f++) {
        output_files[f].exists = 0;
        output_files[f].name[0] = 0;
    }
    
    int file_idx = 0;
    int skipped = 0;
    char lfn_buf[256];
    _39(int i=0; i<256; i++) lfn_buf[i] = 0;
    
    _39(int i=0; i < 512 / sizeof(FAT32_DirectoryEntry); i++) {
        _15(file_idx >= max_files) _37;
        
        _15(dir[i].name[0] == 0x00) _37; /// End of directory
        _15((uint8_t)dir[i].name[0] == 0xE5) {
            _39(int n=0; n<256; n++) lfn_buf[n] = 0;
            _101; /// Deleted file
        }
        
        _15(dir[i].attr == 0x0F) {
            /// Long File Name (LFN) entry
            FAT32_LFN_Entry* lfn = (FAT32_LFN_Entry*)&dir[i];
            int order = lfn->order & 0x3F;
            int offset = (order - 1) * 13;
            
            _15(offset >= 0 AND offset < 200) {
                lfn_buf[offset+0] = lfn->name1[0]; lfn_buf[offset+1] = lfn->name1[1];
                lfn_buf[offset+2] = lfn->name1[2]; lfn_buf[offset+3] = lfn->name1[3];
                lfn_buf[offset+4] = lfn->name1[4];
                
                lfn_buf[offset+5] = lfn->name2[0]; lfn_buf[offset+6] = lfn->name2[1];
                lfn_buf[offset+7] = lfn->name2[2]; lfn_buf[offset+8] = lfn->name2[3];
                lfn_buf[offset+9] = lfn->name2[4]; lfn_buf[offset+10] = lfn->name2[5];
                
                lfn_buf[offset+11] = lfn->name3[0]; lfn_buf[offset+12] = lfn->name3[1];
            }
        } _41 {
            /// Standard 8.3 entry
            _15(dir[i].attr & 0x08) {
                _39(int n=0; n<256; n++) lfn_buf[n] = 0;
                _101; /// Volume Label (skip)
            }
            
            if (skipped < current_page_offset) {
                skipped++;
                _39(int n=0; n<256; n++) lfn_buf[n] = 0;
                _101;
            }
            
            _15(file_idx < max_files) {
                output_files[file_idx].exists = 1;
                output_files[file_idx].parent_idx = current_folder_id;
                output_files[file_idx].is_folder = (dir[i].attr & 0x10) ? 1 : 0;
                output_files[file_idx].size = dir[i].size;
                
                uint32_t fcluster = (dir[i].fst_clus_hi << 16) | dir[i].fst_clus_lo;
                output_files[file_idx].start_lba = fat32_data_start + ((fcluster - 2) * fat32_sectors_per_cluster);
                
                _15(lfn_buf[0] != 0) {
                    _39(int n=0; n<63; n++) {
                        char c = lfn_buf[n];
                        output_files[file_idx].name[n] = (c >= 32 && c <= 126) ? c : 0;
                    }
                    output_files[file_idx].name[63] = 0;
                    _39(int n=0; n<256; n++) lfn_buf[n] = 0; /// Reset LFN buffer
                } _41 {
                    _39(int n=0; n<11; n++) {
                        char c = dir[i].name[n];
                        output_files[file_idx].name[n] = (c >= 32 && c <= 126) ? c : 0;
                    }
                    output_files[file_idx].name[11] = 0;
                }
                
                file_idx++;
            }
        }
    }
    
    return _128;
}
