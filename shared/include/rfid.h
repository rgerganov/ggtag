#ifndef __RFID_H__
#define __RFID_H__

#include <stdint.h>

bool program_em_rfid(uint8_t mfr_id, uint32_t uid);

bool program_hid_rfid(uint16_t id1, uint32_t id2);

#endif