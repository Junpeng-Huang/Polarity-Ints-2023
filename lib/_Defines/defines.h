#ifndef DEFINES_H
#define DEFINES_H

#include <Arduino.h>
#include <math.h>
#include <EEPROM.h>

#define DEG2RAD (3.141592/180)
#define RAD2DEG (180/3.141592)

#define ROBOT 2

#define ORBIT_FAR_SPEED (ROBOT == 1 ? 240 : 240)
#define ORBIT_CLOSE_SPEED (ROBOT == 1 ? 200 : 200)
#define ORBIT_STRIKE_ANGLE (ROBOT == 1 ? 5 : 5)
#define STRIKE_SPEED (ROBOT == 1 ? 255 : 255)

#define COMPASS_P (ROBOT == 1 ? 2 : 2) 
#define COMPASS_I (ROBOT == 1 ? 0 : 0)
#define COMPASS_D (ROBOT == 1 ? 0.07 : 0.07)

#define CAMERA_P (ROBOT == 1 ? 1.5 : 1.5) 
#define CAMERA_I (ROBOT == 1 ? 0 : 0)
#define CAMERA_D (ROBOT == 1 ? 0.07 : 0.07)

//defending

#define centre_P (ROBOT == 1 ? 3 : 3) //2
#define centre_I (ROBOT == 1 ? 0 : 0)
#define centre_D (ROBOT == 1 ? 0.02 : 0.02) //0.05

#define SIDEWAY_P (ROBOT == 1 ? 2.75 : 3.3) //4
#define SIDEWAY_I (ROBOT == 1 ? 0 : 0)
#define SIDEWAY_D (ROBOT == 1 ? 0.01 : 0.01) //0.05

#define defend_P (ROBOT == 1 ? 1.5 : 1.5) //2
#define defend_I (ROBOT == 1 ? 0 : 0)
#define defend_D (ROBOT == 1 ? 0.07 : 0.1) //0.09

#define forward_P (ROBOT == 1 ? 20 : 15) //20
#define forward_I (ROBOT == 1 ? 0 : 0)
#define forward_D (ROBOT == 1 ? 0 : 0) //0.05

#define TSSP_X_0 0.70711
#define TSSP_X_1 0.92388
#define TSSP_X_2 1
#define TSSP_X_3 0.92388
#define TSSP_X_4 0.70711
#define TSSP_X_5 0.38268
#define TSSP_X_6 0
#define TSSP_X_7 -0.38268
#define TSSP_X_8 -0.70711
#define TSSP_X_9 -0.92388
#define TSSP_X_10 -1
#define TSSP_X_11 -0.92388
#define TSSP_X_12 -0.70711
#define TSSP_X_13 -0.38268
#define TSSP_X_14 0
#define TSSP_X_15 0.38268

#define TSSP_Y_0 -0.70711
#define TSSP_Y_1 -0.38268
#define TSSP_Y_2 0
#define TSSP_Y_3 0.38268
#define TSSP_Y_4 0.70711
#define TSSP_Y_5 0.92388
#define TSSP_Y_6 1
#define TSSP_Y_7 0.92388
#define TSSP_Y_8 0.70711
#define TSSP_Y_9 0.38268
#define TSSP_Y_10 0
#define TSSP_Y_11 -0.38268
#define TSSP_Y_12 -0.70711
#define TSSP_Y_13 -0.92388
#define TSSP_Y_14 -1
#define TSSP_Y_15 -0.92388

#define TSSP_NUM 16 //opposite
#define TSSP_READ_NUM 256
#define TSSP_SAMPLE 25
#define BALL_CLOSE_STRENGTH (ROBOT == 1 ? 140 : 140)
#define ATT_MULT (ROBOT == 1? 0.0075: 0.08)
#define DEF_MULT (ROBOT == 1? 0.1: 0.12)

#define ARRAYSHIFTDOWN(a, lower, upper){          \
	if (upper == (sizeof(a)/sizeof(a[0])) - 1){   \
		for (int q = upper - 1; q >= lower; q--){ \
			*(a + q + 1) = *(a + q); }            \
	} else{                                       \
		for (int q = upper; q >= lower; q--){     \
			*(a + q + 1) = *(a + q); }}}

float floatMod(float x, float m);

float angleBetween(float angleCounterClockwise, float angleClockwise);

float smallestAngleBetween(float angleCounterClockwise, float angleClockwise);

int8_t findSign(float value);

float midAngleBetween(float angleCounterClockwise, float angleClockwise);

float smallestAngleBetween(float angleCounterClockwise, float angleClockwise);

int8_t findSign(float value);

bool angleIsInside(float angleBoundCounterClockwise, float angleBoundClockwise, float angleCheck);



#define LS_NUM 32
#define LS_NUM_IND 16
#define LINE_BUFFER (ROBOT == 1 ? 700 : 650)
#define LS_AVOID_MEDIUM (ROBOT == 1 ? 160  : 210 )
#define LS_AVOID_FAST (ROBOT == 1 ? 200  : 255)
#define LS_CALIBRATE_COUNT 10
#define LS_OFFSET 180

#define LS_X_0  0
#define LS_X_1  0.195090322016
#define LS_X_2  0.382683432365
#define LS_X_3  0.55557023302
#define LS_X_4  0.707106781187
#define LS_X_5  0.831469612303
#define LS_X_6  0.923879532511
#define LS_X_7  0.980785280403
#define LS_X_8  1
#define LS_X_9  0.980785280403
#define LS_X_10 0.923879532511
#define LS_X_11 0.831469612303
#define LS_X_12 0.707106781187
#define LS_X_13 0.55557023302
#define LS_X_14 0.382683432365
#define LS_X_15 0.195090322016
#define LS_X_16 0
#define LS_X_17 -0.195090322016
#define LS_X_18 -0.382683432365
#define LS_X_19 -0.55557023302
#define LS_X_20 -0.707106781187
#define LS_X_21 -0.831469612303
#define LS_X_22 -0.923879532511
#define LS_X_23 -0.980785280403
#define LS_X_24 -1
#define LS_X_25 -0.980785280403
#define LS_X_26 -0.923879532511
#define LS_X_27 -0.831469612303
#define LS_X_28 -0.707106781187
#define LS_X_29 -0.55557023302
#define LS_X_30 -0.382683432365
#define LS_X_31 -0.195090322016

#define LS_Y_0 -1
#define LS_Y_1 -0.980785280403
#define LS_Y_2 -0.923879532511
#define LS_Y_3 -0.831469612303
#define LS_Y_4 -0.707106781187
#define LS_Y_5 -0.55557023302
#define LS_Y_6 -0.382683432365
#define LS_Y_7 -0.195090322016
#define LS_Y_8  0
#define LS_Y_9  0.195090322016
#define LS_Y_10  0.382683432365
#define LS_Y_11  0.55557023302
#define LS_Y_12  0.707106781187
#define LS_Y_13  0.831469612303
#define LS_Y_14  0.923879532511
#define LS_Y_15  0.980785280403
#define LS_Y_16  1
#define LS_Y_17  0.980785280403
#define LS_Y_18  0.923879532511
#define LS_Y_19  0.831469612303
#define LS_Y_20  0.707106781187
#define LS_Y_21  0.55557023302
#define LS_Y_22  0.382683432365
#define LS_Y_23  0.195090322016
#define LS_Y_24  0
#define LS_Y_25 -0.195090322016
#define LS_Y_26 -0.382683432365
#define LS_Y_27 -0.55557023302
#define LS_Y_28 -0.707106781187
#define LS_Y_29 -0.831469612303
#define LS_Y_30 -0.923879532511
#define LS_Y_31 -0.980785280403

#define CAMERA_BAUD 9600
#define EDGESPEED 75
#define blueAttack 1

#define DEFENSE_SUGRE_SPEED 255
#define DEFENSE_SURGE_STRENGTH 155
#define SURGE_MAX_TIME 500000

#define KICK_BALL_STR 175
#define KICK_DELAY_TIME 250000
#define KICK_DISCHARGE_TIME 50000 
#define KICK_CHARGE_TIME 15000000
#define KICKING_DISTANCE 50

#define BLUETOOTH_BAUD 9600
#define DISCONNECTED_TIMER 1000000
#define CONNECTED_TIMER 200000
#define BT_UPDATE_TIME 100000
#define BT_START_BYTE 255
#define BT_PACKET_SIZE 4
#define BT_SEND_TIMER 100000
#define BT_SWITCH_TIMER 4000
#define SWITCH_STRENGTH 150
#define SWITCH_GOALDIST 47
#define DEFENSE_MODE 0
#define ATTACK_MODE 1

struct BluetoothData{
	int ballStrSurge;
	int role;
};


#endif