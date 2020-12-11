/* 	File: 		main.c
* 	Author: 	Michael Thoreau
*/

/* system imports */
#include <mbed.h>
#include <math.h>
#include <USBSerial.h>

/* user imports */
#include "LIS3DSH.h"


/* USBSerial library for serial terminal */
USBSerial serial(0x1f00,0x2012,0x0001,false);

/* LIS3DSH Library for accelerometer  - using SPI*/
LIS3DSH acc(PA_7, SPI_MISO, SPI_SCK, PE_3);

/* LED output */
DigitalOut ledOut(LED1);


/* state type definition */
typedef enum state_t {
	lying,
	sitting
} state_t;



int main() {
	int16_t X, Y;							// not really used
	int16_t zAccel = 0; 					// acceleration in z (raw)
	float g_z = 0;  						// acceleration in z (g force)
    float angle = 0;						// angle relative to z axis
	const float PI = 3.1415926;				// pi

	state_t state = lying;

	/**** Filter Parameters  ****/
    const uint8_t N = 20; 					// filter length
    float ringbuf[N];						// sample buffer
    uint8_t ringbuf_index = 0;				// index to insert sample

	/**  ringbug = {0, 15000, 0, 0, 0, 0, 0}  **/
	/*				   ^				       */
	/*				   |					   */
	/*			 ringbuf_index	  			   */


	/* check detection of the accelerometer */
	while(acc.Detect() != 1) {
        printf("Could not detect Accelerometer\n\r");
       	wait_ms(200);
    }


	while(1) {

		/* read data from the accelerometer */
		acc.ReadData(&X, &Y, &zAccel);

		/* normalise to 1g */
		g_z = (float)zAccel/17694.0;

		/* insert in to circular buffer */
		ringbuf[ringbuf_index++] = g_z;

		/* at the end of the buffer, wrap around to the beginning */
		if (ringbuf_index >= N) {
			ringbuf_index = 0;
		}




		/********** START of filtering ********************/
		
		float g_z_filt = 0;

		/* add all samples */
		for (uint8_t i = 0; i < N; i++) {
			g_z_filt += ringbuf[i];
		}

		/* divide by number of samples */
		g_z_filt /= (float)N;

		/* restrict to 1g (acceleration not decoupled from orientation) */
		if (g_z_filt > 1) {
			g_z_filt = 1;
		}

		/********** END of filtering **********************/
		


		/* compute angle in degrees */
		angle = 180*acos(g_z_filt)/PI;



		switch (state) {
			case lying: // lying down
				ledOut = 0;

				if (angle > 45) {
					state = sitting;
				}
				break;

			case sitting:	// sitting up
				ledOut = 1;

				if (angle < 30) {
					state = lying;
				}
				break;
			
			default:
				printf("michael made a mistake\n\r");
		}

		/* print angle - avoid doing this in a time sensitive loop */
		serial.printf("angle: %.2f degrees \r\n", angle);

		
		/* sample rate is very approximately 10Hz  - better would be to use the accelerometer to trigger an interrupt*/		
		wait_ms(100);
	}
	
}



