#ifndef _MLX90641_API_H_
#define _MLX90641_API_H_

#include <stdint.h>

#define SCALEALPHA 0.000001
    
  typedef struct
    {
        int16_t kVdd;
        int16_t vdd25;
        float KvPTAT;
        float KtPTAT;
        uint16_t vPTAT25;
        float alphaPTAT;
        int16_t gainEE;
        float tgc;
        float cpKv;
        float cpKta;
        uint8_t resolutionEE;
        uint8_t calibrationModeEE;
        float KsTa;
        float ksTo[8];
        int16_t ct[8];
        uint16_t alpha[192]; 
        uint8_t alphaScale;   
        int16_t offset[2][192];    
        int8_t kta[192];
        uint8_t ktaScale;    
        int8_t kv[192];
        uint8_t kvScale;
        float cpAlpha;
        int16_t cpOffset;
        float emissivityEE; 
        uint16_t brokenPixel;
    } paramsMLX90641;
    
    int MLX90641_DumpEE(uint8_t slaveAddr, uint16_t *eeData);
    int MLX90641_SynchFrame(uint8_t slaveAddr);
    int MLX90641_TriggerMeasurement(uint8_t slaveAddr);
    int MLX90641_GetFrameData(uint8_t slaveAddr, uint16_t *frameData);
    int MLX90641_ExtractParameters(uint16_t *eeData, paramsMLX90641 *mlx90641);
    float MLX90641_GetVdd(uint16_t *frameData, const paramsMLX90641 *params);
    float MLX90641_GetTa(uint16_t *frameData, const paramsMLX90641 *params);
    void MLX90641_GetImage(uint16_t *frameData, const paramsMLX90641 *params, float *result);
    void MLX90641_CalculateTo(uint16_t *frameData, const paramsMLX90641 *params, float emissivity, float tr, float *result);
    int MLX90641_SetResolution(uint8_t slaveAddr, uint8_t resolution);
    int MLX90641_GetCurResolution(uint8_t slaveAddr);
    int MLX90641_SetRefreshRate(uint8_t slaveAddr, uint8_t refreshRate);   
    int MLX90641_GetRefreshRate(uint8_t slaveAddr);  
    int MLX90641_GetSubPageNumber(uint16_t *frameData);
    float MLX90641_GetEmissivity(const paramsMLX90641 *mlx90641);
    void MLX90641_BadPixelsCorrection(uint16_t pixel, float *to);
    
#endif