#pragma once

// CAN IDs are 11 bits long

// first 4 bits are used to indicate CAN message purpose

// from NV10_back: last 7 bits indicates message index, string max length = 127
#define NV_CAN_BACK 0x004 // init ID
//#define NV_CAN_EOL 0x003