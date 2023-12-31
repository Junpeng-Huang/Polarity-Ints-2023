#include "LightSensor.h"
LightSensor::LightSensor()
{
    pinMode(MPLS1, OUTPUT);
    pinMode(MPLS2, OUTPUT);
    pinMode(MPLS3, OUTPUT);
    pinMode(MPLS4, OUTPUT);
    pinMode(MPOUT2, INPUT);
    pinMode(MPOUT, INPUT);
}

void LightSensor::debug()
{
    readAll();
    for (int i = 0; i < LS_NUM; i++)
    {
        if (read[i] > 5)
        {
            broke = false;
        }
        else
        {
            broke = true;
            break;
        }
    }
}

void LightSensor::readAll()
{
    for (uint8_t i = 0; i < LS_NUM_IND; i++)
    {
        digitalWrite(MPLS4, (i >> 0) & 0x01);
        digitalWrite(MPLS3, (i >> 1) & 0x01);
        digitalWrite(MPLS2, (i >> 2) & 0x01);
        digitalWrite(MPLS1, (i >> 3) & 0x01);
        // int a = 4;
        // Serial.print((a>>0) & 0x01);
        // Serial.print(" ");
        // Serial.print((a>>1) & 0x01);
        // Serial.print(" ");
        // Serial.print((a>>2) & 0x01);
        // Serial.print(" ");
        // Serial.print((a>>3) & 0x01);
        // Serial.println(" ");
        // delay(500);
        read[indexA[i]] = analogRead(MPOUT2);
        read[indexB[i]] = analogRead(MPOUT);
    }
}

void LightSensor::init()
{
    for (uint8_t i = 0; i < LS_NUM; i++)
    {
        uint16_t calibrateTotal = 0;
        for (uint8_t j = 0; j < LS_CALIBRATE_COUNT; j++)
        {
            calibrateTotal += ls_cal[i];
        }
        ls_cal[i] = (uint16_t)round(calibrateTotal / LS_CALIBRATE_COUNT) + LINE_BUFFER;
    }
}

void LightSensor::test()
{
    readAll();
    
    for (int j = 0; j < LS_NUM; j++)
    {
        if (ROBOT == 1 && (j == 9 || j == 12 || j == 15 || j == 18 || j == 29))
        {
            if (read[j] >= ls_cal[j] - 350)
            {
                white[j] = 1;
                // Serial.println(read[j]);
            }
            else
            {
                white[j] = 0;
            }
        } else if (ROBOT == 1 ? (j == 2 || j == 7 || j == 8 || j == 13) : (j == 4 || j == 6 || j == 7 || j == 21 || j == 23 || j == 24)) {
            if (read[j] >= ls_cal[j] - 400)
            {
                white[j] = 1;
                // Serial.println(read[j]);
            }
            else
            {
                white[j] = 0;
            }
        } else if (ROBOT == 1 && j == 17) {
            if (white[j-1] == 1 && white[j+1] == 1) {
                white[j] = 1;
            } else {
                white[j] = 0;
            }
        } else if (read[j] >= ls_cal[j]) {
            white[j] = 1;
        } else {
            white[j] = 0;
        }
        Serial.print(white[j]);
        Serial.print(" ");
    }
    Serial.println(" ");

    // Serial.println(read[17]);

    // for(int i=0; i<32; i++){
    //     Serial.print(read[i]);
    //     Serial.print(" ");
    // }
    // Serial.println(" ");
    // delay(200);
}

double LightSensor::update()
{
    readAll();
    clusterNum = -1;
    LightSensor::Cluster clusterArray[4];

    for (int j = 0; j < LS_NUM; j++)
        {
        if (ROBOT == 1 && (j == 9 || j == 12 || j == 15 || j == 17 || j == 18))
        {
            if (read[j] >= ls_cal[j] - 350)
            {
                white[j] = 1;
                // Serial.println(read[j]);
            }
            else
            {
                white[j] = 0;
            }
        } else if (ROBOT == 1 ? (j == 2 || j == 7 || j == 8 || j == 13) : (j == 4 || j == 6 || j == 7 || j == 21 || j == 23 || j == 24)) {
            if (read[j] >= ls_cal[j] - 400)
            {
                white[j] = 1;
                // Serial.println(read[j]);
            }
            else
            {
                white[j] = 0;
            }
        } else if (read[j] >= ls_cal[j]) {
            white[j] = 1;
        } else {
            white[j] = 0;
        }
    }

for (int j = 0; j < LS_NUM; j++)
{ // reads sensors
    if (white[j] == 1)
    { // checks for white
        if (!inCluster)
        { // checks if it is in cluster
            inCluster = true;
            clusterNum++;
            clusterArray[clusterNum].start = j;
        }
        if (j == 31 && clusterArray[0].start != -1)
        {
            clusterArray[0].start = clusterArray[clusterNum].start;
        }
    }
    else
    { // if no longer in white
        if (inCluster)
        { // checks if in cluster
            inCluster = false;
            clusterArray[clusterNum].end = j - 1;
            clusterArray[clusterNum].midpoint = -1 * (midAngleBetween(clusterArray[clusterNum].start * (360 / LS_NUM), clusterArray[clusterNum].end * (360 / LS_NUM))) + 360;
        }
    }
}

switch (clusterNum)
{
case -1:
    lineAngle = -1;
    break;
case 0:
    lineAngle = 360 - floatMod(clusterArray[0].midpoint - LS_OFFSET, 360);
    break;
case 1:
    if (angleBetween(clusterArray[1].midpoint, clusterArray[0].midpoint) <= 180)
    {
        lineAngle = 360 - floatMod(midAngleBetween(clusterArray[1].midpoint, clusterArray[0].midpoint) - LS_OFFSET, 360);
    }
    else
    {
        lineAngle = 360 - floatMod(midAngleBetween(clusterArray[0].midpoint, clusterArray[1].midpoint) - LS_OFFSET, 360);
    }
    break;
case 2:
    float angleDiff12 = angleBetween(clusterArray[1].midpoint, clusterArray[0].midpoint);
    float angleDiff23 = angleBetween(clusterArray[2].midpoint, clusterArray[1].midpoint);
    float angleDiff31 = angleBetween(clusterArray[0].midpoint, clusterArray[2].midpoint);
    float biggestAngle = fmax(angleDiff12, fmax(angleDiff23, angleDiff31));
    if (biggestAngle == angleDiff12)
    {
        lineAngle = 360 - floatMod(midAngleBetween(clusterArray[0].midpoint, clusterArray[1].midpoint) + LS_OFFSET, 360);
    }
    else if (biggestAngle == angleDiff23)
    {
        lineAngle = 360 - floatMod(midAngleBetween(clusterArray[1].midpoint, clusterArray[2].midpoint) + LS_OFFSET, 360);
    }
    else
    {
        lineAngle = 360 - floatMod(midAngleBetween(clusterArray[2].midpoint, clusterArray[0].midpoint) + LS_OFFSET, 360);
    }
    break;
}

// Serial.print(clusterArray[0].midpoint);
// Serial.print("\t");
// Serial.print(clusterArray[1].midpoint);
// Serial.print("\t");
// Serial.println(clusterArray[2].midpoint);

return lineAngle;
}