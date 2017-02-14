#include <algorithm>
#include "Battery.h"

void Battery::UpdateBatteryLife(bool isCharge = false) {
		isCharge ? batteryLife += rechargeRate : batteryLife -= consumptionRate;
		batteryLife = std::min(batteryLife, capacity);
}