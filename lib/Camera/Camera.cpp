#include "Camera.h"

Camera::Camera() {}

void Camera::init() {
    Serial7.begin(CAMERA_BAUD);
}

void Camera::update(bool attackBlue) {
    if(Serial7.available() >= 8) {
        uint8_t firstByte = Serial7.read();
        uint8_t secondByte = Serial7.peek();
        if(firstByte == 255 && secondByte == 255) {
            Serial7.read();
            int yellowX = Serial7.read();
            int yellowY = Serial7.read();
            yellowAngle = floatMod((RAD2DEG * atan2(240-yellowY-120, 320-yellowX-160)), 360);
            if(yellowAngle < 0){
                yellowAngle += 360;
            }
            yellowDist = sqrt(((320-yellowX-160)*(320-yellowX-160))+((240-yellowY-120)*(240-yellowY-120)));
            int blueX = Serial7.read();
            int blueY = Serial7.read();
            blueAngle = RAD2DEG * atan2(240-blueY-120, 320-blueX-160);
            if(blueAngle < 0){
                blueAngle += 360;
            }
            blueDist = sqrt(((320-blueX-160)*(320-blueX-160))+((240-blueY-120)*(240-blueY-120)));

            attackVis = (attackBlue ? (blueAngle != 224 ? true : false) : (yellowAngle != 224 ? true : false));
            defendVis = (attackBlue ? (yellowAngle != 224 ? true : false) : (blueAngle != 224 ? true : false));
            
            if(yellowAngle != 225){
                prevAngY = yellowAngle;
            } else{
                yellowAngle = prevAngY;
            }
            if(yellowDist < 60){
                prevDistY = yellowDist;
            } else{
                yellowDist = prevDistY;
            }
            if(blueAngle != 225){
                prevAngB = blueAngle;
            } else{
                blueAngle = prevAngB;
            }
            if(blueDist < 60){
                prevDistB = blueDist;
            } else{
                blueDist = prevDistB;
            }

            attackAngle = (attackBlue ? blueAngle : yellowAngle);
            attackDist = (attackBlue ? blueDist : yellowDist);
            defendAngle = (attackBlue ? yellowAngle : blueAngle);
            defendDist = (attackBlue ? yellowDist : blueDist);
        }
    }
    // delay(10);
}

float Camera::ballPixeltoCM(float dist){
    return (dist != 0) ? 0.0195649 * expf(0.0634054 * (dist + 30.1281)) + 21.2566 : 0;
}

float Camera::goalPixeltoCM(float dist){
    return (dist != 0) ? 5.7478f * powf(10.0f, -13.0f) * expf(0.0494379*(dist + 552.825f)) + 13.8327f : 0;
}

// #include "Camera.h"

// Camera::Camera() {}

// void Camera::init() {
//     Serial7.begin(CAMERA_BAUD);
//     // Serial.println("Done");
// }

// void Camera::update() {
//     if(Serial7.available() >= 8) {
//         // Serial.println("On");
//         uint8_t firstByte = Serial7.read();
//         uint8_t secondByte = Serial7.peek();
//         // Serial.printf("first: %d, second: %d\n", firstByte, secondByte);
//         if(firstByte == 255 && secondByte == 255) {
//             Serial7.read();
//             uint8_t highByte = Serial7.read();
//             uint8_t lowByte = Serial7.read();
//             if ((highByte << 8 | lowByte) == 65535){
//                 attackingGoalAngle = -1;
//                 // Serial.println("-1");
//             } else{
//                 attackingGoalAngle = floatMod((highByte << 8 | lowByte), 360);
//                 // Serial.print(lowByte);
//                 // Serial.print("\t");
//                 // Serial.println(highByte);
//             }
//             attackingGoalVisible = (attackingGoalAngle != 65535);
//             attackingGoalDistance = Serial7.read();
//             // highByte = Serial7.read();
//             // lowByte = Serial7.read();
//             // defendingGoalDistance = Serial7.read();
//             // defendingGoalAngle = highByte << 8 | lowByte;
//             // defendingGoalVisible = (defendingGoalAngle != 65535);
//             // Serial.println(1000 / (millis() - previousTime));
//             previousTime = millis();
//         }
//     }
// }