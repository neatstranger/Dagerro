#include <stdio.h>
#include <stdlib.h>
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
int char_array_to_int(const char *chars, int length) {
    int result = 0;
    int is_negative = 0;
    int start = 0;

    // Check if the number is negative
    if (chars[0] == '-') {
        is_negative = 1;
        start = 1; // Start from the next character
    }

    for (int i = start; i < length; i++) {
        // Shift the result by 1 decimal place (multiply by 10)
        result = (result << 3) + (result << 1); // result * 10 using bit shifting

        // Convert the current character to its integer value and add it
        result += chars[i] - '0';
    }

    // Apply the negative sign if needed
    if (is_negative) {
        result = -result;
    }

    return result;
}
int executeMoveCommand(int eq, int dec, int fc){
	trackingEnabled = false;
	printf("Disabled Tracking For Manual Control\n......\n");
	if(eq == 0){
		printf("No EQ Command\n");
	}else if(eq != 0){
		printf("Moving EQ Axis %d steps, in the %d direction \n", abs(eq), eq > 0);
		moveEQAxis(eq > 0, abs(eq));
	}
	if(dec == 0){
		printf("No DEC Command\n");
	}else if (dec != 0){
		printf("Moving DEC Axis %d steps, in the %d direction \n", abs(dec), dec >0);
		moveDecAxis(dec > 0, abs(dec));
	}
	if(fc == 0){
		printf("No Focus Command\n");
	}else if( fc != 0){
		printf("Moving FC Axis %d steps, in the %d direction \n", abs(fc), fc >0);
		moveFocAxis(fc > 0, abs(fc));
	}

	printf(".....\nFinished all movement commands, re-enabling tracking ...\n");

	
}
int executeMachineCommand(int commandNumber){
	printf("Machine command %d  initiated \n", commandNumber);
	return 0;

}
int main(){
	stdio_init_all();
	printf("Initializing...\n");
	initializeI2C();
	initializeIo();
	gpio_set_irq_enabled_with_callback(KHZ, 0x04, 1, & callBack);
	bool messageCompleted = false;
	char message[1000];	
	int messageChar = 0;
	int cmds[3];
	while(1){
		if(uart_is_readable(uart0)){
			char c = uart_getc(uart0);
			message[messageChar] = c;
			messageChar += 1;
			if(c == '\r'){
				messageCompleted = true;
				message[messageChar -1] = 0;
				messageChar = 0;
			}
		}
		if(messageCompleted && message[0] != 'G'){
			int count = 0; 
			char numArr[1000];
			int varsPulled = 0;
			for(int i = 0; i < 1000; i++){
				if(message[i] != 0 || varsPulled < 3){
					if(message[i] != ';'){
						numArr[count] = message[i];
						count++;
					}else{
						cmds[varsPulled] = char_array_to_int(numArr, count);								  varsPulled += 1;
						count = 0;
						for(int i = 0; i < 1000; i++){
							numArr[i] = 0; 
						}
					}
				}
			}	
			messageCompleted = false;
			executeMoveCommand(cmds[0], cmds[1], cmds[2]);
			for (int i = 0; i < 1000; i++){
				message[i] = 0;
			}
			cmds[0] = 0;
			cmds[1] = 0;
			cmds[2] = 0;
		}else if(messageCompleted){
			int count = 0;
			char numArr[1000];
			for (int i = 1; i < 1000; i++){
				if(message[i] != 0){
					numArr[count] = message[i];
					count++;
				}else{
					numArr[i] = message[i];
			}
		}
				
			executeMachineCommand(char_array_to_int(numArr, count));
			messageCompleted = false;
			for (int i = 0; i < 1000; i++){
				message[i] = 0;
				numArr[i] = 0;
			}


	}	

	}
    
}