/*
********************************************************************************
* bsp_spi.h
*
*   Copyright (C) 2010-2015 ACEA Biosciences, Inc. All rights reserved.
*   Author: AlexShi <shiweining123@163.com>
*
********************************************************************************
*/

#ifndef __BSP_SPI_H
#define __BSP_SPI_H

/*
********************************************************************************
* Included Files
********************************************************************************
*/
#include "includes.h"


/*
********************************************************************************
* Pre-processor Definitions
********************************************************************************
*/



/*
********************************************************************************
* Public Types
********************************************************************************
*/

enum _spi_type {
    SPI_Soft = 0,       // 软件SPI
    SPI_Hard = 1        // 硬件SPI
};

enum _spi_nwire {
    SPI_3Wires  = 0,    // 3 wires
    SPI_4Wires  = 1     // 4 wires
};

enum _spi_mode {
    MSB_POSEDGE = 0,    // MSB优先, 上升沿发送
    MSB_NEGEDGE = 1,    // MSB优先, 下降沿发送
    LSB_POSEDGE = 2,    // LSB优先, 上升沿发送
    LSB_NEGEDGE = 3,    // LSB优先, 下降沿发送
    SPI_MODE_MAX
};

enum _spi_id {
    SPI_ID1 = 0,
    SPI_ID2 = 1
};
#define SPI_ID_MIN          SPI_ID1
#define SPI_ID_MAX          SPI_ID2
#define SPI_ID_NUMS        (SPI_ID_MAX-SPI_ID_MIN+1)
/*
struct _spi;
typedef struct _spi_ops {
    struct _spi* (*open)(CPU_INT08U id);
    void (*close)(CPU_INT08U id);
    cpu_err_t (*control)(struct _spi *pdev, int cmd, void *arg);
} spi_ops_t;
*/
#define SPI_FLAGS_INIT      DEF_BIT00_MASK
#define SPI_FLAGS_HARD      DEF_BIT01_MASK
typedef struct _spi {
    const CPU_INT08U    id;
    CPU_INT16U          flags;
    OS_EVENT           *lock;
  //const spi_ops_t    *ops;
    void               *priv;
} spi_t;

/*
********************************************************************************
* Inline Functions
********************************************************************************
*/



/*
********************************************************************************
* Public Variables
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

void bsp_spi_lock   (spi_t *pdev);
void bsp_spi_unlock (spi_t *pdev);
void bsp_spi_init   (void);
spi_t *bsp_spi_get  (CPU_INT08U id);
void bsp_spi_open   (CPU_INT08U id);
void bsp_spi_close  (CPU_INT08U id);
/* API for 4-wires SPI */
CPU_INT32U bsp_spi_rw(spi_t *pdev, enum _spi_mode mode, CPU_INT08U nbits, CPU_INT32U txdat);
/* API for 3-wires SPI */
CPU_INT32U bsp_spi_read(spi_t *pdev, enum _spi_mode mode, CPU_INT08U nbits);
/* API for 3-wires SPI */
void bsp_spi_write(spi_t *pdev, enum _spi_mode mode, CPU_INT08U nbits, CPU_INT32U txdat);
CPU_INT32U ad7124_spi_read(spi_t *pdev, CPU_INT08U nbits);
void ad7124_spi_write(spi_t *pdev, CPU_INT08U nbits, CPU_INT32U txdat);
#undef EXTERN
#ifdef __cplusplus
}
#endif

#endif      /* __BSP_SPI_H */

/*
********************************************************************************
* No More!
********************************************************************************
*/
