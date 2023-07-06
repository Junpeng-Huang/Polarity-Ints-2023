#include "TSSPs.h"

TSSPs::TSSPs()
{
	for (uint8_t i = 0; i < TSSP_NUM; i++)
	{
		pinMode(tsspPins[i], INPUT);
	}
}

void TSSPs::update()
{
	for (int i = tsspCounter; i < TSSP_READ_NUM; i++)
	{
		for (int i = 0; i < TSSP_NUM; i++)
		{
			tsspValues[i] += 1 - digitalRead(tsspPins[i]);
		}
		delayMicroseconds(3);
		tsspCounter++;
	}

	// for(int i=0; i<TSSP_NUM; i++){
	//     Serial.print(tsspValues[i]);
	//     Serial.print(" ");
	// }
	// Serial.println(" ");
	// delay(10);

	// Serial.println(tsspValues[15]); //1 is dead

	// if(tsspValues[7] != 0) {
	// 	tsspValues[7] += 42.5;
	// }

	for (int i = 0; i < TSSP_NUM; i++)
	{
		for (int j = 0; j < TSSP_NUM; j++)
		{
			if (tsspValues[i] > tsspSortedValues[j])
			{
				if (j <= i)
				{
					ARRAYSHIFTDOWN(tsspSortedValues, j, i);
					ARRAYSHIFTDOWN(tsspIndexes, j, i);
				}
				tsspSortedValues[j] = tsspValues[i];
				tsspIndexes[j] = i;
				break;
			}
		}
	}

	ballStr = ((3 * tsspSortedValues[0]) + (2 * tsspSortedValues[1]) + tsspSortedValues[2] + tsspSortedValues[3]) / 7;
	ballVisible = (ballStr != 0);
	ballStrSurge = tsspValues[6];

	float x = ((tsspSortedValues[0] * tsspXValues[tsspIndexes[0]]) + (tsspSortedValues[1] * tsspXValues[tsspIndexes[1]]) + (tsspSortedValues[2] * tsspXValues[tsspIndexes[2]]) + (tsspSortedValues[3] * tsspXValues[tsspIndexes[3]]) + (tsspSortedValues[4] * tsspXValues[tsspIndexes[4]])) / 5;
	float y = ((tsspSortedValues[0] * tsspYValues[tsspIndexes[0]]) + (tsspSortedValues[1] * tsspYValues[tsspIndexes[1]]) + (tsspSortedValues[2] * tsspYValues[tsspIndexes[2]]) + (tsspSortedValues[3] * tsspYValues[tsspIndexes[3]]) + (tsspSortedValues[4] * tsspYValues[tsspIndexes[4]])) / 5;
	ballDir = (ROBOT == 1 ? (ballVisible ? 360 - floatMod((RAD2DEG * (atan2f(y, x))) - 90, 360) : -1) : (ballVisible ? 360 - floatMod((RAD2DEG * (atan2f(y, x))) - 90, 360) : -1));


	float x1 = ((tsspSortedValues[0] * tsspXValues[tsspIndexes[0]]) + (tsspSortedValues[1] * tsspXValues[tsspIndexes[1]]) + (tsspSortedValues[2] * tsspXValues[tsspIndexes[2]])) / 3;
	float y1 = ((tsspSortedValues[0] * tsspYValues[tsspIndexes[0]]) + (tsspSortedValues[1] * tsspYValues[tsspIndexes[1]]) + (tsspSortedValues[2] * tsspYValues[tsspIndexes[2]])) / 3;
	ballDirRaw = ballVisible ? 360 - (RAD2DEG * (atan2f(y1, x1))) - 270 : -1;

	tsspCounter = 0;
	for (int i = 0; i < TSSP_NUM; i++)
	{
		tsspValues[i] = 0;
		tsspSortedValues[i] = 0;
		tsspIndexes[i] = 0;
	}
	rollingaverage();
}

void TSSPs::rollingaverage()
{
	ballStrSurgeAvg -= ballStrSurgeAvg / TSSP_SAMPLE;
	ballStrSurgeAvg  += ballStrSurge / TSSP_SAMPLE;

	ballDirAvg -= ballDirAvg / TSSP_SAMPLE;
	ballDirAvg  += ballDirRaw / TSSP_SAMPLE;

	ballStrAvg -= ballStrAvg / TSSP_SAMPLE;
	ballStrAvg  += ballStr / TSSP_SAMPLE;
}


float TSSPs::calculateAngleAddition()
{
	float dir = ballDir > 180 ? ballDir - 360 : ballDir;
	float ballAngleDifference = findSign(dir) * fmin(90, 0.15 * expf(0.35 * smallestAngleBetween(ballDir, 0)));
	float strengthFactor = constrain(ballStr / BALL_CLOSE_STRENGTH, 0, 1);
	float distanceMultiplier = constrain((0.01 * expf(4.5 * strengthFactor)), 0, 1);
	angleAddition = ballAngleDifference * distanceMultiplier;
	return angleAddition;
}