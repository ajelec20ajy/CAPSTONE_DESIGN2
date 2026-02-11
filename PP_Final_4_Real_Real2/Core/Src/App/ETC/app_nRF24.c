/*
 * app_nRF24.c
 *
 *  Created on: Jan 30, 2026
 *      Author: ajy97
 */
#include "app_nRF24.h"
#include "bsp_nRF24.h"

/* 전역 변수 정의 */
TotalData_t g_node_data[6];
uint8_t g_rx_complete_flag[6] = {0};

/* 내부 버퍼 관리용 변수 (static으로 숨김) */
static uint8_t rx_raw_buffers[6][96];
static int rx_byte_counts[6] = {0};

void APP_NRF24_Init(void) {
    // 하드웨어 레벨 초기화 호출
	NRF24_Init();

    // 변수 초기화
    memset(g_node_data, 0, sizeof(g_node_data));
    memset(g_rx_complete_flag, 0, sizeof(g_rx_complete_flag));
}

void APP_NRF24_WaitForData(uint32_t timeout_ms) {
    uint32_t start_tick = HAL_GetTick();

    // 타임아웃 시간 동안 반복
    while ((HAL_GetTick() - start_tick) < timeout_ms) {

        // 데이터가 도착했는지 BSP에 확인
        if (NRF24_DataReady()) {
            uint8_t pipe = NRF24_GetPipeNum();

            // 유효한 파이프(0~5)인 경우만 처리
            if (pipe < 6) {
                uint8_t temp[32];
                NRF24_Receive(temp);

                // 필요한 만큼만 버퍼에 쌓기 (67바이트)
                if (rx_byte_counts[pipe] < 67) {
                    memcpy(&rx_raw_buffers[pipe][rx_byte_counts[pipe]], temp, 32);
                    rx_byte_counts[pipe] += 32;
                }

                // 67바이트 이상 모이면 조립 완료
                if (rx_byte_counts[pipe] >= 67) {
                    memcpy(&g_node_data[pipe], rx_raw_buffers[pipe], sizeof(TotalData_t));

                    // 상태 업데이트
                    rx_byte_counts[pipe] = 0;       // 카운터 리셋
                    g_rx_complete_flag[pipe] = 1;   // 수신 완료 플래그 설정
                }
            }
            else {
                // 알 수 없는 파이프 데이터는 버림
                uint8_t trash[32];
                NRF24_Receive(trash);
            }
        }
    }
}

