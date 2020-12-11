#include "LIS3DSH.h"
#include "mbed.h"
 
#define LIS3DSH_INFO1                       0x0D
#define LIS3DSH_INFO2                       0x0E
#define LIS3DSH_WHO_AM_I                    0x0F
#define LIS3DSH_OFF_X                       0x10
#define LIS3DSH_OFF_Y                       0x11
#define LIS3DSH_OFF_Z                       0x12
#define LIS3DSH_CTRL_REG4                   0x20
#define LIS3DSH_CTRL_REG1                   0x21
#define LIS3DSH_CTRL_REG2                   0x22
#define LIS3DSH_CTRL_REG3                   0x23
#define LIS3DSH_CTRL_REG5                   0x24
#define LIS3DSH_CTRL_REG6                   0x25
#define LIS3DSH_OUT_X_L                     0x28
#define LIS3DSH_OUT_X_H                     0x29
#define LIS3DSH_OUT_Y_L                     0x2A
#define LIS3DSH_OUT_Y_H                     0x2B
#define LIS3DSH_OUT_Z_L                     0x2C
#define LIS3DSH_OUT_Z_H                     0x2D
#define LIS3DSH_FIFO_CTRL_REG               0x2E
 
#define LIS3DSH_READ                        0x80
#define LIS3DSH_WRITE                       0x00
 
LIS3DSH::LIS3DSH(PinName mosi, PinName miso, PinName clk, PinName cs)
: _spi(mosi, miso, clk), _cs(cs) 
 {
    
    // Make sure CS is high
    _cs = 1;
 
    // Set up the spi interface
    _spi.format(8, 3);
    _spi.frequency(1000000);
    
    // Configure LIS3DSH
    WriteReg(LIS3DSH_CTRL_REG4, 0x5F);             // Normal power mode, all axes enabled, 50 Hz ODR
    WriteReg(LIS3DSH_CTRL_REG5, 0x80);             // 200 Hz antialias filter, +/- 2g FS range   
    WriteReg(LIS3DSH_FIFO_CTRL_REG, 0);            // configure FIFO for bypass mode   
    WriteReg(LIS3DSH_CTRL_REG6, 0x10);             // disable FIFO, enable register address auto-increment
 
    /* these two lines prevents lock-up of sampling according to:
    https://my.st.com/public/STe2ecommunities/mems_sensors/Lists/Accelerometers/DispForm.aspx?ID=304&Source=/public/STe2ecommunities/mems_sensors/Tags.aspx?tags=sampling
    
    Not sure why it works
    */
    WriteReg(LIS3DSH_CTRL_REG4, 0x00);
    WriteReg(LIS3DSH_CTRL_REG4, 0x37);
}
 
void LIS3DSH::WriteReg(uint8_t addr, uint8_t data) {
    _cs = 0;
    _spi.write(LIS3DSH_WRITE | addr);
    _spi.write(data);
    _cs = 1;
}
 
uint8_t LIS3DSH::ReadReg(uint8_t addr) {
    uint8_t data;
    
    _cs = 0;           
    _spi.write(LIS3DSH_READ | addr);
    data = _spi.write(0x00);             
    _cs = 1;
    
    return(data);
}
 
int LIS3DSH::Detect(void) {
    if(ReadReg(LIS3DSH_WHO_AM_I) == 0x3F)
        return(1);
    else
        return(0);
}
 
void LIS3DSH::ReadData(int16_t *X, int16_t *Y, int16_t *Z) {
    uint8_t xLSB=0, xMSB, yLSB, yMSB, zLSB, zMSB;       // 8-bit values from accelerometer
    
    xMSB = ReadReg(LIS3DSH_OUT_X_H);                    // read X high byte register
    xLSB = ReadReg(LIS3DSH_OUT_X_L);                     // read X low byte register
    yMSB = ReadReg(LIS3DSH_OUT_Y_H);
    yLSB = ReadReg(LIS3DSH_OUT_Y_L);
    zMSB = ReadReg(LIS3DSH_OUT_Z_H);
    zLSB = ReadReg(LIS3DSH_OUT_Z_L);
        

    //pack MSB and LSB bytes for X, Y, and Z
    *X = (xMSB << 8) | (xLSB);   
    *Y = (yMSB << 8) | (yLSB);
    *Z = (zMSB << 8) | (zLSB);
}
 
void LIS3DSH::ReadAngles(float *Roll, float *Pitch) {   
    int16_t Xg, Yg, Zg;                          // 16-bit values from accelerometer
 
    //Read X, Y, Z raw values from acceleromoter     
    ReadData(&Xg, &Yg, &Zg);
    
    //Convert X, Y, and Z accelerometer values to meters per second squared
    Xg /= -141;                          
    Zg /= -141;  // actual = 1g - raw = -1*141                         
    Yg /= -141;
        
    *Roll = gToDegrees(Zg, Xg);                  // get degrees between Z and X planes
    *Pitch = gToDegrees(Zg, Yg);                 // get degrees between Z and Y planes
}

float LIS3DSH::gToDegrees(float V, float H)      
{
  float retval;
  uint16_t orientation=0;
 
  if (H == 0) H = 0.001;                         // preventing division by zero
  if (V == 0) V = 0.001;                         // preventing division by zero
 
  if ((H > 0) && (V > 0)) orientation = 0;
  if ((H < 0) && (V > 0)) orientation = 90; 
  if ((H < 0) && (V < 0)) orientation = 180;
  if ((H > 0) && (V < 0)) orientation = 270;
 
  retval = (atan((double)V/(double)H)/3.14159)*180.0;
  if ((double)retval < 0) 
    retval = (double)retval + 90.0;
  retval = fabs(retval) + orientation;
  return retval;
}