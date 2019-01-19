#pragma once
// if any of below interval fufils:
//		(interval / QUEUE_DATA_INTERVAL) % 100 = 0 
// then it is valid. Otherwise execution may be delayed sometimes.
#define FUELCELL_LOGSEND_INTERVAL 1500 // must be higher than 1000ms, meaningless if send faster than fuel cell payload coming in
#define MOTOR_LOGSEND_INTERVAL 600 // enables furious plotting on PC side, try not to overtax the XBee though
#define BACK_LCD_REFRESH_INTERVAL 2*MOTOR_LOGSEND_INTERVAL // can be slower, won't look at it often
#define SPEEDOMETER_REFRESH_INTERVAL 600 // 0.8s refresh rate

// Declare task intervals
#define READ_FC_INTERVAL 345 // do not anyhow change, fuel cell payload comes once per 1000ms, and faster read as sometimes need 2 reads to process
#define READ_MT_INTERVAL 300
#define QUEUE_DATA_INTERVAL 150 // keep small, fine tune in definitions above
#define LOGSEND_INTERVAL min(MOTOR_LOGSEND_INTERVAL, FUELCELL_LOGSEND_INTERVAL)
#define DISPLAY_INTERVAL QUEUE_DATA_INTERVAL