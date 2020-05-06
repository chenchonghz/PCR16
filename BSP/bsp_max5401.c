#include "bsp_max5401.h"

//struct _max5401_port {
//	struct _io_map	cs;
//};

//static const struct _max5401_port g_max5401_port[MAX5401_ID_NUMS] = {
//    [MAX5401_ID1] = {
//        .cs = {MAX5401_CS1_GPIO_Port,  MAX5401_CS1_Pin}
//    },
//    [MAX5401_ID2] = {
//        .cs = {MAX5401_CS2_GPIO_Port,  MAX5401_CS2_Pin}
//    }
//};

max5401_dev_t	g_max5401_dev[MAX5401_ID_NUMS]	=	{
	[MAX5401_ID1] = {
		.id = MAX5401_ID1,
		.cs = {MAX5401_CS1_GPIO_Port,  MAX5401_CS1_Pin}
	},
	[MAX5401_ID2] = {
		.id = MAX5401_ID2,
		.cs = {MAX5401_CS2_GPIO_Port,  MAX5401_CS2_Pin}
	}
};

void MAX5401_init(void)
{
	max5401_dev_t  *pdev = &g_max5401_dev[MAX5401_ID1];

	if (pdev->flags & MAX5401_FLAGS_INIT) return;
	
    pdev->p_spi   = bsp_spi_get(SPI_ID1);
    pdev->flags |= MAX5401_FLAGS_INIT;
	
	pdev = &g_max5401_dev[MAX5401_ID2];
	if (pdev->flags & MAX5401_FLAGS_INIT) return;
	pdev->p_spi   = bsp_spi_get(SPI_ID1);
    pdev->flags |= MAX5401_FLAGS_INIT;
}

static INT32U MAX5401_spi_wr(max5401_dev_t *pdev, INT32U dat)
{
    INT32U rxdat;
    struct _io_map const *cs = &pdev->cs;
	
    bsp_spi_lock(pdev->p_spi);

    /* 设置片选信号 */
    SET_L(cs);

//    rxdat = bsp_spi_rw(pdev->p_spi, MSB_POSEDGE, 20, dat);
	bsp_spi_write(pdev->p_spi, MSB_POSEDGE, 8, dat);
	
    SET_H(cs);
    bsp_spi_unlock(pdev->p_spi);

    return rxdat;
}
//MAX5401阻值 = (100k/255 = 392.2) * res_cell
void MAX5401WriteResistor(u8 id, u32 res_cell)
{
	max5401_dev_t  *pdev = &g_max5401_dev[id];
	MAX5401_spi_wr(pdev, res_cell);
}

