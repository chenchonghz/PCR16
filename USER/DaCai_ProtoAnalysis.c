#include "DaCai_ProtoAnalysis.h"
#include "app_display.h"

static _dacai_usart_t dacai_uart;//串口结构体
_DACAI_t  dacai;

u8 usart_rx_buf[DACAIUSART_RXSIZE];
//static u8 usart_tx_buf[DACAIUSART_TXSIZE];
static  message_pkt_t    msg_pkt_dacaiuart;
//extern DMA_HandleTypeDef hdma_uart4_tx;
extern const u8 CMD_END[4];
static void uart_message_rx_handler(_dacai_usart_t *pUsart, INT8U rx_dat);


//数据初始化
void DaCaiProto_init(void)
{
	dacai.phuart = &huart4;
	dacai.lock   = OSSemCreate(1);
	
	dacai.puart_t = &dacai_uart;
	dacai.puart_t->rx_buf = usart_rx_buf;
	dacai.puart_t->rx_bufsize = DACAIUSART_RXSIZE;
	dacai.puart_t->rx_idx = 0;
	dacai.puart_t->rx_cnt = 0;
	
	dacai.puart_t->tx_buf = (u8 *)tlsf_malloc(UserMem,DACAIUSART_TXSIZE);//usart_tx_buf;
	dacai.puart_t->tx_bufsize = DACAIUSART_TXSIZE;
	dacai.state = DEF_OFFLINE;
	dacai.puart_t->rx_indicate = &uart_message_rx_handler;

	__HAL_UART_DISABLE(dacai.phuart);
	__HAL_UART_DISABLE_IT(dacai.phuart, UART_IT_TXE); 
	__HAL_UART_DISABLE_IT(dacai.phuart, UART_IT_TC);
	__HAL_UART_ENABLE_IT(dacai.phuart, UART_IT_RXNE);          //使能串口接收FIFO非空中断
	
	__HAL_DMA_ENABLE_IT(dacai.phuart->hdmatx, DMA_IT_TC);
	__HAL_UART_ENABLE(dacai.phuart);
	__HAL_UART_CLEAR_FLAG(dacai.phuart, UART_CLEAR_TCF);
}

//串口接收数据帧解析
static void uart_message_rx_handler(_dacai_usart_t *pUsart, INT8U rx_dat)
{
    switch (pUsart->rx_state) {
        case PRO_RX_STATE_SD0:                     /* waiting for start first  start delimiter (SD0)  */
            if (rx_dat == DACAI_PROTOCOL_RX_SD0) {
                pUsart->rx_state = PRO_RX_STATE_DATA;
                pUsart->rx_crc	 = rx_dat;
                pUsart->rx_idx   = 0;
                pUsart->rx_cnt   = 0;
				pUsart->endflag = 0;
            }
            break;       
        case PRO_RX_STATE_DATA:                    /* waiting for data                                */
			if((rx_dat == DACAI_PROTOCOL_RX_END0)&&(pUsart->endflag==0))	{//查找是否帧尾
					pUsart->endflag++;              						
			}else if((rx_dat == DACAI_PROTOCOL_RX_END1)&&(pUsart->endflag==1))	{
					pUsart->endflag++;
			}else if((rx_dat == DACAI_PROTOCOL_RX_END2)&&(pUsart->endflag==2))	{
					pUsart->endflag++;
			}else if((rx_dat == DACAI_PROTOCOL_RX_END3)&&(pUsart->endflag==3))	{//帧尾解析ok
				pUsart->rx_state = PRO_RX_STATE_SD0;
				pUsart->endflag = 0;
				msg_pkt_dacaiuart.Src = USART_MSG_RX_TASK;
				StartAppDisplay(&msg_pkt_dacaiuart);//唤醒任务处理数据	
				break;
			}else {
				pUsart->endflag = 0;
			}
			if (pUsart->rx_cnt < pUsart->rx_bufsize) {
                pUsart->rx_buf[pUsart->rx_cnt++] = rx_dat;
            }
            break;
        default:
            pUsart->rx_state = PRO_RX_STATE_SD0;
            pUsart->rx_err   = MSG_ERR_STATE;
            break;
    }
}

//串口数据发送 使用DMA
void DaCai_SendData(u8 *buf, u16 length)
{
	u16 len = length,i;
	u8 *pbuf = buf;

	memcpy(pbuf+len, CMD_END, 4);
	len += 4;
	i=0;
	do	{
		if(HAL_UART_Transmit_DMA(dacai.phuart, pbuf, len)==HAL_OK)	{
			break;
		}else	{
			OSTimeDly(1);
		}
		i++;
	}while(i<10);
	OSTimeDly(5);
}

void DaCaiUART_DMA_Callback(DMA_HandleTypeDef *hdma)
{
	mutex_unlock(dacai.lock);
}

