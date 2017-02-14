#ifndef __SENSOR__H_
#define __SENSOR__H_

#include "AbstractSensor.h"

class Sensor : public AbstractSensor {
	SensorInformation info;

public:
	virtual SensorInformation sense() const override { return info; }

	void UpdateSensor(int dirtLevel, bool isWall[4]);
};

#endif //__SENSOR__H_