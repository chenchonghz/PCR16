/*
 * Copyright (C) ARM Limited, 2006. All rights reserved.
 * 
 * This is a retargeted I/O example which implements the functions required
 * for communication through an UART. The implementation relies on two UART
 * functions which the user must provide (UART_write and UART_read) for 
 * sending and receiving single characters to and from the UART.
 *
 * See the "rt_sys.h" header file for complete function descriptions.
 */

#include <rt_sys.h>
#include "includes.h"

#ifdef __cplusplus
extern "C" {
#endif

#define DEFAULT_HANDLE 0x100;

// 是否将fopen与FatFS关联起来
#define FATFS_EN 1
 
#if FATFS_EN
#include <ff.h>
#include <stdlib.h>
#endif
 
#pragma import(__use_no_semihosting) // 禁用半主机模式
#define STDIN 0
#define STDOUT 1
#define STDERR 2
#define IS_STD(fh) ((fh) >= 0 && (fh) <= 2)

/*
 * UART prototype functions
 */
char UART_read(void);
void UART_write(char);

char UART_read(void)
{
	return 0;
}

void UART_write(char ch)
{
		/* 写一个字节到USART1 */
	while((USART1->ISR &0X40)==0);//循环发送,直到发送完毕   
	USART1->TDR  = (unsigned char) ch;  
//	return ch;
}
/*
 * These names are special strings which will be recognized by 
 * _sys_open and will cause it to return the standard I/O handles, instead
 * of opening a real file.
 */
const char __stdin_name[] ="STDIN";
const char __stdout_name[]="STDOUT";
const char __stderr_name[]="STDERR";

/*
 * Open a file. May return -1 if the file failed to open. We do not require
 * this function to do anything. Simply return a dummy handle.
 */
FILEHANDLE _sys_open(const char * name, int openmode)
{
//    return DEFAULT_HANDLE;  
#if FATFS_EN
  BYTE mode;
  FIL *fp;
  FRESULT fr;
#endif
  if (name == __stdin_name)
	return STDIN;
  else if (name == __stdout_name)
  {
//    usart_init(); // 初始化串口 (在main函数执行前执行)
    return STDOUT;
  }
  else if (name == __stderr_name)
    return STDERR;
#if FATFS_EN
  if (sizeof(FILEHANDLE) < sizeof(void *))
  {
//    usart_send_string("sizeof(FILEHANDLE) should be no less than sizeof(void *)!\n", -1);
    return -1;
  }
  
  fp = user_malloc(sizeof(FIL));
  if (fp == NULL)
    return -1;
  
  if (openmode & OPEN_W)
  {
    mode = FA_CREATE_ALWAYS | FA_WRITE;
    if (openmode & OPEN_PLUS)
      mode |= FA_READ;
  }
  else if (openmode & OPEN_A)
  {
    mode = FA_OPEN_APPEND | FA_WRITE;
    if (openmode & OPEN_PLUS)
      mode |= FA_READ;
  }
  else
  {
    mode = FA_READ;
    if (openmode & OPEN_PLUS)
      mode |= FA_WRITE;
  }
  
  fr = f_open(fp, name, mode);
  if (fr == FR_OK)
    return (uintptr_t)fp;
  
  user_free(fp);
#endif
  
  return -1;
}

/*
 * Close a file. Should return 0 on success or a negative value on error.
 * Not required in this implementation. Always return success.
 */
int _sys_close(FILEHANDLE fh)
{
//    return 0; //return success
#if FATFS_EN
  FRESULT fr;
#endif
  
  if (IS_STD(fh))
  {
    if (fh == STDOUT)
//      usart_deinit();
    return 0;
  }
  
#if FATFS_EN
  fr = f_close((FIL *)fh);
  if (fr == FR_OK)
  {
    user_free((void *)fh);
    return 0;
  }
#endif
  
  return -1;
}

/*
 * Write to a file. Returns 0 on success, negative on error, and the number
 * of characters _not_ written on partial success. This implementation sends
 * a buffer of size 'len' to the UART.
 */
int _sys_write(FILEHANDLE fh, const unsigned char * buf,
               unsigned len, int mode)
{
    int i;
	
#if FATFS_EN
  FRESULT fr;
  UINT bw;
#endif
  
  if (fh == STDIN)
    return -1;
  
  if (fh == STDOUT || fh == STDERR)
  {
	for(i=0;i<len;i++) 
    {
        UART_write(buf[i]);        
        if(buf[i]=='\n') UART_write('\r');// Fix for HyperTerminal    
    }
    return 0;
  }
#if FATFS_EN
  fr = f_write((FIL *)fh, buf, len, &bw);
  if (fr == FR_OK)
    return len - bw;
#endif
  
  return -1;   
}

/*
 * Read from a file. Can return:
 *  - zero if the read was completely successful
 *  - the number of bytes _not_ read, if the read was partially successful
 *  - the number of bytes not read, plus the top bit set (0x80000000), if
 *    the read was partially successful due to end of file
 *  - -1 if some error other than EOF occurred
 * This function receives a character from the UART, processes the character
 * if required (backspace) and then echo the character to the Terminal 
 * Emulator, printing the correct sequence after successive keystrokes.  
 */
int _sys_read(FILEHANDLE fh, unsigned char * buf,
              unsigned len, int mode)
{
    int pos=0;
#if FATFS_EN
  FRESULT fr;
  UINT br;
#endif

	if (fh == STDIN)	{
		do {					
			buf[pos]=UART_read();			
			// Advance position in buffer
			pos++;			
			// Handle backspace
			if(buf[pos-1] == '\b') 
			{
				// More than 1 char in buffer
				if(pos>1)
				{
					// Delete character on terminal
					UART_write('\b');
					UART_write(' ');
					UART_write('\b');         
					
					// Update position in buffer
					pos-=2;   
				}
				else if (pos>0) pos--; // Backspace pressed, empty buffer
			}
			else UART_write(buf[pos-1]); // Echo normal char to terminal						
		}while(buf[pos-1] != '\r');		
		buf[pos]= '\0'; // Ensure Null termination
		return 0;     
	}
	else if (fh == STDOUT || fh == STDERR)	{
		return -1;
	}
#if FATFS_EN
  fr = f_read((FIL *)fh, buf, len, &br);
  if (fr == FR_OK)
    return len - br;
#endif
  
  return -1;      
}

/*
 * Writes a character to the output channel. This function is used
 * for last-resort error message output.
 */
void _ttywrch(int ch)
{
    // Convert correctly for endianness change
    char ench=ch;
    
    UART_write(ench);
}

/*
 * Return non-zero if the argument file is connected to a terminal.
 */
int _sys_istty(FILEHANDLE fh)
{
    return 1; // no interactive device present
}

/*
 * Move the file position to a given offset from the file start.
 * Returns >=0 on success, <0 on failure. Seeking is not supported for the 
 * UART.
 */
int _sys_seek(FILEHANDLE fh, long pos)
{
#if FATFS_EN
  FRESULT fr;
  
  if (!IS_STD(fh))
  {
    fr = f_lseek((FIL *)fh, pos);
    if (fr == FR_OK)
      return 0;
  }
#endif
    return -1; // error
}

/*
 * Flush any OS buffers associated with fh, ensuring that the file
 * is up to date on disk. Result is >=0 if OK, negative for an
 * error.
 */
int _sys_ensure(FILEHANDLE fh)
{
    return 0; // success
}

/*
 * Return the current length of a file, or <0 if an error occurred.
 * _sys_flen is allowed to reposition the file pointer (so Unix can
 * implement it with a single lseek, for example), since it is only
 * called when processing SEEK_END relative fseeks, and therefore a
 * call to _sys_flen is always followed by a call to _sys_seek.
 */
long _sys_flen(FILEHANDLE fh)
{
#if FATFS_EN
  if (!IS_STD(fh))
    return f_size((FIL *)fh);
#endif

    return 0;
}

/*
 * Return the name for temporary file number sig in the buffer
 * name. Returns 0 on failure. maxlen is the maximum name length
 * allowed.
 */
int _sys_tmpnam(char * name, int sig, unsigned maxlen)
{
    return 0; // fail, not supported
}

/*
 * Terminate the program, passing a return code back to the user.
 * This function may not return.
 */
void _sys_exit(int returncode)
{
    while(1) {};
}

#ifdef __cplusplus
}
#endif
