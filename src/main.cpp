#include <Arduino.h>
#include <math.h>
#include "Movement.h"
#include "pins.h"
#include "defines.h"
#include "Adafruit_BNO055.h"
#include "PID.h"
#include "TSSPs.h"
#include "orbit.h"
#include "LightSensor.h"
#include "lineavoidance.h"
#include "Camera.h"
#include "time.h"
#include "kicker.h"
#include "Bluetooth.h"
#include <Wire.h>

using namespace bon;
sensors_event_t event;
Movement motors;
TSSPs tssps;
Orbit orbit;
LightSensor lightsensor;
oAvoidance outAvoidance;
Camera camera;
Kicker kicker;
Bluetooth bluetooth;
Timer surgeTimer = Timer((unsigned long)SURGE_MAX_TIME);

Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x28, &Wire);
PID pid = PID(COMPASS_P, COMPASS_I, COMPASS_D);
PID cameraPid = PID(CAMERA_P, CAMERA_I, CAMERA_D);
PID sidewayPid = PID(SIDEWAY_P, SIDEWAY_I, SIDEWAY_D);
PID forwardPid = PID(forward_P, forward_I, forward_D);
PID centrePid = PID(centre_P, centre_I, centre_D);
PID defendPid = PID(defend_P, defend_I, defend_D);

struct Move
{
	int speed;
	int dir;
	int rot;
};

long long start;
int counter = 0;
int bnoCtr = 0;
unsigned long startTime;
bool spin;
bool surge = false;

int compass_correct(float targetHeading = 0)
{
	sensors_event_t event;
	bno.getEvent(&event);
	float orient = (float)event.orientation.x;
	if (orient > 180)
	{
		orient = orient - 360;
	}
	if (targetHeading > 180)
	{
		targetHeading = targetHeading - 360;
	}

	return pid.update(orient, 1.25 * targetHeading);
}

int sideways_correct(float orient = tssps.ballDir, float targetHeading = 0)
{
	if (orient > 180)
	{
		orient -= 360;
	}
	orient *= -1;
	return sidewayPid.update(orient, targetHeading);
}

float centre_correct(float targetHeading = float(event.orientation.x))
{
	float orient = camera.defendDist*sin(DEG2RAD*camera.defendAngle);
	// Serial.println(orient);
	if (targetHeading > 180)
	{
		targetHeading = targetHeading - 360;
	}
	targetHeading = targetHeading*-1;
	return centrePid.update(orient, targetHeading);
}

int forwards_correct()
{
	float targetDist = (ROBOT == 1? 25 : 30);

	int currentDist = camera.defendDist;

	return forwardPid.update(currentDist, targetDist);
}

int defend_correct(float targetHeading = 180)
{
	float orient = camera.defendAngle;

	return defendPid.update(orient, targetHeading);
}

int camera_correct(float targetHeading = 0)
{
	float orient = camera.attackAngle;
	if (orient > 180)
	{
		orient = orient - 360;
	}
	if (targetHeading > 180)
	{
		targetHeading = targetHeading - 360;
	}

	return cameraPid.update(orient, targetHeading);
}

Move attack(double ballDir, double ballStr, bool ballVis, double outDir, double outSpd, double lineAngle){

	Move movement;

	if (lineAngle != -1) {
		if (outAvoidance.botlocation == -1){
			movement.speed = outSpd;
			movement.dir = outDir;
			movement.rot = compass_correct();
		} else if (outAvoidance.botlocation >= 0 && tssps.ballDir > lineAngle-90 && tssps.ballDir < lineAngle+90 && tssps.ballVisible){
			if (lineAngle > 160 && lineAngle < 200){
				movement.speed = outSpd;
				movement.dir = outDir;
				movement.rot = compass_correct();
			} else if ((tssps.ballDir > 330 || tssps.ballDir < 30) && camera.attackDist < 30) {
				movement.speed = outSpd;
				movement.dir = outDir;
				movement.rot = compass_correct();
			} else if (camera.attackDist < 30 || camera.defendDist < 30) {
				Orbit::OrbitData orbitData = orbit.update(tssps.ballDir, tssps.ballStr);
				movement.speed = orbitData.speed-EDGESPEED;
				movement.dir = floatMod(tssps.ballDir +tssps.calculateAngleAddition(), 360);
				movement.rot = camera_correct();
				if (((tssps.ballDirAvg < 19 && tssps.ballDirAvg > 17.8) || (tssps.ballDirAvg < -1 && tssps.ballDirAvg > -6)) && tssps.ballStrSurgeAvg > KICK_BALL_STR && (camera.attackAngle < 5 || camera.attackAngle > 355)){
					kicker.shouldKick = true;
					kicker.kickDelay.resetTime();
					// Serial.println("Kicking");
				}
			} else {
				Orbit::OrbitData orbitData = orbit.update(tssps.ballDir, tssps.ballStr);
				movement.speed = orbitData.speed;
				movement.dir = floatMod(tssps.ballDir +tssps.calculateAngleAddition(), 360);
				movement.rot = camera_correct();
			}
		} else{
			movement.speed = outSpd;
			movement.dir = outDir;
			movement.rot = compass_correct();
			// Serial.println(movement.direction);
		}
	} else {
		if (tssps.ballVisible && outAvoidance.botlocation != -1) {
			if (camera.attackDist < 35){
				Orbit::OrbitData orbitData = orbit.update(tssps.ballDir, tssps.ballStr);
				movement.speed = orbitData.speed-EDGESPEED;
				movement.dir = floatMod(tssps.ballDir +tssps.calculateAngleAddition(), 360);
				movement.rot = camera_correct();
			} else {
				Orbit::OrbitData orbitData = orbit.update(tssps.ballDir, tssps.ballStr);
				movement.speed = orbitData.speed;
				movement.dir = floatMod(tssps.ballDir +tssps.calculateAngleAddition(), 360);
				movement.rot = camera_correct();
				if (((tssps.ballDirAvg < 19 && tssps.ballDirAvg > 17.8) || (tssps.ballDirAvg < -1 && tssps.ballDirAvg > -6)) && tssps.ballStrSurgeAvg > KICK_BALL_STR && (camera.attackAngle < 5 || camera.attackAngle > 355)){
					kicker.shouldKick = true;
					kicker.kickDelay.resetTime();
					// Serial.println("Kicking");
				}
			}
		} else{
			movement.speed = outSpd;
			movement.dir = outDir;
			movement.rot = camera_correct();
		}
	}
	return movement;
}

Move defend(double ballDir, double ballStrAvg, bool ballVis, double outDir, double outSpd, bool defendVis, int defendDist, double lineAngle, double heading){
	Move move;
	bnoCtr++;
	if(bnoCtr % 5 == 0) {
		bno.getEvent(&event);
	}

	if (ballStrAvg >= DEFENSE_SURGE_STRENGTH && (tssps.ballDirAvg < 30 && tssps.ballDirAvg > -30) && defendDist < (ROBOT == 1? 28 : 33)){
		surge = true;
		surgeTimer.resetTime();	
	} else if (surge && defendDist > 45){
		surge = false;
	} else if (surgeTimer.timeHasPassedNoUpdate()){
		surge = false;
	}

	int fwdc = forwards_correct();
	int swdc = sideways_correct();
	int cdc = centre_correct();

	if(defendDist < 45){
		if(outDir == -1 && outAvoidance.botlocation != -1){
			if(ballVis && defendVis){
				if (surge){
					Orbit::OrbitData orbitData = orbit.update(tssps.ballDir, tssps.ballStr);
					move.speed = orbitData.speed;
					if (defendDist > 27.5){
						move.dir = floatMod(tssps.ballDir +tssps.calculateAngleAddition(), 360);
					} else{
						move.dir = ballDir;
					}
					if(camera.attackVis){
						move.rot = camera_correct();
						if ((camera.attackAngle < 2.5 || camera.attackAngle > 357.5) && tssps.ballDirAvg < 1 && tssps.ballDirAvg > -1 && tssps.ballStrSurgeAvg > KICK_BALL_STR){
							kicker.shouldKick = true;
						}
					} else {
						move.rot = compass_correct();
						if (tssps.ballDirAvg < 1 && tssps.ballDirAvg > -1 && tssps.ballStrSurgeAvg > KICK_BALL_STR){
							kicker.shouldKick = true;
						}
					}
					Serial.println("Surging");
				} else {
					if(camera.defendAngle-heading > 235 && tssps.ballDir > 200 && tssps.ballDir < 230) {
						move.dir = 270;
						move.speed = ORBIT_FAR_SPEED;
						move.rot = defend_correct();
						Serial.println("Moving Left");
					} else if (camera.defendAngle-heading > 135 && tssps.ballDir > 130 && tssps.ballDir < 160 ){
						move.dir = 90;
						move.speed = ORBIT_FAR_SPEED;
						move.rot = defend_correct();
					} else {
						move.speed = sqrt(((fwdc)*(fwdc))+((swdc)*(swdc)));
						move.dir = floatMod(RAD_TO_DEG * atan2(swdc, fwdc), 360);
						move.rot = defend_correct();
					}
				}
			} else{
				if (defendVis){
					move.speed = sqrt(((fwdc) * (fwdc)) + ((cdc) * (cdc)));
					move.dir = floatMod(RAD_TO_DEG * atan2(cdc, fwdc), 360);
					move.rot = defend_correct();
				} 
			}
		} else{
				move.dir = outDir;
				move.speed = outSpd;
				move.rot = defend_correct();
		}
    } else if (tssps.ballVisible && ballDir > 90 && ballDir < 270){
        Orbit::OrbitData orbitData = orbit.update(tssps.ballDir, tssps.ballStr);
        move.speed = orbitData.speed;
        move.dir = floatMod(tssps.ballDir +tssps.calculateAngleAddition(), 360);
        move.rot = compass_correct();
	} else {
		move.speed = sqrt(((fwdc)*(fwdc))+((swdc)*(swdc)));
		move.dir = floatMod(RAD_TO_DEG * atan2(swdc, fwdc), 360);
		move.rot = defend_correct();
	}
	return move;
}


void setup()
{
	Serial.begin(9600);
	camera.init();
 	lightsensor.init();
	bluetooth.init();
	Wire.begin();
	bno.begin();
	kicker.init();
	if (!bno.begin(bno.OPERATION_MODE_IMUPLUS))
	{
		Serial.println("Error connecting to bno");
		while(1);
	}
	bno.setExtCrystalUse(true);
	delay(500);
	Serial.println("Done");
}

void loop()
{
	tssps.update();
	bnoCtr++;
	if(bnoCtr % 5 == 0) {
		bno.getEvent(&event);
	}
	float ol = lightsensor.update();
	float orient = -1* ((float)event.orientation.x) +360;
	if (orient > 180){
		orient = orient -360;
	}
  	float lineAngle = (ol != -1 ? floatMod(ol+orient, 360) : -1.00);
  	oAvoidance::Movement outavoidance = outAvoidance.moveDirection(lineAngle);
  	outavoidance.direction = (outavoidance.direction != -1 ? -1* (floatMod(outavoidance.direction-orient, 360)) + 360 : -1.00);
	camera.update(blueAttack == true);
	bluetooth.update(tssps.ballStrSurgeAvg);

	if (bluetooth.thisData.role == ATTACK_MODE){	// -- Attacking -- 
		// Serial.println("Attacking");
		Move att = attack(tssps.ballDir, tssps.ballStr, tssps.ballVisible, outavoidance.direction, outavoidance.speed, lineAngle);
		motors.move(att.speed, att.dir, att.rot);

	} else {	// -- Defending --
		// Serial.println("Defending");
		if (camera.defendDist != 0) {
			Move def = defend(tssps.ballDir, tssps.ballStrAvg, tssps.ballVisible, outavoidance.direction, outavoidance.speed, camera.defendVis, camera.defendDist, lineAngle, orient);
			motors.move(def.speed, def.dir, def.rot);
			// Serial.println("Defending");
		} 
		else if (tssps.ballVisible && lineAngle != -1 && outAvoidance.botlocation != -1) {
			Orbit::OrbitData orbitData = orbit.update(tssps.ballDir, tssps.ballStr);
			motors.move(orbitData.speed, constrain(floatMod(tssps.ballDir +tssps.calculateAngleAddition(), 360), 90, 270), camera_correct());
			// Serial.println("Orbiting");
		} else {
			motors.move(outavoidance.speed, outavoidance.direction,compass_correct());
		}
	}

	kicker.update();


	//- Testing -//

	// bno.getEvent(&event);
	// Serial.println(event.orientation.x);
	// motors.move(0,0,defend_correct());
	// if (tssps.ballStrSurgeAvg > 180){
	// 	Serial.println(tssps.ballStrSurgeAvg);
	// }
	// if (tssps.ballStrAvg > 165){
	// 	Serial.println(tssps.ballStrAvg);
	// }
	// Serial.print(camera.defendAngle-orient);
	// Serial.print("  ");
	// Serial.println(tssps.ballDir);
	// Serial.println(camera.attackAngle);

	// lightsensor.test();
	// Serial.println(outAvoidance.botlocation);
	// Serial.println(lineAngle);
	// motors.move(outavoidance.speed, outavoidance.direction,compass_correct());

	
	//- Kicker -//

	// digitalWrite(KICKER_PIN,HIGH);
	// delay(5000);
	// digitalWrite(KICKER_PIN,LOW);
	// Serial.println("LOWWWWWW");
	// delay(30);
	// digitalWrite(KICKER_PIN,HIGH);
	// delay(5000);

}