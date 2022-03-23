#include <zephyr.h>
#include <errno.h>
#include <device.h>
#include <drivers/i2c.h>

#include <logging/log.h>
LOG_MODULE_DECLARE(TTPMS);

#include "MLX90641_I2C_Driver.h"
#include "sensors.h"

//const struct device *i2c_dev;

// this function should be moved into sensors.c, it has no reason to be MLX-specific
// i2c_dev will then need to be a global variable in sensors.c and extern in sensors.h
/*void MLX90641_I2CInit()
{   
    i2c_dev = DEVICE_DT_GET(DT_NODELABEL(i2c0));
    i2c_configure(i2c_dev, I2C_SPEED_SET(I2C_SPEED_FAST));
    // set MLX90641 refresh rate
}*/

int MLX90641_I2CGeneralReset(void)  // this function ideally should not be used since it affects all devices on I2C line
{    
    int ack;

    uint8_t cmd = 0x06;

    ack = i2c_write(i2c_dev, &cmd, 1, 0x0000);  // writing 0x06 to address 0x00 should reset all devices on I2C
    
    if (ack != 0)
    {
        return -1;
    }         
  
    k_sleep(K_USEC(50));    // idk why this sleep is here
    
    return 0;
}

int MLX90641_I2CRead(uint8_t slaveAddr, uint16_t startAddress, uint16_t nMemAddressRead, uint16_t *data)
{
    //LOG_INF("slaveAddr: %d startAddress: %d nMemAddressRead: %d *data: %p", slaveAddr, startAddress, nMemAddressRead, data);
                           
    int ack = 0;                               
    int cnt = 0;
    int i = 0;
    char cmd[2] = {0,0};
    char i2cData[1664] = {0};
    uint16_t *p;
    
    p = data;
    cmd[0] = startAddress >> 8;
    cmd[1] = startAddress & 0x00FF;

    //ack = i2c_write(i2c_dev, cmd, 2, slaveAddr);     
    //ack = i2c_read(i2c_dev, i2cData, 2*nMemAddressRead, slaveAddr);
    ack = i2c_write_read(i2c_dev, slaveAddr, cmd, 2, i2cData, 2*nMemAddressRead);
    
    if (ack != 0x00)
    {
        return -1; 
    }            
    
    for(cnt=0; cnt < nMemAddressRead; cnt++)    // this just turns uint8_t[2*nMemAddressRead] into uint16_t[nMemAddressRead]
    {
        i = cnt << 1;
        *p++ = (uint16_t)i2cData[i]*256 + (uint16_t)i2cData[i+1];
    }
    
    return 0;   
} 

void MLX90641_I2CFreqSet(int freq)  // unused, we set frequency on init
{
    
}

int MLX90641_I2CWrite(uint8_t slaveAddr, uint16_t writeAddress, uint16_t data)
{
    int ack = 0;
    char cmd[4] = {0,0,0,0};
    static uint16_t dataCheck;
    
    cmd[0] = writeAddress >> 8;
    cmd[1] = writeAddress & 0x00FF;
    cmd[2] = data >> 8;
    cmd[3] = data & 0x00FF;
    
    k_sleep(K_USEC(5));

    ack = i2c_write(i2c_dev, cmd, 4, slaveAddr);
    
    if (ack != 0x00)
    {
        return -1;
    }          
    
    MLX90641_I2CRead(slaveAddr, writeAddress, 1, &dataCheck);
    
    if (dataCheck != data)
    {
        return -2;
    }    
    
    return 0;
}