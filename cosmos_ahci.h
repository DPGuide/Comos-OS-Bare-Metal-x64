#ifndef COSMOS_AHCI_H
#define COSMOS_AHCI_H

#include <stdint.h>
#include "schneider_lang.h"

#define SATA_SIG_ATA   0x00000101
#define SATA_SIG_ATAPI 0xEB140101

// --- 1. ZUERST DIE STRUKTUREN ---
_81 _202 {
    _89 clb;        _89 clbu;       _89 fb;         _89 fbu;
    _89 is;         _89 ie;         _89 cmd;        _89 reserved0;
    _89 tfd;        _89 sig;        _89 ssts;       _89 sctl;
    _89 serr;       _89 sact;       _89 ci;         _89 sntf;
    _89 fbs;        _89 reserved1[11]; _89 vendor[4];
} HBA_PORT;

_81 _202 {
    _89 cap;        _89 ghc;        _89 is;         _89 pi;
    _89 vs;         _89 ccc_ctl;    _89 ccc_pts;    _89 em_loc;
    _89 em_ctl;     _89 cap2;       _89 bohc;       _89 reserved[29];
    _89 vendor[15]; HBA_PORT ports[32];
} HBA_MEM;

_81 _202 {
    _89 dba;        _89 dbau;       _89 reserved0;  _89 dbc;
} HBA_PRDT_ENTRY;

_81 _202 {
    _184 cfis[64];   _184 acmd[16];   _184 reserved[48];
    HBA_PRDT_ENTRY prdt_entry[1]; 
} HBA_CMD_TBL;

_81 _202 {
    _184 cfl:5;      _184 a:1;        _184 w:1;        _184 p:1;
    _184 r:1;        _184 b:1;        _184 c:1;        _184 reserved0:1;
    _184 pmp:4;      _182 prdtl;      _89 prdbc;      _89 ctba;
    _89 ctbau;      _89 reserved1[4];
} HBA_CMD_HEADER;

_81 _202 {
    _184 fis_type;   _184 pmport:4;   _184 rsv0:3;     _184 c:1;
    _184 command;    _184 featurel;   _184 lba0;       _184 lba1;
    _184 lba2;       _184 device;     _184 lba3;       _184 lba4;
    _184 lba5;       _184 featureh;   _184 countl;     _184 counth;
    _184 icc;        _184 control;    _184 rsv1[4];
} FIS_REG_H2D;

// --- 2. DANN ERST DIE FUNKTIONEN ---
_172 _50 ahci_init(_89 abar_address);
_172 _44 ahci_read(HBA_PORT *port, _89 startlba, _50 *target_ram_address);
_172 _44 ahci_write(HBA_PORT *port, _89 startlba, _50 *source_ram_address);
_172 HBA_PORT* get_active_ahci_port();
_172 _50 ahci_get_raw_identify(_43 port_no, _182* out_buffer);

#endif