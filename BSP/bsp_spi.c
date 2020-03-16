/*
********************************************************************************
* bsp_spi.c
*
*   Copyright (C) 2010-2015 ACEA Biosciences, Inc. All rights reserved.
*   Author: jason guo
*
********************************************************************************
*/

/*
********************************************************************************
* Included Files
********************************************************************************
*/
#include "bsp_spi.h"

/*
********************************************************************************
* Pre-processor Definitions
********************************************************************************
*/

//#define SPI1_PORT_SCK           AD_SCLK_GPIO_Port
//#define SPI1_PIN_SCK            AD_SCLK_Pin

//#define SPI1_PORT_MISO	        AD_MISO_GPIO_Port
//#define SPI1_PIN_MISO	       	 AD_MISO_Pin

//#define SPI1_PORT_MOSI	        AD_MOSI_GPIO_Port
//#define SPI1_PIN_MOSI	        	AD_MOSI_Pin

#define SPI1_PORT_SCK           TMC260_SCK_GPIO_Port
#define SPI1_PIN_SCK            TMC260_SCK_Pin

#define SPI1_PORT_MISO	        TMC260_SDO_GPIO_Port
#define SPI1_PIN_MISO	       	 TMC260_SDO_Pin

#define SPI1_PORT_MOSI	        TMC260_SDI_GPIO_Port
#define SPI1_PIN_MOSI	        	TMC260_SDI_Pin

#define SPI2_PORT_SCK           TMC260_SCK_GPIO_Port
#define SPI2_PIN_SCK            TMC260_SCK_Pin

#define SPI2_PORT_MISO	        TMC260_SDO_GPIO_Port
#define SPI2_PIN_MISO	       	 TMC260_SDO_Pin

#define SPI2_PORT_MOSI	        TMC260_SDI_GPIO_Port
#define SPI2_PIN_MOSI	        	TMC260_SDI_Pin


/*
********************************************************************************
* Private Types
********************************************************************************
*/

typedef void (*iosw_t)(const struct _io_map *pio, cpu_bool_t sw);

struct _spi_4port {
    struct _io_map  SCK;
    struct _io_map  MISO;
    struct _io_map  MOSI;
};

struct _spi_3port {
    struct _io_map  SCK;
    struct _io_map  DIO;
    iosw_t iosw;
};

union _spi_port {
    struct _spi_4port   _4port;
    struct _spi_3port   _3port;
};

typedef struct _spi_priv {
    const CPU_INT08U nwire;
    const union _spi_port *const pport;
    void (*init)(struct _spi *pdev);
} spi_priv_t;

/*
********************************************************************************
* Private Function Prototypes
********************************************************************************
*/

static void iosw(const struct _io_map *pio, cpu_bool_t sw);
static CPU_INT32U spi_mp4wires_rw   (spi_t *pdev, CPU_INT08U nbits, CPU_INT32U txdat);
static CPU_INT32U spi_mn4wires_rw   (spi_t *pdev, CPU_INT08U nbits, CPU_INT32U txdat);
static CPU_INT32U spi_lp4wires_rw   (spi_t *pdev, CPU_INT08U nbits, CPU_INT32U txdat);
static CPU_INT32U spi_ln4wires_rw   (spi_t *pdev, CPU_INT08U nbits, CPU_INT32U txdat);
static CPU_INT32U spi_mp3wires_read (spi_t *pdev, CPU_INT08U nbits);
static void       spi_mp3wires_write(spi_t *pdev, CPU_INT08U nbits, CPU_INT32U txdat);
static CPU_INT32U spi_mn3wires_read (spi_t *pdev, CPU_INT08U nbits);
static void       spi_mn3wires_write(spi_t *pdev, CPU_INT08U nbits, CPU_INT32U txdat);
static CPU_INT32U spi_lp3wires_read (spi_t *pdev, CPU_INT08U nbits);
static void       spi_lp3wires_write(spi_t *pdev, CPU_INT08U nbits, CPU_INT32U txdat);
static CPU_INT32U spi_ln3wires_read (spi_t *pdev, CPU_INT08U nbits);
static void       spi_ln3wires_write(spi_t *pdev, CPU_INT08U nbits, CPU_INT32U txdat);

/*
********************************************************************************
* Private Data
********************************************************************************
*/

static const union _spi_port g_spi_port[SPI_ID_NUMS] = {
    [SPI_ID1]._4port = {
        .SCK = {SPI1_PORT_SCK,  SPI1_PIN_SCK},
        .MISO= {SPI1_PORT_MISO, SPI1_PIN_MISO},
        .MOSI= {SPI1_PORT_MOSI, SPI1_PIN_MOSI}
    },
    [SPI_ID2]._4port = {
        .SCK = {SPI2_PORT_SCK,  SPI2_PIN_SCK},
        .MISO= {SPI2_PORT_MISO, SPI2_PIN_MISO},
        .MOSI= {SPI2_PORT_MOSI, SPI2_PIN_MOSI}
    }
};

static spi_priv_t   g_spi_priv  [SPI_ID_NUMS] = {
    [SPI_ID1] = {
        .nwire = SPI_4Wires,
        .pport = &g_spi_port[SPI_ID1]
    },
    [SPI_ID2] = {
        .nwire = SPI_4Wires,
        .pport = &g_spi_port[SPI_ID2]
    }
};

static spi_t        g_spi       [SPI_ID_NUMS] = {
    [SPI_ID1] = {
        .id = SPI_ID1
    },
    [SPI_ID2] = {
        .id = SPI_ID2
    }
};

static CPU_INT32U (*const g_spi_rw_call[])(spi_t *pdev, CPU_INT08U nbits, CPU_INT32U txdat) = {
    spi_mp4wires_rw,
    spi_mn4wires_rw,
    spi_lp4wires_rw,
    spi_ln4wires_rw
};

static CPU_INT32U (*const g_spi_read_call[])(spi_t *pdev, CPU_INT08U nbits) = {
    spi_mp3wires_read,
    spi_mn3wires_read,
    spi_lp3wires_read,
    spi_ln3wires_read
};

static void (*const g_spi_write_call[])(spi_t *pdev, CPU_INT08U nbits, CPU_INT32U txdat) = {
    spi_mp3wires_write,
    spi_mn3wires_write,
    spi_lp3wires_write,
    spi_ln3wires_write
};

/*
********************************************************************************
* Private Functions
********************************************************************************
*/

/*
 * @brief
 *
 * @param
 *
 * @return
 *
 * @notes
 */
static void iosw(const struct _io_map *pio, cpu_bool_t sw)
{
    GPIO_InitTypeDef  GPIO_InitStructure;

    GPIO_InitStructure.Pin   = pio->pin;
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStructure.Mode  = (sw == DEF_OUT)? GPIO_MODE_OUTPUT_PP: GPIO_MODE_INPUT;
    HAL_GPIO_Init(pio->port, &GPIO_InitStructure);
}

static CPU_INT32U spi_mp4wires_rw(spi_t *pdev, CPU_INT08U nbits, CPU_INT32U txdat)
{
    #if OS_CRITICAL_METHOD == 3
    OS_CPU_SR   cpu_sr = 0;
    #endif
    spi_priv_t *priv = (spi_priv_t *)pdev->priv;

    CPU_INT08U  idx;
    CPU_INT32U  mark, rxdat = 0;
    const struct _io_map *SCK  = &priv->pport->_4port.SCK;
    const struct _io_map *MISO = &priv->pport->_4port.MISO;
    const struct _io_map *MOSI = &priv->pport->_4port.MOSI;

    /* 拉低时钟 */
    SET_L(SCK);
    /* 设定左边界 */
    mark = 1 << (nbits-1);
    OS_ENTER_CRITICAL();
    for (idx = 0; idx < nbits; idx++)
    {
        /* 准备发送一位数据 */
        if (txdat & mark) SET_H(MOSI);
        else SET_L(MOSI);
        /* 拉高时钟 */
        SET_H(SCK);
        /* 左移发送数据 */
        txdat <<= 1;
        /* 保存读取的数据 */
        rxdat <<= 1;
        if (IS_HIGH(MISO)) {
            rxdat |= 1;
        }
        /* 拉低时钟 */
        SET_L(SCK);
    }
    OS_EXIT_CRITICAL();

    return rxdat;
}

static CPU_INT32U spi_mn4wires_rw(spi_t *pdev, CPU_INT08U nbits, CPU_INT32U txdat)
{
    #if OS_CRITICAL_METHOD == 3
    OS_CPU_SR   cpu_sr = 0;
    #endif
    spi_priv_t *priv = (spi_priv_t *)pdev->priv;

    CPU_INT08U  idx;
    CPU_INT32U  mark, rxdat = 0;
    const struct _io_map *SCK  = &priv->pport->_4port.SCK;
    const struct _io_map *MISO = &priv->pport->_4port.MISO;
    const struct _io_map *MOSI = &priv->pport->_4port.MOSI;

    /* 拉低时钟 */
    SET_H(SCK);
    /* 设定左边界 */
    mark = 1 << (nbits-1);
    OS_ENTER_CRITICAL();
    for (idx = 0; idx < nbits; idx++)
    {
        /* 准备发送一位数据 */
        if (txdat & mark) SET_H(MOSI);
        else SET_L(MOSI);
        /* 拉高时钟 */
        SET_L(SCK);
        /* 左移发送数据 */
        txdat <<= 1;
        /* 保存读取的数据 */
        rxdat <<= 1;
        if (IS_HIGH(MISO)) {
            rxdat |= 1;
        }
        /* 拉低时钟 */
        SET_H(SCK);
    }
    OS_EXIT_CRITICAL();

    return rxdat;
}

static CPU_INT32U spi_lp4wires_rw(spi_t *pdev, CPU_INT08U nbits, CPU_INT32U txdat)
{
    #if OS_CRITICAL_METHOD == 3
    OS_CPU_SR   cpu_sr = 0;
    #endif
    spi_priv_t *priv = (spi_priv_t *)pdev->priv;

    CPU_INT08U  idx;
    CPU_INT32U  mark, rxdat = 0;
    const struct _io_map *SCK  = &priv->pport->_4port.SCK;
    const struct _io_map *MISO = &priv->pport->_4port.MISO;
    const struct _io_map *MOSI = &priv->pport->_4port.MOSI;

    /* 拉低时钟 */
    SET_L(SCK);
    /* 设定左边界 */
    mark = 1 << (nbits-1);
    OS_ENTER_CRITICAL();
    for (idx = 0; idx < nbits; idx++)
    {
        /* 准备发送一位数据 */
        if (txdat & 1) SET_H(MOSI);
        else SET_L(MOSI);
        /* 拉高时钟 */
        SET_H(SCK);
        /* 右移发送数据 */
        txdat >>= 1;
        /* 保存读取的数据 */
        rxdat >>= 1;
        if (IS_HIGH(MISO)) {
            rxdat |= mark;
        }
        /* 拉低时钟 */
        SET_L(SCK);
    }
    OS_EXIT_CRITICAL();

    return rxdat;
}

static CPU_INT32U spi_ln4wires_rw(spi_t *pdev, CPU_INT08U nbits, CPU_INT32U txdat)
{
    #if OS_CRITICAL_METHOD == 3
    OS_CPU_SR   cpu_sr = 0;
    #endif
    spi_priv_t *priv = (spi_priv_t *)pdev->priv;

    CPU_INT08U  idx;
    CPU_INT32U  mark, rxdat = 0;
    const struct _io_map *SCK  = &priv->pport->_4port.SCK;
    const struct _io_map *MISO = &priv->pport->_4port.MISO;
    const struct _io_map *MOSI = &priv->pport->_4port.MOSI;

    /* 拉低时钟 */
    SET_H(SCK);
    /* 设定左边界 */
    mark = 1 << (nbits-1);
    OS_ENTER_CRITICAL();
    for (idx = 0; idx < nbits; idx++)
    {
        /* 准备发送一位数据 */
        if (txdat & 1) SET_H(MOSI);
        else SET_L(MOSI);
        /* 拉高时钟 */
        SET_L(SCK);
        /* 左移发送数据 */
        txdat >>= 1;
        /* 保存读取的数据 */
        rxdat >>= 1;
        if (IS_HIGH(MISO)) {
            rxdat |= mark;
        }
        /* 拉低时钟 */
        SET_H(SCK);
    }
    OS_EXIT_CRITICAL();

    return rxdat;
}

static CPU_INT32U spi_mp3wires_read(spi_t *pdev, CPU_INT08U nbits)
{
    #if OS_CRITICAL_METHOD == 3
    OS_CPU_SR   cpu_sr = 0;
    #endif
    spi_priv_t *priv = (spi_priv_t *)pdev->priv;

    CPU_INT08U  idx;
    CPU_INT32U  rxdat = 0;
    const struct _io_map *SCK  = &priv->pport->_3port.SCK;
    const struct _io_map *DI   = &priv->pport->_3port.DIO;

    /* 拉低时钟 */
    SET_L(SCK);
    OS_ENTER_CRITICAL();
    for (idx = 0; idx < nbits; idx++)
    {
        /* 拉低时钟 */
        SET_L(SCK);
        /* 保存读取的数据 */
        rxdat <<= 1;
        /* 拉高时钟 */
        SET_H(SCK);
        if (IS_HIGH(DI)) {
            rxdat |= 1;
        }
    }
    OS_EXIT_CRITICAL();

    return rxdat;
}

static void spi_mp3wires_write(spi_t *pdev, CPU_INT08U nbits, CPU_INT32U txdat)
{
    #if OS_CRITICAL_METHOD == 3
    OS_CPU_SR   cpu_sr = 0;
    #endif
    spi_priv_t *priv = (spi_priv_t *)pdev->priv;

    CPU_INT08U  idx;
    CPU_INT32U  mark;
    const struct _io_map *SCK  = &priv->pport->_3port.SCK;
    const struct _io_map *DO   = &priv->pport->_3port.DIO;

    /* 拉低时钟 */
    SET_L(SCK);
    /* 设定左边界 */
    mark = 1 << (nbits-1);
    OS_ENTER_CRITICAL();
    for (idx = 0; idx < nbits; idx++)
    {
        /* 准备发送一位数据 */
        if (txdat & mark) SET_H(DO);
        else SET_L(DO);
        /* 拉低时钟 */
        SET_L(SCK);
        /* 左移发送数据 */
        txdat <<= 1;
        /* 拉高时钟 */
        SET_H(SCK);
    }
    OS_EXIT_CRITICAL();
}

static CPU_INT32U spi_mn3wires_read(spi_t *pdev, CPU_INT08U nbits)
{
    #if OS_CRITICAL_METHOD == 3
    OS_CPU_SR   cpu_sr = 0;
    #endif
    spi_priv_t *priv = (spi_priv_t *)pdev->priv;

    CPU_INT08U  idx;
    CPU_INT32U  rxdat = 0;
    const struct _io_map *SCK  = &priv->pport->_3port.SCK;
    const struct _io_map *DI   = &priv->pport->_3port.DIO;

    /* 拉高时钟 */
    SET_H(SCK);
    OS_ENTER_CRITICAL();
    for (idx = 0; idx < nbits; idx++)
    {
        /* 拉高时钟 */
        SET_H(SCK);
        /* 保存读取的数据 */
        rxdat <<= 1;
        /* 拉低时钟 */
        SET_L(SCK);
        if (IS_HIGH(DI)) {
            rxdat |= 1;
        }
    }
    OS_EXIT_CRITICAL();

    return rxdat;
}

static void spi_mn3wires_write(spi_t *pdev, CPU_INT08U nbits, CPU_INT32U txdat)
{
    #if OS_CRITICAL_METHOD == 3
    OS_CPU_SR   cpu_sr = 0;
    #endif
    spi_priv_t *priv = (spi_priv_t *)pdev->priv;

    CPU_INT08U  idx;
    CPU_INT32U  mark;
    const struct _io_map *SCK  = &priv->pport->_3port.SCK;
    const struct _io_map *DO   = &priv->pport->_3port.DIO;

    /* 拉高时钟 */
    SET_H(SCK);
    /* 设定左边界 */
    mark = 1 << (nbits-1);
    OS_ENTER_CRITICAL();
    for (idx = 0; idx < nbits; idx++)
    {
        /* 准备发送一位数据 */
        if (txdat & mark) SET_H(DO);
        else SET_L(DO);
        /* 拉高时钟 */
        SET_H(SCK);
        /* 左移发送数据 */
        txdat <<= 1;
        /* 拉低时钟 */
        SET_L(SCK);
    }
    OS_EXIT_CRITICAL();
}

static CPU_INT32U spi_lp3wires_read(spi_t *pdev, CPU_INT08U nbits)
{
    #if OS_CRITICAL_METHOD == 3
    OS_CPU_SR   cpu_sr = 0;
    #endif
    spi_priv_t *priv = (spi_priv_t *)pdev->priv;

    CPU_INT08U  idx;
    CPU_INT32U  mark, rxdat = 0;
    const struct _io_map *SCK  = &priv->pport->_3port.SCK;
    const struct _io_map *DI   = &priv->pport->_3port.DIO;

    /* 拉低时钟 */
    SET_L(SCK);
    /* 设定左边界 */
    mark = 1 << (nbits-1);
    OS_ENTER_CRITICAL();
    for (idx = 0; idx < nbits; idx++)
    {
        /* 拉低时钟 */
        SET_L(SCK);
        /* 保存读取的数据 */
        rxdat >>= 1;
        /* 拉高时钟 */
        SET_H(SCK);
        if (IS_HIGH(DI)) {
            rxdat |= mark;
        }
    }
    OS_EXIT_CRITICAL();

    return rxdat;
}

static void spi_lp3wires_write(spi_t *pdev, CPU_INT08U nbits, CPU_INT32U txdat)
{
    #if OS_CRITICAL_METHOD == 3
    OS_CPU_SR   cpu_sr = 0;
    #endif
    spi_priv_t *priv = (spi_priv_t *)pdev->priv;

    CPU_INT08U  idx;
    const struct _io_map *SCK  = &priv->pport->_3port.SCK;
    const struct _io_map *DO   = &priv->pport->_3port.DIO;

    /* 拉低时钟 */
    SET_L(SCK);
    OS_ENTER_CRITICAL();
    for (idx = 0; idx < nbits; idx++)
    {
        /* 准备发送一位数据 */
        if (txdat & 1) SET_H(DO);
        else SET_L(DO);
        /* 拉低时钟 */
        SET_L(SCK);
        /* 右移发送数据 */
        txdat >>= 1;
        /* 拉高时钟 */
        SET_H(SCK);
    }
    OS_EXIT_CRITICAL();
}

static CPU_INT32U spi_ln3wires_read(spi_t *pdev, CPU_INT08U nbits)
{
    #if OS_CRITICAL_METHOD == 3
    OS_CPU_SR   cpu_sr = 0;
    #endif
    spi_priv_t *priv = (spi_priv_t *)pdev->priv;

    CPU_INT08U  idx;
    CPU_INT32U  mark, rxdat = 0;
    const struct _io_map *SCK  = &priv->pport->_3port.SCK;
    const struct _io_map *DI   = &priv->pport->_3port.DIO;

    /* 拉高时钟 */
    SET_H(SCK);
    /* 设定左边界 */
    mark = 1 << (nbits-1);
    OS_ENTER_CRITICAL();
    for (idx = 0; idx < nbits; idx++)
    {
        /* 拉高时钟 */
        SET_H(SCK);
        /* 保存读取的数据 */
        rxdat >>= 1;
        /* 拉低时钟 */
        SET_L(SCK);
        if (IS_HIGH(DI)) {
            rxdat |= mark;
        }
    }
    OS_EXIT_CRITICAL();

    return rxdat;
}

static void spi_ln3wires_write(spi_t *pdev, CPU_INT08U nbits, CPU_INT32U txdat)
{
    #if OS_CRITICAL_METHOD == 3
    OS_CPU_SR   cpu_sr = 0;
    #endif
    spi_priv_t *priv = (spi_priv_t *)pdev->priv;

    CPU_INT08U  idx;
    const struct _io_map *SCK  = &priv->pport->_3port.SCK;
    const struct _io_map *DO   = &priv->pport->_3port.DIO;

    /* 拉高时钟 */
    SET_H(SCK);
    OS_ENTER_CRITICAL();
    for (idx = 0; idx < nbits; idx++)
    {
        /* 准备发送一位数据 */
        if (txdat & 1) SET_H(DO);
        else SET_L(DO);
        /* 拉高时钟 */
        SET_H(SCK);
        /* 右移发送数据 */
        txdat >>= 1;
        /* 拉低时钟 */
        SET_L(SCK);
    }
    OS_EXIT_CRITICAL();
}

/*
********************************************************************************
* Public Functions
********************************************************************************
*/

/*
 * @brief
 *
 * @param
 *
 * @return
 *
 * @notes
 */
void bsp_spi_lock(spi_t *pdev)
{
    CPU_INT08U os_err;

    OSSemPend(pdev->lock, 0, &os_err);
    //OSErrorCheck(os_err, __FUNCTION__, __LINE__);
}

void bsp_spi_unlock(spi_t *pdev)
{
    CPU_INT08U os_err;

    os_err = OSSemPost(pdev->lock);
    //OSErrorCheck(os_err, __FUNCTION__, __LINE__);

    (void)os_err;
}

void bsp_spi_init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    spi_t *pdev;

//	pdev = &g_spi[SPI_ID1];
//    /* 配置SPI引脚SCK、MISO 和 MOSI为复用推挽模式 */
//    GPIO_InitStruct.Pin   = SPI1_PIN_SCK|SPI1_PIN_MOSI;
//    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
//    GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;	/* 推挽输出模式 */
//    HAL_GPIO_Init(SPI1_PORT_SCK, &GPIO_InitStruct);

//    GPIO_InitStruct.Pin   = SPI1_PIN_MISO;
//    GPIO_InitStruct.Mode  = GPIO_MODE_INPUT;		/* MISO 设置为输入上拉 */
//		GPIO_InitStruct.Pull = GPIO_PULLUP;
//    HAL_GPIO_Init(SPI1_PORT_SCK, &GPIO_InitStruct);

//    pdev->flags &= ~SPI_FLAGS_HARD;
//    bsp_spi_open(SPI_ID1);

    pdev = &g_spi[SPI_ID2];
    /* 配置SPI引脚SCK 为复用推挽模式 */
    GPIO_InitStruct.Pin   = SPI2_PIN_SCK|SPI2_PIN_MOSI;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;	/* 推挽输出模式 */
    HAL_GPIO_Init(SPI2_PORT_SCK, &GPIO_InitStruct);

    GPIO_InitStruct.Pin   = SPI2_PIN_MISO;
    GPIO_InitStruct.Mode  = GPIO_MODE_INPUT;		
		GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(SPI2_PORT_SCK, &GPIO_InitStruct);

    pdev->flags &= ~SPI_FLAGS_HARD;
    bsp_spi_open(SPI_ID2);
}

spi_t *bsp_spi_get(CPU_INT08U id)
{
    spi_t *pdev = NULL;

    if (id < SPI_ID_NUMS) {
        pdev = &g_spi[id];
    }

    return pdev;
}

void bsp_spi_open(CPU_INT08U id)
{
    //DBG_Assert(id < SPI_ID_NUMS);

    spi_t  *pdev = &g_spi[id];
    spi_priv_t *priv = &g_spi_priv[id];

    if (pdev->flags & SPI_FLAGS_INIT) return;

  //priv->pport  = &g_spi_port[id];
    pdev->priv   = priv;
    pdev->lock   = OSSemCreate(1);

    pdev->flags |= SPI_FLAGS_INIT;
}

void bsp_spi_close(CPU_INT08U id)
{
    CPU_INT08U os_err;

    spi_t  *pdev = &g_spi[id];
    OSSemDel(pdev->lock, OS_DEL_ALWAYS, &os_err);
    pdev->flags &= ~SPI_FLAGS_INIT;
}

/* API for 4-wires SPI */
CPU_INT32U bsp_spi_rw(spi_t *pdev, enum _spi_mode mode, CPU_INT08U nbits, CPU_INT32U txdat)
{
    CPU_INT32U ret = 0;

    if (mode < SPI_MODE_MAX) {
        ret = g_spi_rw_call[mode](pdev, nbits, txdat);
    }

    return ret;
}

/* API for 3-wires SPI */
CPU_INT32U bsp_spi_read(spi_t *pdev, enum _spi_mode mode, CPU_INT08U nbits)
{
    spi_priv_t *priv = (spi_priv_t *)pdev->priv;
    iosw_t iosw_call = NULL;
    CPU_INT32U ret = 0;

    if (priv->nwire == SPI_3Wires) {
        iosw_call = priv->pport->_3port.iosw;
        iosw_call(&priv->pport->_3port.DIO, DEF_IN);
        if (mode < SPI_MODE_MAX) {
            ret = g_spi_read_call[mode](pdev, nbits);
        }
    }

    return ret;
}

/* API for 3-wires SPI */
void bsp_spi_write(spi_t *pdev, enum _spi_mode mode, CPU_INT08U nbits, CPU_INT32U txdat)
{
    spi_priv_t *priv = (spi_priv_t *)pdev->priv;
    iosw_t iosw_call = NULL;

    if (priv->nwire == SPI_3Wires) {
        iosw_call = priv->pport->_3port.iosw;
        iosw_call(&priv->pport->_3port.DIO, DEF_OUT);
        if (mode < SPI_MODE_MAX) {
            g_spi_write_call[mode](pdev, nbits, txdat);
        }
    }
}

CPU_INT32U ad7124_spi_read(spi_t *pdev, CPU_INT08U nbits)
{
    #if OS_CRITICAL_METHOD == 3
    OS_CPU_SR   cpu_sr = 0;
    #endif
    spi_priv_t *priv = (spi_priv_t *)pdev->priv;

    CPU_INT08U  idx;
    CPU_INT32U  rxdat = 0;
    const struct _io_map *SCK  = &priv->pport->_4port.SCK;
    const struct _io_map *MISO = &priv->pport->_4port.MISO;
    const struct _io_map *MOSI = &priv->pport->_4port.MOSI;

    OS_ENTER_CRITICAL();
    for (idx = 0; idx < nbits; idx++)
    {
        SET_L(SCK);
        /* 濞ｅ洦绻傞悺銊ф嫚鐠囨彃绲块柣銊ュ閺嗙喖骞戦敓锟? */
				rxdat <<= 1;
        /* 闁瑰嘲顦甸悵顕?寮崼鏇熷 */       
				if (IS_HIGH(MISO)) {
            rxdat |= 1;
        }
				SET_H(SCK);
    }
    OS_EXIT_CRITICAL();

    return rxdat;
}

void ad7124_spi_write(spi_t *pdev, CPU_INT08U nbits, CPU_INT32U txdat)
{
    #if OS_CRITICAL_METHOD == 3
    OS_CPU_SR   cpu_sr = 0;
    #endif
    spi_priv_t *priv = (spi_priv_t *)pdev->priv;

    CPU_INT08U  idx;
    CPU_INT32U  mark;
    const struct _io_map *SCK  = &priv->pport->_4port.SCK;
    const struct _io_map *MISO = &priv->pport->_4port.MISO;
    const struct _io_map *MOSI = &priv->pport->_4port.MOSI;

    /* 閻犱焦鍎抽悾鎯ь啅閿曚胶鐝堕柣锝忔嫹 */
    mark = 1 << (nbits-1);
    OS_ENTER_CRITICAL();
    for (idx = 0; idx < nbits; idx++)
    {
        /* 闁瑰嘲顦紞鍡涘籍閸洘瀵? */
        SET_L(SCK);
        /* 闁告垵妫楅ˇ顒勫矗閹达讣鎷锋担椋庮伇濞达絽绉甸弳鐔煎箲閿燂拷 */
        if (txdat & mark) SET_H(MOSI);
        else SET_L(MOSI);

        /* 鐎归潻濡囦簺闁告瑦鍨块敓鎴掔劍閺嗙喖骞戦敓锟? */
        txdat <<= 1;
        /* 闁瑰嘲顦甸悵顕?寮崼鏇熷 */
        SET_H(SCK);
				__NOP();//__NOP();
    }
    OS_EXIT_CRITICAL();
}
/*
********************************************************************************
* No More!
********************************************************************************
*/
