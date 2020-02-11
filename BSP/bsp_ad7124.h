/*
********************************************************************************
* bsp_ad7124.h
*
*   Copyright (C) 2010-2016 ACEA Biosciences, Inc. All rights reserved.
*   Author: AlexShi <shiweining123@163.com>
*
********************************************************************************
*/

#ifndef BSP_AD7124_H
#define BSP_AD7124_H

/*
********************************************************************************
* Included Files
********************************************************************************
*/

#include "bsp_spi.h"
#include "sys_types.h"
/*
********************************************************************************
* Pre-processor Definitions
********************************************************************************
*/

#define AD7124_R                                           DEF_High
#define AD7124_W                                            DEF_Low

#define DEF_INT_08_BITS                                          8u
#define DEF_INT_16_BITS                                         16u
#define DEF_INT_24_BITS                                         24u
#define DEF_INT_32_BITS                                         32u

#define AD7124_CLKSRC_ExtClock                                    1 // external clock
#define AD7124_CLKSRC_ExtCrystal                                  0 // external crystal
#define MODE_PSEUDO_DIFF_INPUT                                 TRUE
#define MODE_DIFF_INPUT                                       FALSE
#define DETECT_VOLTAGE                       MODE_PSEUDO_DIFF_INPUT
#define DETECT_CURRENT                              MODE_DIFF_INPUT

#define AD7124_PIN_RDY                               DEF_BIT02_MASK
#define AD7124_STATUS_RDY                            DEF_BIT07_MASK
#define AD7124_STATUS_ERR                            DEF_BIT06_MASK
#define AD7124_STATUS_NOREF                          DEF_BIT05_MASK
#define AD7124_STATUS_PARITY                         DEF_BIT04_MASK
#define AD7124_ERRCHK_None                     0
#define AD7124_ERRCHK_Warning                  1
#define AD7124_ERRCHK_Inactive                 2
#define AD7124_ERRCHK_Error                    3

#define DEF_CHANNEL(name)                   name, #name
#define	AD7124_ID		0X04

/*
********************************************************************************
* Shared Types
********************************************************************************
*/



/*
********************************************************************************
* Public Types
********************************************************************************
*/

enum _ad7124_id {
    AD7124_ID1 = 0
};
#define AD7124_ID_MIN     AD7124_ID1
#define AD7124_ID_MAX     AD7124_ID1
#define AD7124_ID_NUMS   (AD7124_ID_MAX-AD7124_ID_MIN+1)

#define MONITOR1_USE_AD7124     AD7124_ID1

enum _ad7124_channel {
    uCH_0    = 0x00,
    uCH_1    = 0x1,
    uCH_2    = 0x2,
    uCH_3    = 0x3,
    uCH_4    = 0x4,
    uCH_5    = 0x5,
    uCH_6    = 0x6,
    uCH_7    = 0x7,
    uCH_8    = 0x8,
    uCH_9    = 0x9,
    uCH_A    = 0xA,
    uCH_B    = 0xB,
    uCH_C    = 0xC,
    uCH_D    = 0xD,
    uCH_E    = 0xE,
    uCH_F    = 0xF,
};

enum _ad7124_AIN {
    AIN0    = 0x00,
    AIN1    = 0x1,
    AIN2    = 0x2,
    AIN3    = 0x3,
    AIN4    = 0x4,
    AIN5    = 0x5,
    AIN6    = 0x6,
    AIN7    = 0x7,
		AIN_TEMP	= 0X10,//温度采集
		AIN_AVSS	= 0X11,//AVss
		AIN_AVREF	= 0X12,//内部基准电压
		AIN_DGND	= 0X13,
		AIN_AVDD_AVSS1	= 0X14,
		AIN_AVDD_AVSS0	= 0X15,
};

enum _ad7124_config_x {
    CONFIG_0    = 0x00,
    CONFIG_1    = 0x1,
    CONFIG_2    = 0x2,
    CONFIG_3    = 0x3,
    CONFIG_4    = 0x4,
    CONFIG_5    = 0x5,
    CONFIG_6    = 0x6,
    CONFIG_7    = 0x7,
};

/*
enum _ad7124_channel {
    uCH_0    = 0x00,
    uCH_1    = 0x1,
    uCH_2    = 0x2,
    uCH_3    = 0x3,
    uCH_4    = 0x4,
    uCH_5    = 0x5,
    uCH_6    = 0x6,
    uCH_7    = 0x7,
    uCH_8    = 0x8,
    uCH_9    = 0x9,
    uCH_A    = 0xA,
    uCH_B    = 0xB,
    uCH_C    = 0xC,
    uCH_D    = 0xD,
    uCH_E    = 0xE,
    uCH_F    = 0xF,
};*/

enum _ad7124_err {
    AD7124_ERR_NONE     = 0,
    AD7124_ERR_BUSY     = 1,
    AD7124_ERR_ACTIVE   = 2,
    AD7124_ERR_NOREF    = 3,
    AD7124_ERR_INACTIVE = 4,
    AD7124_ERR_CHANNEL = 5
};

/* AD7124 Register List */
enum _ad7124_reg {
    AD7124_REG_COMMUNICATIONS               = 00,//COMM
    AD7124_REG_STATUS                       = 00,//STATUS
    AD7124_REG_CONTROL                         = 01,
    //AD7124_REG_CONFIGURATION                = 02,
    AD7124_REG_DATA                         = 02,
		IO_CONTROL1															= 03,
		IO_CONTROL2															= 04,
    AD7124_REG_ID                           = 05,//ID
    AD7124_REG_ERROR                       = 06,
    AD7124_REG_FULL_SCALE                   = 07,
		AD7124_REG_CHANNEL                   = 9,
		AD7124_REG_CONFIG                = 0x19,
};

/* AD7124 Operating Modes Register */
enum _ad7124_mode {
    AD7124_MODE_CONTINUOUS_CNV              = 00,
    AD7124_MODE_SINGLE_CNV                  = 01,
    AD7124_MODE_STANDYBY                    = 02,
    AD7124_MODE_POWER_DOWN                  = 03,
		AD7124_MODE_IDLE												= 04,
    AD7124_MODE_INTERNAL_ZERO_SCALE         = 05,
    AD7124_MODE_INTERNAL_FULL_SCALE         = 06,
    AD7124_MODE_SYSTEM_ZERO_SCALE           = 07,
    AD7124_MODE_SYSTEM_FULL_SCALE           = 8
};

enum _ad7124_gain_key {
    AD7124_GAIN_1_MUL                       = 00,
    AD7124_GAIN_2_MUL                       = 01,
    AD7124_GAIN_4_MUL                      = 02,
    AD7124_GAIN_8_MUL                      = 03,
    AD7124_GAIN_16_MUL                      = 04,
		AD7124_GAIN_32_MUL                      = 05,
		AD7124_GAIN_64_MUL                      = 06,
    AD7124_GAIN_128_MUL                     = 07
};
/*#define AD7124_GAIN_KV(gain)    {AD7124_GAIN_##gain##_MUL, gain}

typedef struct _ad7124_gain {
    CPU_INT08U key;
    CPU_INT08U value;
} ad7124_gain_t;*/

typedef union _ad7124_reg_comm {
    struct {
        //CPU_INT08U  RSVD   :2;          // Set to 0
        //CPU_INT08U  CRED   :1;          // Continuous read of the data register
        CPU_INT08U  RS     :6;          // Register address bits
        CPU_INT08U  RW     :1;          // Read/Write operation select
        CPU_INT08U  WEN    :1;          // Write enable bit
    } bits;
    CPU_INT08U ubyte;
} ad7124_reg_comm_t;

typedef union _ad7124_reg_status {
    struct {
        CPU_INT08U  CHD    :4;          // Indicate which channel corresponds to the data register contents
        CPU_INT08U  PARITY :1;          // Parity check of the data register
        CPU_INT08U  NOREF  :1;          // No external reference bit
        CPU_INT08U  ERR    :1;          // ADC error bit
        CPU_INT08U  RDY    :1;          // Ready bit for the ADC
    } bits;
    CPU_INT08U ubyte;
} ad7124_reg_status_t;

typedef union _ad7124_reg_mode {
    struct {
        CPU_INT32U  FS      :10;        // Filter output data rate select bits
        CPU_INT32U  REJ60   :1;         // Enable a notch at 60Hz
        CPU_INT32U  SINGLE  :1;         // Single cycle conversion enable bit
        CPU_INT32U  CLK_DIV :1;         // Clock divid-by-2
        CPU_INT32U  ENPAR   :1;         // Enable parity bit
        CPU_INT32U  RSVD    :1;         // Set to 0
        CPU_INT32U  SINC3   :1;         // Sinc3 filter select bit
        CPU_INT32U  AVG     :2;         // Fast settling filter
        CPU_INT32U  CLK     :2;         // Clock source select
        CPU_INT32U  DAT_STA :1;         // Enable the transmission of status register contents after ench data register read
        CPU_INT32U  MD      :3;         // Mode select bits
        CPU_INT32U  rsvd    :8;         // Unused
    } bits;
    CPU_INT32U  uword;
} ad7124_reg_mode_t;

typedef union _ad7124_reg_ctrl {
    struct {
				CPU_INT16U  CLK_SEL :2;         // Clock divid-by-2
        CPU_INT16U  Mode   :4;         // Enable parity bit
        CPU_INT16U  POWER_MODE    :2;         // Set to 0
        CPU_INT16U  REF_EN   :1;         // Sinc3 filter select bit
        CPU_INT16U  CS_EN     :1;         // Fast settling filter
        CPU_INT16U  DATA_STATUS    :1; //
        CPU_INT16U  CONT_READ :1;       //Continuous read of the data register 
        CPU_INT16U  DOUT_RDY_DEL      :1;        //set to 0
        CPU_INT16U  unused    :3;         // Unused
    } bits;
    CPU_INT16U  uword;
} ad7124_reg_ctrl_t;

typedef union _ad7124_reg_channel {
    struct {
				CPU_INT16U  AINM   :5;         // Set to 0
        CPU_INT16U  AINP  :5;         // Reference select bits
        CPU_INT16U  O   :2;         // Set to 0
        CPU_INT16U  SETUP    :3;         // Polarity select bit(0-bipolar, 1-unipolar)
        CPU_INT16U  EANBLE    :1;         // 
    } bits;
    CPU_INT16U  uword;
} ad7124_reg_channel_t;

typedef union _ad7124_reg_cfg {
    struct {
        CPU_INT16U  GAIN      :3;          // Gain select bits
        CPU_INT16U  REFSEL    :2;         // Reference select bits
        CPU_INT16U  AINBUFM   :1;         // Set to 0
        CPU_INT16U  AINBUFP  :1;         // Pseudo differential analog inputs
        CPU_INT16U  REFBUFM   :1;         // Set to 0
        CPU_INT16U  REFBUFP  :1;         // Reference select bits
        CPU_INT16U  BURNOUT   :2;         // Set to 0
        CPU_INT16U  U_B    :1;         // Polarity select bit(0-bipolar, 1-unipolar)
        CPU_INT16U  UNUSED    :4;         // Unused
    } bits;
    CPU_INT16U  uword;
} ad7124_reg_cfg_t;

typedef union _ad7124_reg_error {
    struct {
				CPU_INT16U  ROM_CRC_ERR   	:1; 
				CPU_INT16U  MM_CRC_ERR   	:1; 
				CPU_INT16U  SPI_CRC_ERR   	:1; 
				CPU_INT16U  SPI_WRITE_ERR   	:1; 
				CPU_INT16U  SPI_READ_ERR   	:1; 
				CPU_INT16U  SPI_SCLK_CNT_ERR   	:1;         // Set to 0
        CPU_INT16U  SPI_IGNORE_ERR  		:1;         // Reference select bits
				CPU_INT16U	ALDO_PSM_ERR	:1;
				CPU_INT16U  UNUSED1    :8;
				CPU_INT16U	ADC_SAT_ERR	:1;
        CPU_INT16U  ADC_CONV_ERR   :1;         // Set to 0
        CPU_INT16U  ADC_CAL_ERR    :1;         // Polarity select bit(0-bipolar, 1-unipolar)
        CPU_INT16U  LDO_CAP_ERR    :1;         // 
				CPU_INT16U  UNUSED    :4;         // Unused
    } bits;
    CPU_INT16U  uword;
} ad7124_reg_error_t;

typedef union _ad7124_reg_filter {
    struct {
        CPU_INT32U  FS  :11;         // Pseudo differential analog inputs
        CPU_INT32U  O   :5;         // Set to 0
        CPU_INT32U  SingleCycle  :1;         // Reference select bits
        CPU_INT32U  PostFilter   :3;         // Set to 0
        CPU_INT32U  REJ60    :1;         // Polarity select bit(0-bipolar, 1-unipolar)
        CPU_INT32U  Filter    :3;         // Unused
    } bits;
    CPU_INT32U  uword;
} ad7124_reg_filter_t;

#define UNIPOLAR    0u
#define BIPOLAR     1u

typedef struct _ad7124_reg_gpocon {
    struct {
        CPU_INT08U  PDAT    :4;         // Digital output
        CPU_INT08U  GP10EN  :1;         // Digital output P1 & Digital output P0 enable
        CPU_INT08U  GP32EN  :1;         // Digital output P3 & Digital output P2 enable
        INT8U       rsvd    :2;         // Reserved
    } bits;
    CPU_INT08U  ubyte;
} ad7124_reg_gpocon_t;

typedef struct _ad7124_chcfg {
    CPU_INT08U channel;         // Test Channel
   // const CPU_CHAR *chname;     // Channel name
    CPU_INT08U enable;       // idx
    CPU_INT08U config_idx;             // Voltage or Current
    CPU_INT08U U_B;             // Unipolar or Bipolar
    CPU_INT08U burnout;            // unit
		CPU_INT08U AINP;
		CPU_INT08U AINM;
    CPU_INT08S extgain;         // external gain
    //ad7124_gain_t intgain;      // AD internal gain
		CPU_INT08U intgain;	// AD internal gain
} ad7124_chcfg_t;

typedef struct _ad7124_threshold {
    cpu_bool_t  ischk;
    CPU_FP32    valwarn;
    CPU_FP32    valerr;
    CPU_FP32    valdelta;
} ad7124_threshold_t;

struct _ad7124_chinfo;
typedef int (*ad7124_check_t)(struct _ad7124_chinfo*, void*);
typedef struct _ad7124_chinfo {
    CPU_INT08U chidx;
    CPU_INT08U chnext;
    CPU_INT08U errchk;
    CPU_INT08U errint;
    CPU_INT32U adcode;
  //ad7124_chcfg_t *pcfg;
  //linear_scale_t *pscale;
    CPU_FP32   *pval;
    void *priv;
    ad7124_check_t check;
} ad7124_chinfo_t;

#define DEF_FLAGS_AD7124_INIT       DEF_BIT00_MASK
typedef struct _ad7124_dev {
    const CPU_INT08U id;
    //const CPU_INT08U p_spi;
    const CPU_INT08U clksrc;
    const CPU_FP32   vref;
    CPU_INT32U cs_mask;
    CPU_INT32U flags;
    spi_t 				*p_spi;
    void *priv;
} ad7124_dev_t;

/*
********************************************************************************
* Inline Functions
********************************************************************************
*/



/*
********************************************************************************
* Shared Data
********************************************************************************
*/



/*
********************************************************************************
* Public Data
********************************************************************************
*/



/*
********************************************************************************
* Public Function Prototypes
********************************************************************************
*/

#ifdef __cplusplus
#define EXTERN extern "C"
extern "C" {
#else
#define EXTERN extern
#endif

EXTERN void bsp_ad7124_reset(ad7124_dev_t *pdev);
EXTERN CPU_INT08U bsp_ad7124_status_get(ad7124_dev_t *pdev);
EXTERN CPU_INT08U bsp_ad7124_id_get(ad7124_dev_t *pdev);
EXTERN CPU_INT32U bsp_ad7124_mode_get(ad7124_dev_t *pdev);
EXTERN CPU_INT32U bsp_ad7124_cfg_get(ad7124_dev_t *pdev, u8 config_x);
EXTERN CPU_INT08U bsp_ad7124_gpocon_get(ad7124_dev_t *pdev);
EXTERN CPU_INT32U bsp_ad7124_offset_get(ad7124_dev_t *pdev);
EXTERN CPU_INT32U bsp_ad7124_fullscale_get(ad7124_dev_t *pdev);
CPU_INT32U bsp_ad7124_errorreg_get(ad7124_dev_t *pdev);
EXTERN void bsp_ad7124_control_set(ad7124_dev_t *pdev);
void bsp_ad7124_channel_set(ad7124_dev_t *pdev, const ad7124_chcfg_t *pcfg);
EXTERN void bsp_ad7124_cfg_set(ad7124_dev_t *pdev, const ad7124_chcfg_t *pcfg);
EXTERN ad7124_dev_t* bsp_ad7124_find(CPU_INT08U id);
EXTERN CPU_INT32U bsp_ad7124_value_get(ad7124_dev_t *pdev);
EXTERN ad7124_dev_t* bsp_ad7124_init(CPU_INT08U id);

CPU_INT32U bsp_ad7124_channel_get(ad7124_dev_t *pdev, u8 channel);
CPU_INT32U bsp_ad7124_control_get(ad7124_dev_t *pdev);
void bsp_ad7124_errorreg_set(ad7124_dev_t *pdev, u8 flag);
void bsp_ad7124_filterreg_set(ad7124_dev_t *pdev);
CPU_INT08U bsp_ad7124_conv_ready(ad7124_dev_t *pdev, CPU_INT08U *perr);
void ad7124_cs_low(void);
void ad7124_cs_high(void);
#undef EXTERN
#ifdef __cplusplus
}
#endif

#endif      /* __BSP_AD7124_H */

/*
********************************************************************************
* No More!
********************************************************************************
*/
