/*
 * nRF24.c
 *
 *  Created on: Jan 30, 2026
 *      Author: ajy97
 */
#include "bsp_nRF24.h"

extern SPI_HandleTypeDef hspi3;

#define CSN_LOW()   HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET)
#define CSN_HIGH()  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET)
#define CE_LOW()    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11, GPIO_PIN_RESET)
#define CE_HIGH()   HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11, GPIO_PIN_SET)

void NRF24_WriteReg(uint8_t reg, uint8_t value) {
    uint8_t buf[2] = {W_REGISTER | (reg & 0x1F), value};
    CSN_LOW();
    HAL_SPI_Transmit(&hspi3, buf, 2, 100);
    CSN_HIGH();
}

uint8_t NRF24_ReadReg(uint8_t reg) {
    uint8_t tx = R_REGISTER | (reg & 0x1F);
    uint8_t rx = 0;
    CSN_LOW();
    HAL_SPI_Transmit(&hspi3, &tx, 1, 100);
    HAL_SPI_Receive(&hspi3, &rx, 1, 100);
    CSN_HIGH();
    return rx;
}

// 6개 파이프 초기화 함수
void NRF24_Init(void) {
    CE_LOW();

    NRF24_WriteReg(CONFIG, 0x0F);    // RX 모드, CRC 2bytes
    NRF24_WriteReg(EN_AA, 0x00);     // Auto Ack 끄기 (수신 전용)

    // [중요] Pipe 0 ~ 5 모두 활성화 (비트 0~5 = 1) -> 0x3F (0011 1111)
    NRF24_WriteReg(EN_RXADDR, 0x3F);

    NRF24_WriteReg(RF_CH, 0x02);     // 채널 2 (2.402 GHz)
    NRF24_WriteReg(RF_SETUP, 0x06);  // 1Mbps, 0dBm

    // 각 파이프별 페이로드 크기 (모두 32바이트)
    NRF24_WriteReg(RX_PW_P0, 32);
    NRF24_WriteReg(RX_PW_P1, 32);
    NRF24_WriteReg(RX_PW_P2, 32);
    NRF24_WriteReg(RX_PW_P3, 32);
    NRF24_WriteReg(RX_PW_P4, 32);
    NRF24_WriteReg(RX_PW_P5, 32);

    // --- 주소 설정 ---
    // Pipe 0 주소 (LSB First): 0xE7E7E7E700
    uint8_t addr_p0[5] = {0x00, 0xE7, 0xE7, 0xE7, 0xE7};
    CSN_LOW();
    HAL_SPI_Transmit(&hspi3, (uint8_t[]){W_REGISTER | RX_ADDR_P0}, 1, 100);
    HAL_SPI_Transmit(&hspi3, addr_p0, 5, 100);
    CSN_HIGH();

    // Pipe 1 주소 (Base Address): 0xC2C2C2C201
    uint8_t addr_p1[5] = {0x01, 0xC2, 0xC2, 0xC2, 0xC2};
    CSN_LOW();
    HAL_SPI_Transmit(&hspi3, (uint8_t[]){W_REGISTER | RX_ADDR_P1}, 1, 100);
    HAL_SPI_Transmit(&hspi3, addr_p1, 5, 100);
    CSN_HIGH();

    // Pipe 2~5 주소 (마지막 1바이트만 설정)
    NRF24_WriteReg(RX_ADDR_P2, 0x02); // Full addr: ...02
    NRF24_WriteReg(RX_ADDR_P3, 0x03); // Full addr: ...03
    NRF24_WriteReg(RX_ADDR_P4, 0x04); // Full addr: ...04
    NRF24_WriteReg(RX_ADDR_P5, 0x05); // Full addr: ...05

    CE_HIGH();
}

uint8_t NRF24_DataReady(void) {
    if (NRF24_ReadReg(STATUS) & 0x40) return 1;
    return 0;
}

// 데이터를 보낸 파이프 번호 반환 (0~5)
uint8_t NRF24_GetPipeNum(void) {
    uint8_t status = NRF24_ReadReg(STATUS);
    // 상태 레지스터의 [3:1] 비트가 파이프 번호
    return (status >> 1) & 0x07;
}

void NRF24_Receive(uint8_t *pBuf) {
    uint8_t cmd = R_RX_PAYLOAD;
    CSN_LOW();
    HAL_SPI_Transmit(&hspi3, &cmd, 1, 100);
    HAL_SPI_Receive(&hspi3, pBuf, 32, 100);
    CSN_HIGH();
    NRF24_WriteReg(STATUS, 0x40); // RX_DR 클리어
}
