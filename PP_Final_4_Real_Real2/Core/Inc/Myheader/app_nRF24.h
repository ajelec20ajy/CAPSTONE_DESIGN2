/*
 * app_nRF24.h
 *
 *  Created on: Jan 30, 2026
 *      Author: ajy97
 */

#ifndef SRC_APP_NRF24_H_
#define SRC_APP_NRF24_H_

#include "main.h"
#include <stdint.h>
#include <string.h>
#include "types.h"
// 데이터 구조체 정의
#pragma pack(push, 1)
typedef struct {
    int16_t cds_value;
    Waypoint_t path[5]; // 65 bytes
} TotalData_t; // 67 bytes
#pragma pack(pop)

extern TotalData_t g_node_data[6];      // 파이프별 최종 데이터
extern uint8_t g_rx_complete_flag[6];   // 수신 완료 플래그

/* --- 함수 원형 --- */
void APP_NRF24_Init(void);
void APP_NRF24_WaitForData(uint32_t timeout_ms);


#endif /* SRC_APP_NRF24_H_ */
