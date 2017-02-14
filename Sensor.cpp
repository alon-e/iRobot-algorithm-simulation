#include "Sensor.h"

void Sensor::UpdateSensor(int dirtLevel, bool isWall[4]) {
	info.dirtLevel = dirtLevel;
	for (auto i = 0; i < 4; i++) {
		info.isWall[i] = isWall[i];
	}
}