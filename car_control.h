#include <stdint.h>
#include <unistd.h>

#define Kp  0.3
#define Ki  0
#define Kd  0.2
#define dT  0.1

extern "C" {
	#include "car_lib.h"
}
class Driver
{
    public:
    	Driver();
        void drive(struct thr_data *data, CVinfo cvInfo, SensorInfo sensorInfo);
        void goTunnel();
        void waitStartSignal();
    private:
        struct DriveState {
        	bool isGoing;
        	bool isTurningRight;
        	bool isTurningLeft;
        	bool isEnteringCurve;

            bool isWhiteLineDetected;
        };
        struct ParkingState {
            uint8_t stage[7];
        };
        DriveState driveState;
        ParkingState parkingState;

        float I_term;
        float prev_error;
        uint8_t emergencyTimeout;

        bool TurnDetected(CVinfo cvInfo);
        bool LineDetected(CVinfo cvInfo);

        bool isWhiteLineDetected(SensorInfo sensorInfo);
        void updatePakingState(SensorInfo sensorInfo, ParkingState *parkingState);

        bool Turning(DriveState driveState);
        void StateisEnteringCurve(struct DriveState *driveState);
        void StateisGoing(struct DriveState *driveState);
};
class Sensor
{
    public:
        Sensor();
        SensorInfo getInfo();
    private:
        SensorInfo sensorInfo;
};