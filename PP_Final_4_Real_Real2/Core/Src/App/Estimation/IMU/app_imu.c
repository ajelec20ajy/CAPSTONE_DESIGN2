/*
 * imu.c
 *
 *  Created on: Nov 1, 2025
 *      Author: ajy97
 */
#include <main.h> // i2c헤더랑 핀 할당
#include <cmsis_os2.h> // osThread 쓰니
#include <math.h> // 수학 연산등. 굳이 필요없는데 혹몰
#include "app_i2c_callback.h" // i2c dma 콜백
#include "bsp_acc.h" // 가속도
#include "bsp_gyro.h" // 자이로
#include "bsp_mag.h" // 지자기
#include "types.h" // 구조체들
#include "app_filter.h" // CF, 기울어짐 보상

#define MAG_PERIOD 5 // Mag 카운터(느리게 읽으니깐)

IMU_Handle_t hIMU; // IMU 구조체

volatile uint8_t flag_dma_done = 0;
static uint8_t Mag_Period_Cnt = 0;

float Get_Fused_Heading() { //sensorTask에서 실행되는, 헤딩 추종하기. 자이로+지자기(기울기보상-가속도) 기반 CF
	Read_Gyro(); // 자이로 raw 긁어오기
	osThreadFlagsWait(FLAG_DMA_DONE, osFlagsWaitAny, 5); // 자이로 다 가져오는거 기다리기.

	Read_Accel(); // 가속도 raw 긁어오기
	Gyro_ProcessData(&hIMU); // 가속도 DMA가 I2C로 가져오는 동안 자이로 오프셋 처리 후 Heading 산출
	osThreadFlagsWait(FLAG_DMA_DONE, osFlagsWaitAny, 5);

	if(Mag_Period_Cnt++ >= MAG_PERIOD) { // 지자기는 느리게 샘플링레이트 되어있으니깐 굳이 100hZ로 읽을 필요없
		Mag_Period_Cnt = 0;
		Read_Mag();      // 지자기 가져와!
		Accel_ProcessData(&hIMU); // DMA 하는 동안 아까 받아온 가속도값 보정
		osThreadFlagsWait(FLAG_DMA_DONE, osFlagsWaitAny, 5);// 지자기 다 올 때까지 대기

		Mag_ProcessData(&hIMU);   // 지자기 계산
	}
	else {
		Accel_ProcessData(&hIMU); // 지자기 읽어올 떄 아니면 가속도 연산
	}

	Tilt_Compensation(&hIMU); // 가속도 센서를 활용해 Heading 기울어짐 보상해서 각도 산출
	Complemetary_Filter(&hIMU); // 자이로 + 지자기 Heading CF 융합
	return hIMU.yaw_rad;
}

void Init_Sensors(){ // 센서 레지스터 초기 설정 및 캘리브레이션
	Init_Accel(); // 가속도 초기화 및 오프셋
	Init_Gyro(); // 자이로 초기화 및 오프셋
	HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET); // 자 이제 준비하시고
	HAL_Delay(3000); // Mag는 8자 회전 시켜가면서 하드/소프트아이언 보정해야되서 잠깐 기다렸다가 하자.
	Init_Mag(); // 지자기 초기화 및 소프트/하드 아이언 보정값 구하기
	HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
}

