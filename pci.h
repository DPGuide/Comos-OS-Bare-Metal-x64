#ifndef PCI_H
#define PCI_H

#include <stdint.h>
#include "schneider_lang.h"

// Struktur aus kernel.cpp hierher verschoben
_202 NICInfo { _30 name[32]; _89 address; _43 type; };

// Globale Funktionen
_172 _89 pci_read(_184 bus, _184 slot, _184 func, _184 offset);
_172 _50 pci_scan_all();

#endif