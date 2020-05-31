#ifndef __APP_USART_H
#define __APP_USART_H

#include "includes.h"

typedef struct _usart {
		UART_HandleTypeDef *port;
    OS_EVENT           *lock;
    //OS_EVENT           *resend_lock;
    OS_EVENT           *sem;
    OS_EVENT           *ack_sem;
    OS_EVENT           *mbox;

	INT8U		ack_cmd;
    volatile INT8U               rx_state;
    INT32U              rx_idx;
    INT32U              rx_cnt;
    INT32U              rx_len;
    INT32U              rx_bufsize;
    INT16U              rx_crc;
    INT16U              rx_err;
    INT8U              *rx_buf;
    void (*rx_indicate)(struct _usart *pUsart, INT8U rx_dat);

    volatile INT8U               tx_state;
    INT32U              tx_idx;
    INT32U              tx_len;
    INT32U              tx_bufsize;
    INT16U              tx_crc;
    INT16U              tx_err;
    INT8U              *tx_buf;
    //void (*tx_complete)(struct _usart *pUsart);
} usart_t;

extern usart_t      usart;
void UART1_DMA_Callback(DMA_HandleTypeDef *hdma);
void AppUsartInit(void);
void UsartSendAck(message_pkt_t *pMsg, INT8U ack);
#endif
