#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

#include "i2c.h"

#define RTC_ADDR 32
#define KHZ 13
#define EN_5V 14
#define EN_CAM 15

#define EN_DEC 6
#define STEP_DEC 7
#define DIR_DEC 8

#define EN_EQ 10
#define STEP_EQ 11
#define DIR_EQ 12

#define EN_FC 20
#define STEP_FC 19
#define DIR_FC 18

#define TRK_DIR 1
#define STEP_SLEEP_US 10



#define SMALL_CT_LOOPS 776
#define SMALL_CT_PULSES 1485

#define LG_CT_LOOPS 724
#define LG_CT_PULSES 1486

int currentInterrupts = 0;
bool onLGLoop = false;
int loops = 0;

bool trackingEnabled = true;


int initializeIo(){
	int controlPins[] = {EN_5V, EN_CAM, EN_DEC, STEP_DEC, DIR_DEC, EN_EQ, STEP_EQ, DIR_EQ, EN_FC, STEP_FC, DIR_FC};
	int numPins  = sizeof(controlPins) / sizeof(controlPins[0]);
	for(int i = 0; i < numPins; i++){
		gpio_init(controlPins[i]);
		gpio_set_dir(controlPins[i], GPIO_OUT);
	}
	return 0;
		
}


int moveDecAxis(bool dir, long int steps){
	gpio_put(EN_DEC, 1);
	gpio_put(DIR_DEC, dir);
	for(long int i = 0; i < steps; i++){
		gpio_put(STEP_DEC, 1);
		busy_wait_us_32(STEP_SLEEP_US);
		gpio_put(STEP_DEC, 0);
	}
	gpio_put(EN_DEC, 0);
	return 0;
}
int moveEQAxis(bool dir, long int steps){
	gpio_put(EN_EQ, 1);
	gpio_put(DIR_EQ, dir);
	for(long int i = 0; i < steps; i++){
		gpio_put(STEP_EQ, 1);
		busy_wait_us_32(STEP_SLEEP_US);
		gpio_put(STEP_EQ, 0);
	}
	gpio_put(EN_EQ, 0);
	return 0;
}
int moveFocAxis(bool dir, long int steps){
	gpio_put(EN_FC, 1);
	gpio_put(DIR_EQ, dir);
	for(long int i = 0; i < steps; i++){
		gpio_put(STEP_FC, 1);
		busy_wait_us_32(STEP_SLEEP_US);
		gpio_put(STEP_FC, 0);
	}
	gpio_put(EN_FC, 0);
	return 0;
}

void callBack(uint gpio, uint32_t events){
	if(trackingEnabled){
		currentInterrupts += 1;
		if(onLGLoop){
			if(currentInterrupts == LG_CT_PULSES-1){
				currentInterrupts = 0;
				moveEQAxis(TRK_DIR, 1);
				loops += 1;
				if(loops == LG_CT_LOOPS-1){
					onLGLoop = false;
					loops = 0;
					printf("LoopReset/n");
				}
			}
		}else{
			if(currentInterrupts == SMALL_CT_PULSES -1){
				currentInterrupts = 0;
				moveEQAxis(TRK_DIR, 1);
				loops +=1;
				if(loops == SMALL_CT_LOOPS -1){
					loops = 0;
					onLGLoop = true;
					printf("LoopReset \n");
				}
			}
		}
	}
}

int main(){
	stdio_init_all();
	printf("Initializing...\n");
	initializeI2C();
	initializeIo();
	gpio_set_irq_enabled_with_callback(KHZ, 0x04, 1, & callBack);


	uint8_t addr = 0x20;
	uint8_t rxdata[2] = {0x00, 0x00};
	
	while(1){
		//scanI2CBus();
		sleep_ms(1000);
	}	


    }

