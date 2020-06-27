#include "app_usart.h"

//堆栈
__align(4) OS_STK      AppUsartRxStk       [STK_SIZE_USART_RX];           // Usart接收任务堆栈
__align(4) OS_STK      AppUsartTxStk       [STK_SIZE_USART_TX];           // Usart发送任务堆栈

#define USART_TXBUFF_SIZE		1036
#define USART_RXBUFF_SIZE		1036
usart_t      usart;

message_pkt_t    msg_pkt_usart[2];
static INT8U       usart_rx_buf        [USART_RXBUFF_SIZE];
static INT8U       usart_tx_buf        [USART_TXBUFF_SIZE];
static void AppUsartTxTask(void *parg);
static void AppUsartRxTask(void *parg);
static void uart_message_rx_handler(usart_t *pUsart, INT8U rx_dat);

void AppUsartInit(void)
{
	OSTaskCreate(AppUsartTxTask,  (void * )0, (OS_STK *)&AppUsartTxStk[STK_SIZE_USART_TX-1], TASK_PRIO_USART_TX);
	OSTaskCreate(AppUsartRxTask,  (void * )0, (OS_STK *)&AppUsartRxStk[STK_SIZE_USART_RX-1], TASK_PRIO_USART_RX);
}

static void UsartInit (void)
{
    //INT8U err;
	usart.port 		   = &huart2;
    usart.lock         = OSSemCreate(1);
    usart.sem          = OSSemCreate(0);
//    usart.ack_sem 	   = OSSemCreate(0);
    usart.mbox         = OSMboxCreate((void *)0);
    usart.rx_state     = PRO_RX_STATE_SD0;
    usart.rx_idx       = 0;
    usart.rx_cnt       = 0;
    usart.rx_len       = 0;
    usart.rx_bufsize   = USART_RXBUFF_SIZE;
    usart.rx_crc       = 0;
    usart.rx_err       = MSG_ERR_NONE;
    usart.rx_buf       = usart_rx_buf;
    usart.tx_state     = PRO_TX_STATE_SD0;
    usart.tx_idx       = 0;
    usart.tx_len       = 0;
    usart.tx_bufsize      = USART_TXBUFF_SIZE;
    usart.tx_crc       = 0;
    usart.tx_err       = MSG_ERR_NONE;
    usart.tx_buf       = usart_tx_buf;
    usart.rx_indicate  = &uart_message_rx_handler;
    //usart.tx_complete  = &uart_message_tx_handler;
}

static void uart_message_rx_handler(usart_t *pUsart, INT8U rx_dat)
{
    switch (pUsart->rx_state) {
        case PRO_RX_STATE_SD0:                     /* waiting for start first  start delimiter (SD0)  */
            if (rx_dat == PROTOCOL_RX_SD0) {
                pUsart->rx_state = PRO_RX_STATE_SD1;
                pUsart->rx_crc	 = rx_dat;
                pUsart->rx_idx   = 0;
                pUsart->rx_cnt   = 0;
            }
            break;
        case PRO_RX_STATE_SD1:                     /* waiting for start second start delimiter (SD1)  */
		    if (rx_dat == PROTOCOL_RX_SD1) {
                pUsart->rx_state = PRO_RX_STATE_LEN0;
                pUsart->rx_crc	 += rx_dat;
            } else {
                pUsart->rx_state = PRO_RX_STATE_SD0;
            }
            break;
        case PRO_RX_STATE_LEN0:                    /* waiting for 'len' HIGH byte                      */
            pUsart->rx_len       = rx_dat&0xff;
            pUsart->rx_crc       += rx_dat;
            pUsart->rx_state     = PRO_RX_STATE_LEN1;
            break;
        case PRO_RX_STATE_LEN1:                    /* waiting for 'len' LOW byte                     */
            pUsart->rx_len      |= rx_dat<<8;
            if ((pUsart->rx_len < PRO_EXTENT_LEN) || (pUsart->rx_len > pUsart->rx_bufsize)) {
                pUsart->rx_state = PRO_RX_STATE_SD0;/* Can not handle this size ...                    */
                pUsart->rx_err   = MSG_ERR_LENGTH;
               // usart_rx_int_disable(pUsart->base);
                OSSemPost(pUsart->sem);//解析错误
            } else {
                pUsart->rx_crc  += rx_dat;
                pUsart->rx_state = PRO_RX_STATE_DATA;
            }
            break;
        case PRO_RX_STATE_DATA:                    /* waiting for data                                */
            if (pUsart->rx_cnt < pUsart->rx_bufsize) {
                pUsart->rx_buf[pUsart->rx_cnt++] = rx_dat;
            }
            pUsart->rx_crc += rx_dat;
            if ((pUsart->rx_len - PRO_END_LEN) <= pUsart->rx_cnt) {
                pUsart->rx_state = PRO_RX_STATE_CHKSUM0;
            }
            break;

        case PRO_RX_STATE_CHKSUM0:                 /* waiting for checksum HIGH byte                   */
            if ((pUsart->rx_crc & 0xff) == rx_dat) {
                pUsart->rx_state = PRO_RX_STATE_CHKSUM1;
            } else {
                pUsart->rx_state = PRO_RX_STATE_SD0;
                pUsart->rx_err   = MSG_ERR_CHECKSUM;
                //usart_rx_int_disable(pUsart->base);
                OSSemPost(pUsart->sem);//解析错误
            }
            break;

        case PRO_RX_STATE_CHKSUM1:                 /* waiting for checksum                            */
             if ((pUsart->rx_crc >>8) == rx_dat) {
                pUsart->rx_state = PRO_RX_STATE_END;
            } else {
                pUsart->rx_state = PRO_RX_STATE_SD0;
                pUsart->rx_err   = MSG_ERR_CHECKSUM;
                //usart_rx_int_disable(pUsart->base);
                OSSemPost(pUsart->sem);//解析错误
            }
            break;

        case PRO_RX_STATE_END:
            if (rx_dat != PROTOCOL_RX_END) {   /* End delimiter ?                                 */
                pUsart->rx_err = MSG_ERR_ETX_WORD;
                //usart_rx_int_disable(pUsart->base);
            }
            OSSemPost(pUsart->sem);//解析成功
            pUsart->rx_state = PRO_RX_STATE_SD0;
            break;

        default:
            pUsart->rx_state = PRO_RX_STATE_SD0;
            pUsart->rx_err   = MSG_ERR_STATE;
            //usart_rx_int_disable(pUsart->base);
            OSSemPost(pUsart->sem);//解析错误
            break;
    }
}

void UsartSendAck(message_pkt_t *pMsg, INT8U ack)
{
	static INT8U ack_code[2];
		
	ack_code[0] = pMsg->Cmd;
	ack_code[1] = ack;
	pMsg->Cmd = _CMD_ACK;
	pMsg->Data   = &ack_code;
    pMsg->dLen= 2;
    OSMboxPost(usart.mbox, pMsg);
}

static void UsartSendData(message_pkt_t *pmsg)
{
	u8 idx,datlen,retry_cnt;
	u16 temp;
	usart_t *pUsart = &usart;

	mutex_lock(pUsart->lock);
	idx=0;
	pUsart->tx_buf[idx++] = PROTOCOL_RX_SD0;
	pUsart->tx_buf[idx++] = PROTOCOL_RX_SD1;
	datlen = pmsg->dLen + PRO_EXTENT_LEN; //SENSOR_EXTENT_LEN = 1字节命令 + 2字节校验和 + 1字节结束符
	pUsart->tx_buf[idx++] = datlen&0xff;
	pUsart->tx_buf[idx++] = datlen>>8;
	pUsart->tx_buf[idx++] = pmsg->Cmd;
	if (pmsg->dLen) {
		memcpy(pUsart->tx_buf + idx, pmsg->Data, pmsg->dLen);
    }
	idx += pmsg->dLen;
	temp = CRC16_Sum(pUsart->tx_buf,idx);
	pUsart->tx_buf[idx++] = temp&0xff;
	pUsart->tx_buf[idx++] = temp>>8;
	pUsart->tx_buf[idx++] = PROTOCOL_RX_END;
	pUsart->tx_len = idx;
	retry_cnt = 0;
	do	{
		if(HAL_UART_Transmit_DMA(pUsart->port, pUsart->tx_buf, pUsart->tx_len)==HAL_OK)	{
			break;
		}else	{
			OSTimeDly(1);
		}
		retry_cnt++;
		if(retry_cnt>10)	{
			mutex_unlock(usart.lock);
			break;
		}
	}while(1);
}

extern u8 UsartCmdProcess(usart_t *pUsart, message_pkt_t msg[]);
static  void  UsartCmdParsePkt (usart_t *pUsart)
{
    INT8U cmd,ack;

    msg_pkt_usart[0].Src = USART_MSG_RX_TASK;//任务间通讯
	msg_pkt_usart[1].Src = USART_MSG_RX_TASK;//指令回复
    cmd = UsartRxGetINT8U(pUsart->rx_buf,&pUsart->rx_idx);                                 /* First byte contains command      */

    if (pUsart->rx_err == MSG_ERR_NONE) {
        msg_pkt_usart[0].Cmd = cmd;
		msg_pkt_usart[1].Cmd = cmd;
    } else {
        //UsartSendError(&msg_pkt_usart[0], pUsart->rx_err);
        pUsart->rx_err = MSG_ERR_NONE;        // clear rx error
        return;
    }
	ack = UsartCmdProcess(pUsart, msg_pkt_usart);
	if(ack != ACK_NONE)	{
		UsartSendAck(&msg_pkt_usart[1],ack);
	}
}

static void AppUsartRxTask(void *parg)
{
    INT8U err;

	__HAL_UART_DISABLE(usart.port);
	__HAL_UART_DISABLE_IT(usart.port, UART_IT_TXE); 
	__HAL_UART_DISABLE_IT(usart.port, UART_IT_TC);
	__HAL_UART_ENABLE_IT(usart.port, UART_IT_RXNE);          //使能串口接收FIFO非空中断
	
	__HAL_DMA_ENABLE_IT(usart.port->hdmatx, DMA_IT_TC);
	__HAL_UART_ENABLE(usart.port);
	__HAL_UART_CLEAR_FLAG(usart.port, UART_CLEAR_TCF);
	
    for(;;)
    {
        OSSemPend(usart.sem, 0, &err);
        if(err==OS_ERR_NONE)    {
            UsartCmdParsePkt(&usart);
            __HAL_UART_ENABLE_IT(usart.port, UART_IT_RXNE);
        }
    }
}

/*******************************************************************************************************
*                                              Usart发送任务
********************************************************************************************************/
static void AppUsartTxTask(void *parg)
{
    INT8U err;
    message_pkt_t *msg;

    UsartInit();
	
    for(;;)
    {
        msg = (message_pkt_t *)OSMboxPend(usart.mbox, 0, &err);
		if(err==OS_ERR_NONE)    {
			if (msg->Src == USART_MSG_RX_TASK)	{
				UsartSendData(msg);
			}
		}else if(err==OS_ERR_TIMEOUT)	{
			
		}
    }
}

void UART2_DMA_Callback(DMA_HandleTypeDef *hdma)
{
	mutex_unlock(usart.lock);
}

