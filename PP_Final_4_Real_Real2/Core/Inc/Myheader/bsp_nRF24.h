/*
 * nRF24.h
 *
 *  Created on: Jan 30, 2026
 *      Author: ajy97
 */

#ifndef SRC_BSP_NRF24_H_
#define SRC_BSP_NRF24_H_

#include "main.h"
#include <string.h>

// 레지스터 주소
#define CONFIG      0x00
#define EN_AA       0x01
#define EN_RXADDR   0x02
#define SETUP_AW    0x03
#define SETUP_RETR  0x04
#define RF_CH       0x05
#define RF_SETUP    0x06
#define STATUS      0x07
#define OBSERVE_TX  0x08
#define RPD         0x09
#define RX_ADDR_P0  0x0A
#define RX_ADDR_P1  0x0B
#define RX_ADDR_P2  0x0C
#define RX_ADDR_P3  0x0D
#define RX_ADDR_P4  0x0E
#define RX_ADDR_P5  0x0F
#define TX_ADDR     0x10
#define RX_PW_P0    0x11
#define RX_PW_P1    0x12
#define RX_PW_P2    0x13
#define RX_PW_P3    0x14
#define RX_PW_P4    0x15
#define RX_PW_P5    0x16

// 명령어
#define R_REGISTER    0x00
#define W_REGISTER    0x20
#define R_RX_PAYLOAD  0x61
#define W_TX_PAYLOAD  0xA0
#define FLUSH_TX      0xE1
#define FLUSH_RX      0xE2
#define NOP           0xFF

// 함수 원형
void NRF24_Init(void);
uint8_t NRF24_DataReady(void);
uint8_t NRF24_GetPipeNum(void); // 수신된 파이프 번호 반환
void NRF24_Receive(uint8_t *pBuf);

#endif /* SRC_BSP_NRF24_H_ */
