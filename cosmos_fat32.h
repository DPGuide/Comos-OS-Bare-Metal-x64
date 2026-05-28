#ifndef COSMOS_FAT32_H
#define COSMOS_FAT32_H

#include <stdint.h>
#include "schneider_lang.h"

_202 FAT32_BPB {
    uint8_t jump[3];
    char oem[8];
    uint16_t bytes_per_sector;
    uint8_t sectors_per_cluster;
    uint16_t reserved_sectors;
    uint8_t fat_count;
    uint16_t dir_entries;
    uint16_t total_sectors_16;
    uint8_t media_descriptor;
    uint16_t sectors_per_fat_16;
    uint16_t sectors_per_track;
    uint16_t heads;
    uint32_t hidden_sectors;
    uint32_t total_sectors_32;
    uint32_t sectors_per_fat_32;
    uint16_t ext_flags;
    uint16_t fs_version;
    uint32_t root_cluster;
    uint16_t fs_info;
    uint16_t backup_boot_sector;
    uint8_t reserved[12];
    uint8_t drive_number;
    uint8_t nt_flags;
    uint8_t signature;
    uint32_t serial_number;
    char label[11];
    char sys_id[8];
    uint8_t boot_code[420];
    uint16_t boot_signature;
} __attribute__((packed));

_202 FAT32_DirectoryEntry {
    char name[11];
    uint8_t attr;
    uint8_t nt_res;
    uint8_t crt_time_tenth;
    uint16_t crt_time;
    uint16_t crt_date;
    uint16_t lst_acc_date;
    uint16_t fst_clus_hi;
    uint16_t wrt_time;
    uint16_t wrt_date;
    uint16_t fst_clus_lo;
    uint32_t size;
} __attribute__((packed));

_202 FAT32_LFN_Entry {
    uint8_t order;
    uint16_t name1[5];
    uint8_t attr;
    uint8_t type;
    uint8_t checksum;
    uint16_t name2[6];
    uint16_t fst_clus_lo;
    uint16_t name3[2];
} __attribute__((packed));

_202 FAT32_ParsedFile {
    uint8_t exists;
    char name[64];
    uint32_t size;
    uint32_t start_lba;
    uint8_t is_folder;
    uint8_t parent_idx;
};

_172 _43 fat32_data_start;
_172 _43 fat32_sectors_per_cluster;
_172 _43 fat32_root_lba;

_172 _44 fat32_init(_43 partition_lba, uint8_t* bpb_buffer);
_172 _44 fat32_list_dir(_43 folder_lba, uint8_t* dir_buffer, FAT32_ParsedFile* output_files, _43 max_files, uint8_t current_folder_id, int current_page_offset);

#endif
