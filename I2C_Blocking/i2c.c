/*
 * i2c.c
 *
 *  Created on: Aug 21, 2025
 *      Author: ajy97
 */

#include "i2c.h"
extern I2C_HandleTypeDef hi2c1;
extern UART_HandleTypeDef huart2;

void I2C_Scan(void)
{
    HAL_StatusTypeDef result;
    uint8_t i;
    char msg[50];

    for (i = 1; i < 128; i++) // 7-bit 주소 (0x01 ~ 0x7F)
    {
        result = HAL_I2C_IsDeviceReady(&hi2c1, (uint16_t)(i << 1), 2, 10);
        if (result == HAL_OK)
        {
            sprintf(msg, "I2C device found at address 0x%02X\r\n", i);
            HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg),HAL_MAX_DELAY);
        }
    }
}
// 사용법 : I2C1_Start에 슬레이브주소와 읽기/쓰기를 실어서 보낸다. 그리고 Write나 Read 함수를 호출한다.
void I2C1_Start(uint8_t address, uint8_t direction){ // SR2-BUSY -> CR1 START -> SR1-SB -> DR=ADDRESS+R/W -> SR1-ADDR  -> (void)I2C1->SR2
    I2C1->CR1 |= I2C_CR1_START; // Start 비트에 1을 쓰면 start 조건을 생성하라는 명령이 만들어져서 스타트 조건을 형성하고, SR1의 Start 비트가 1로 세트된다.
    while(!(I2C1->SR1 & I2C_SR1_SB)); // 스타트조건 형성 완료되면 SR1의 Start Bit가 세트된다.
    I2C1->DR = (address << 1) | direction; // 슬레이브주소와 명령(R/W)을 실어서 보낸다. addrss = slave 주소, direction 1이면 read할거라고 알려줌, 0이면 write할거라고 알려줌
    int timeout = 10000;
    while (!(I2C1->SR1 & I2C_SR1_ADDR));
    (void)I2C1->SR2;
}

void I2C1_Stop(){
	I2C1->CR1 |= I2C_CR1_STOP; // STOP 조건 발생.
	while(I2C1->SR2 & I2C_SR2_BUSY);
}
void I2C1_Send(uint8_t regAddr){
	while(!(I2C1->SR1 & I2C_SR1_TXE)); // DR에 데이터가 이미 존재하면, TXE=0이고 DR이 비워지면 TXE = 1이 된다.
	I2C1->DR = regAddr; // 쓸 레지스터의 주소
}

void I2C1_WriteData(uint8_t data){ //기본 1바이트만 전송하기
	while(!(I2C1->SR1 & I2C_SR1_TXE)); //DR을 넘어선 쉬프트까지 체크
	I2C1->DR = data;
}
uint8_t I2C1_ReadDataAck(){
	I2C1->CR1 |= I2C_CR1_ACK;
	while(!(I2C1->SR1 & I2C_SR1_RXNE));
	return (uint8_t)I2C1->DR;
}

uint8_t I2C1_ReadDataNack(){
    I2C1->CR1 &= ~(I2C_CR1_ACK); // 마지막 바이트 → NACK
    I2C1_Stop();
    while(!(I2C1->SR1 & I2C_SR1_RXNE));
    return (uint8_t)I2C1->DR;
}
void I2C1_WriteReg(uint8_t addr, uint8_t reg, uint8_t data){
    I2C1_Start(addr, I2C_WRITE);
    I2C1_WriteData(reg);
    I2C1_WriteData(data);
    while(!(I2C1->SR1 & I2C_SR1_TXE));
    I2C1_Stop();
}

uint8_t I2C1_ReadReg(uint8_t addr, uint8_t reg){
    uint8_t val;
    // Register Address 전송
    I2C1_Start(addr, I2C_WRITE);
    I2C1_WriteData(reg);

    // Restart 후 Read
    I2C1_Start(addr, I2C_READ);
    val = I2C1_ReadDataNack();
    return val;
}

void I2C1_ReadMulti(uint8_t addr, uint8_t reg, uint8_t *buf, uint8_t len){
    // Register Address 전송
    I2C1_Start(addr, I2C_WRITE);
    I2C1_WriteData(reg);

    // Restart 후 Read
    I2C1_Start(addr, I2C_READ);

    for(uint8_t i=0; i<len; i++){
        if(i < (len-1)) buf[i] = I2C1_ReadDataAck();
        else buf[i] = I2C1_ReadDataNack();
    }
}

extern I2C_HandleTypeDef hi2c1;   // 사용 중인 I2C 핸들러

void delay_us(uint32_t us) {
    for (uint32_t i = 0; i < us * (SystemCoreClock / 8000000U); i++) {
        __NOP();
    }
}


void I2C_Bus_Recovery(I2C_HandleTypeDef *hi2c_handle) // hi2c1 대신 핸들러를 파라미터로 받는 것을 권장
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    // 1) I2C 주변장치 비활성화
    __HAL_I2C_DISABLE(hi2c_handle); // hi2c1 대신 전달받은 핸들러 사용

    // 2) SCL, SDA 핀을 GPIO Output/Open-drain 모드로 재설정
    // SDA 핀 (예: PB7)
    GPIO_InitStruct.Pin = GPIO_PIN_7; // 사용자 환경에 맞게 핀 번호 설정
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_InitStruct.Pull = GPIO_NOPULL; // 외부 풀업 저항 사용
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct); // 사용자 환경에 맞게 포트 설정

    // SCL 핀 (예: PB6)
    GPIO_InitStruct.Pin = GPIO_PIN_6; // 사용자 환경에 맞게 핀 번호 설정
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct); // 사용자 환경에 맞게 포트 설정

    // 3) SCL을 9번 토글 (슬레이브 내부 시프트 레지스터 비움)
    // SDA 라인이 LOW에 고정되어 있다면 이 작업 전에 SCL을 Low로 한번 보내야 함.
    // 대부분의 경우 SDA는 slave에 의해 held down 될 수 있으므로,
    // SCL을 수동으로 구동하기 전에 SCL을 HIGH로 한 다음 LOW로 보내는 것이 좋습니다.
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET);   // SCL High로 시작
    delay_us(20); // 짧은 딜레이

    for (int i = 0; i < 9; i++) {
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET); // SCL Low
        delay_us(20);
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET);   // SCL High
        delay_us(20);
    }

    // 4) STOP 조건 강제 발생: SDA High, SCL High
    // STOP 조건은 SCL이 HIGH일 때 SDA가 LOW에서 HIGH로 변하는 것입니다.
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET); // SDA Low
    delay_us(20);
    // SCL은 위에서 High로 설정된 상태를 유지
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_SET);   // SDA High (STOP 조건 생성)
    delay_us(20);

    // 5) SCL, SDA를 다시 I²C Alternate Function으로 복귀
    GPIO_InitStruct.Pin = GPIO_PIN_6 | GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    // 나머지 Pull, Speed 설정은 I2C 초기화 설정에 따라 변경될 수 있음
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    // 6) I2C Peripheral 소프트리셋 (옵션: 아래 HAL_I2C_Init으로 대체 가능)
    hi2c_handle->Instance->CR1 |= I2C_CR1_SWRST; // 소프트웨어 리셋 설정
    __NOP(); // No-operation
    hi2c_handle->Instance->CR1 &= ~I2C_CR1_SWRST; // 소프트웨어 리셋 해제

    // 7) I2C 주변장치 재초기화
    HAL_I2C_Init(hi2c_handle);
}
