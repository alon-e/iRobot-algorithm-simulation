
#ifndef __BATTERY__H_
#define __BATTERY__H_

class Battery {
	int capacity;
	int consumptionRate;
	int rechargeRate;
	int batteryLife;

public:
	Battery() { Battery(0, 0, 0); }

	Battery(int capacity, int consumptionRate, int rechargeRate) 
		: capacity(capacity), consumptionRate(consumptionRate), rechargeRate(rechargeRate) { batteryLife = capacity; }

	int getconsumptionRate() const { return consumptionRate; };

	int getrechargeRate() const { return rechargeRate; };

	int getCapacity() const { return capacity; }

	int getBatteryLife() const { return batteryLife; }

	void UpdateBatteryLife(bool isCharge);

	bool IsFullyCharged() const { return batteryLife == capacity; }

	bool IsBatteryDead() const { return batteryLife <= 0; }
};

#endif //__BATTERY__H_