/* LIS3DSH Library v1.0
 * Copyright (c) 2016 Grant Phillips
 * grant.phillips@nmmu.ac.za
 *
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
 
#ifndef LIS3DSH_H
#define LIS3DSH_H
 
#include "mbed.h"
 
/** Class library for a LIS3DSH MEMS digital output motion sensor (acceleromoter).
 *
 * Example:
 * @code
 * #include "mbed.h"
 * #include "LIS3DSH.h"
 *
 * LIS3DSH acc(PA_7, PA_6, PA_5, PE_3);
 * //          mosi, miso, clk , cs
 * 
 * int main() {
 *    int16_t X, Y, Z;    //signed integer variables for raw X,Y,Z values
 *    float roll, pitch;  //float variables for angles
 *    
 *    if(acc.Detect() != 1) {
 *        printf("LIS3DSH Acceleromoter not detected!\n");
 *        while(1){ };
 *    }
 *    
 *    while(1) {
 *        acc.ReadData(&X, &Y, &Z);           //read X, Y, Z values
 *        acc.ReadAngles(&roll, &pitch);      //read roll and pitch angles
 *        SWO.printf("X: %d  Y: %d  Z: %d\n", X, Y, Z);
 *        SWO.printf("Roll: %f   Pitch: %f\n", roll, pitch);
 *       
 *        wait(1.0);  //delay before reading next values
 *    }
 *}
 * @endcode
 */
 
class LIS3DSH {
  public:
    /** Create a LIS3DSH object connected to the specified pins.
    * @param mosi SPI compatible pin used for the LIS3DSH's MOSI pin
    * @param miso SPI compatible pin used for the LIS3DSH's MISO pin
    * @param clk SPI compatible pin used for the LIS3DSH's CLK pin
    * @param cs DigitalOut compatible pin used for the LIS3DSH's CS pin
    */
    LIS3DSH(PinName mosi, PinName miso, PinName clk, PinName cs);
 
    /** Determines if the LIS3DSH acceleromoter can be detected.
    * @param 
    *     None
    * @return 
    *     1 = detected; 0 = not detected.
    */
    int Detect(void);
    
    /** Write a byte of data to the LIS3DSH at a selected address.
    * @param 
    *     addr 8-bit address to where the data should be written
    *     data 8-bit data byte to write
    * @return 
    *     None
    */
    void WriteReg(uint8_t addr, uint8_t data);
    
    /** Read data from the LIS3DSH from a selected address.
    * @param 
    *     addr 8-bit address from where the data should be read
    * @return
    *     Contents of the register as a 8-bit data byte.
    */
    uint8_t ReadReg(uint8_t addr);
    
    /** Reads the raw X, Y, Z values from the LIS3DSH as signed 16-bit values (int16_t).
    * @param 
    *     *X Reference to variable for the raw X value
    *     *Y Reference to variable for the raw Y value
    *     *Z Reference to variable for the raw Z value
    * @return 
    *     None
    */
    void ReadData(int16_t *X, int16_t *Y, int16_t *Z);
    
    /** Reads the roll angle and pitch angle from the LIS3DSH.
    * @param 
    *     *Roll Reference to variable for the roll angle (0.0 - 359.999999)
    *     *Pitch Reference to variable for the pitch angle (0.0 - 359.999999)
    * @return 
    *     None
    */
    void ReadAngles(float *Roll, float *Pitch);
 
  private:
    SPI _spi;
    DigitalOut _cs; 
    float gToDegrees(float V, float H);
};
 
#endif