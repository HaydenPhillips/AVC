#include <stdio.h>
#include <time.h>
#include "E101.h"
#include "Sensor.cpp"
#include "Movement.cpp"

using namespace std;

class Camera: public Sensor {
	int whitePixels;
	int totalError;

	/** Get current PID values */
	ErrorSignal getErrorSignal() {
		this->whitePixels = 0;
		int sum = 0;
		int error = 0;
		int whiteThreshold = 100;
		ErrorSignal errorSignal = {0, 0 ,0};

		/** PID Constants */
		float kp = 0.0009;
		float ki = 0;
		float kd = 0.001;

		take_picture();
		for (int i = 0; i < 320; i++) {
			if(get_pixel(120, i, 3) > whiteThreshold) {
				whitePixels++;
				sum = 1;
			}
			else {
				sum = 0;
			}
			error = error + (i - 160) * sum;
			errorSignal.p = error * kp;

			if (get_pixel(110, i, 3) > whiteThreshold) {
				sum = 1;
			}
			else {
				sum = 2;
			}
			int newError = error + (i - 160) * sum;
			errorSignal.d = (newError-error)*kd;
		}
		this->totalError += error;
		errorSignal.i = totalError*ki;

		if (this->whitePixels > 300) this->quad = 3;

		return errorSignal;
	}

public:
	Camera() {
		this->quad = 1;
		this->turning = false;
	}

	Movement getNextDirection() {
		this->turning = false;
		ErrorSignal errorSignal = getErrorSignal();
		Movement movement;
		if (whitePixels > 0) {
			printf("%d\n", whitePixels);
			// printf("P: %d, I: %d D: %d\n", errorSignal.p, errorSignal.i, errorSignal.d);
			// movement.setMotor(40 - (errorSignal.p + errorSignal.i + errorSignal.d), 35 + (errorSignal.p + errorSignal.i + errorSignal.d));
			movement.setMotion(errorSignal);
		}
		else if (this->quad == 3) {
			printf("Working\n");
			this->turning = true;
			movement.setMotor(-40, 35);
		}
		else {
			movement.setMotor(-50, -45);
		}

		return movement;
	}
};
