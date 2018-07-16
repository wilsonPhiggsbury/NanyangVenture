#pragma once

// file names
const char FUELCELL_FILENAME[] = "FC.txt";
const char MASTER_FUELCELL_RAW_FILENAME[] = "FC_M.txt";
const char SLAVE_FUELCELL_RAW_FILENAME[] = "FC_S.txt";
const char MOTOR_FILENAME[] = "MT_CAP.txt";
const uint8_t FILENAME_HEADER_LENGTH = 1 + 8 + 1;
// sample filename: /LOG_0002/12345678.txt   1+8+1+8+4+1, 
//											  ^^^ | ^^^
//										   HEADER | FILENAME

// declare globals (please keep to minimum)
extern bool SD_avail;
extern char path[FILENAME_HEADER_LENGTH + 8 + 4 + 1]; // +8 for filename, +4 for '.txt', +1 for '\0'