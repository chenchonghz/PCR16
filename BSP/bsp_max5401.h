#ifndef BSP_MAX5401_H
#define BSP_MAX5401_H

#include "bsp_spi.h"

enum _max5401_id {
    MAX5401_ID1=0,
	MAX5401_ID2
};

#define MAX5401_ID_NUMS   (MAX5401_ID2+1)
#define MAX5401_FLAGS_INIT		1

typedef struct _max5401_dev {
    const CPU_INT08U id;
	spi_t *p_spi;
	 CPU_INT16U          flags;
	struct _io_map	cs;
}max5401_dev_t;

void MAX5401WriteResistor(max5401_dev_t *pdev, u32 res_cell);
	
#endif

