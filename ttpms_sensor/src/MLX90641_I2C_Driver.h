#ifndef _MLX90641_I2C_Driver_H_
#define _MLX90641_I2C_Driver_H_

#include <stdint.h>

#define MLX90641_ADDR 0x33

    void MLX90641_I2CInit(void);
    int MLX90641_I2CGeneralReset(void);
    int MLX90641_I2CRead(uint8_t slaveAddr,uint16_t startAddress, uint16_t nMemAddressRead, uint16_t *data);
    int MLX90641_I2CWrite(uint8_t slaveAddr,uint16_t writeAddress, uint16_t data);
    void MLX90641_I2CFreqSet(int freq);
#endif