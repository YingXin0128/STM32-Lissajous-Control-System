#ifndef __AD9910_H__
#define __AD9910_H__

#include "stm32f10x.h"
#include "sys.h"

#define uchar unsigned char
#define uint  unsigned int
#define ulong unsigned long int

#define AD9910_PWR      PBout(11)
#define AD9910_SDIO     PAout(5)
#define AD9910_PLL_LOCK PCin(0)
#define DRHOLD          PCout(1)
#define DROVER          PCin(2)
#define UP_DAT          PCout(3)
#define PROFILE1        PCout(10)
#define MAS_REST        PAout(6)
#define SCLK            PAout(2)
#define DRCTL           PAout(4)
#define OSK             PCout(8)
#define PROFILE0        PCout(4)
#define PROFILE2        PCout(5)
#define CS              PBout(10)

typedef enum
{
    TRIG_WAVE = 0,
    SQUARE_WAVE,
    SINC_WAVE
} AD9910_WAVE_ENUM;

void AD9110_IOInit(void);
void Init_AD9910(void);
void AD9910_FreWrite(ulong frequency_hz);
void AD9910_AmpWrite(uint16_t asf);
void AD9910_PhaseWrite(uint16_t phase_deg);
uint32_t AD9910_FtwFromMilliHz(uint32_t frequency_millihz);
uint16_t AD9910_PowFromMilliDegrees(int32_t phase_millidegrees);
void AD9910_ProfileWriteRaw(uint32_t ftw, uint16_t pow, uint16_t asf);
void AD9910_SetToneMilliHz(uint32_t frequency_millihz,
                          int32_t phase_millidegrees,
                          uint16_t asf);
uint8_t AD9910_IsPllLocked(void);
void AD9910_DRG_AMP_Init(void);
void AD9910_RAM_WAVE_Set(AD9910_WAVE_ENUM wave,
                         uint32_t freq_hz,
                         uint8_t amp_percent);
void AD9910_DRG_FreInit_AutoSet(FunctionalState auto_sweep_enable);
void AD9910_DRG_FrePara_Set(u32 low_frequency,
                            u32 high_frequency,
                            u32 positive_step,
                            u32 negative_step,
                            u16 positive_rate,
                            u16 negative_rate);

#endif
