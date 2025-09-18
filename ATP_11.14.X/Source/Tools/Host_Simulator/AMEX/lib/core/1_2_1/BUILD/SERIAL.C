/******************************************************************************
  
       Module: serial.c

        Title: Pinnacle Dialog Manager

  Description: This is a modified file of original serial.c.
               Error logging to DB is replaced by logging to NT's
               event log
  
  $Log:   N:\PVCS\PTE\CORE\ptecom_lib\SERIAL.C  $
   
      Rev 1.4   Aug 26 1998 09:41:58   MSALEH
   added another state to the COMM setup tab
   the new value is TCPNOTPDU, allows processing
   transactions without a TPDU
   
      Rev 1.3   Mar 25 1998 15:30:58   drodden
   Reworked the comm library a little to:
     - have an easier set ot headers to use.
     - prototyped all functions in a header for each module.
     - moved the shared type definitions and global variables
       to common.c  (and .h).
     - use PTE basic data types whever possible.
   
   
      Rev 1.2   Mar 25 1998 14:51:02   drodden
   Created a common communication library (ptecom).
   
   
      Rev 1.1  Jan 29 1998 09:44:08    JXIAO
   Make necessary changes to make dialog manager
   working with AppLnk


      Rev 1.0  Dec 23 1997 15:05:00    JXIAO
   Initial version of Dialog Manager NT Service version.
   It is a converting of Passport Dialog Manager. All functionalities
   are kept, all GUI are removed.

******************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <process.h>

#include "serial.h"
#include "NTUtils.h"


#define STATE_STX         1
#define STATE_DATA_LEN1  2
#define STATE_DATA_LEN2  3
#define STATE_DATA       4
#define STATE_ETX         5
#define STATE_LRC         6
#define STATE_RESTART    7

#define STX           0x02
#define ETX           0x03
#define EOT           0x04
#define ENQ           0x05
#define ACK           0x06
#define NACK          0x15

#define WAIT_INTERVAL    0              // relinquish rest of time slice

DM_SERIAL_STRUCT      com;

// Read a byte from the serial port
BOOLEAN serial_get_byte(pBYTE ch, LPDWORD len)
{ 
   BOOLEAN    rc;
   DWORD   bytestoread = 1, err;
   COMSTAT ComStat ;
   DWORD   dwErrorFlags;

   ClearCommError(COMDEV( com ) , &dwErrorFlags, &ComStat );

  *len = *ch = 0x00;
  
   // start an overlapped read from the serial port
   rc = ReadFile(COMDEV( com ), ch, bytestoread, len, &(READ_OS( com )));
   if(!rc) 
   { 
     err = GetLastError();
      if( err == ERROR_IO_PENDING)
      {        
         // read has not completed yet, so wait a WAIT_INTERVAL
         Sleep(WAIT_INTERVAL);
         
         // try again with a blocking read
         // We have to wait for read to complete. 
         // This function will timeout according to the
         // CommTimeOuts.ReadTotalTimeoutConstant variable
         // Every time it times out, check for port errors
         
         while(!GetOverlappedResult(COMDEV( com ), &(READ_OS( com )), len, TRUE))
         {        
           err = GetLastError();             // problem in blocking read?
           if (err == ERROR_IO_INCOMPLETE)   // normal result if not finished
             continue;
           else
           {
          LogEvent("PINNACL: serial_get_byte, get overlapped result failed", ERROR_MSG);
          ClearCommError( COMDEV( com ), &dwErrorFlags, &ComStat ) ;
              *len = *ch = 0x00;
              return(FALSE);
           }
         }
      }
      else
      {
        // some other error occurred
      LogEvent("PINNACLE: serial get byte error occurred", ERROR_MSG);
         ClearCommError( COMDEV( com ), &dwErrorFlags, &ComStat ) ;
         *len = *ch = 0x00;
         return(FALSE);            // problem in overlapped read
      }
   }
   
   return(TRUE);                    // overlapped read completed
}

// Write data to the serial port.

BOOLEAN serial_put_bytes(pBYTE lpBuf, const DWORD dwLen)
{
   DWORD   outlen, err;
   BOOLEAN    rc;
   DWORD   dwErrorFlags;
   COMSTAT ComStat;
   
   // start an overlapped write to the serial port
   rc = WriteFile(COMDEV( com ), lpBuf, dwLen, &outlen, &(WRITE_OS( com )));

   // Note that normally the code will not execute the following
   // because the driver caches write operations. Small I/O requests 
   // (up to several thousand bytes) will normally be accepted 
   // immediately and WriteFile will return true even though an
   // overlapped operation was specified

   if(!rc)
   {  
      err = GetLastError();
      
      if(err == ERROR_IO_PENDING)
      {
        // We should wait for the completion of the write operation
         // so we know if it worked or not

         // If the write takes long enough to complete, this 
         // function will timeout according to the
         // CommTimeOuts.WriteTotalTimeoutConstant variable.
         // At that time we can check for errors and then wait 
         // some more.

         // write has not completed yet, so wait a WAIT_INTERVAL
         Sleep(WAIT_INTERVAL);

         // try again with a blocking write
         while(!GetOverlappedResult(COMDEV( com ), &(WRITE_OS( com )), &outlen, TRUE))
         {
            err = GetLastError();
            if(err == ERROR_IO_INCOMPLETE)
               // normal result if not finished
               continue;
            else
            {
          LogEvent("PINNACLE: serial_put_byte, overlapped result failed", ERROR_MSG);
          ClearCommError( COMDEV( com ), &dwErrorFlags, &ComStat ) ;
              return(FALSE);              // problem with blocking write
            }
         }
      }
      else
      {
        // some other error occurred
      LogEvent("PINNACLE: serial_put_byte. other error occured", ERROR_MSG);
         ClearCommError( COMDEV( com ), &dwErrorFlags, &ComStat ) ;
         return(FALSE);                // prob with nonblocking write
      }
   }

   return(TRUE);                       // good write
}

// Read a formatted byte stream from the serial port.
void serial_read(pIPC_INFO ipc_info)
{
   static BYTE bLen[2];   // message length received
   static WORD wLength;   // message length converted to bin
   static BYTE bLrc;      // Current Lrc value
   static WORD wBufidx;   // index into InMsgBuf to hold message

   BYTE  bTmp, bState = STATE_STX;
   
  char   cStr[255];
   
  pBYTE  lpByteBuffer;
   DWORD   bytesread;
  DWORD   dwEvtMask ;

  // Idiot check the receive buffer
   if(!lpBuf)
   {
    LogEvent("PINNACLE: serial_read bad buffer pointer", ERROR_MSG);
      return;
   }

  login_xipc(DM_PORTTYPE_SERIAL, ipc_info->instancename, ipc_info->loginname);
   
  while ( CONNECTED( com ) )
  {
     dwEvtMask = 0 ;

      WaitCommEvent( COMDEV( com ), &dwEvtMask, NULL );

      if ((dwEvtMask & EV_RXCHAR) == EV_RXCHAR)
      {
       while(serial_get_byte(&bTmp, &bytesread) > 0)
       { 
         if (!bytesread) break;
     
          switch(bState)
          {
             case STATE_STX:
             if(bTmp == STX)
             {
               bState = STATE_DATA_LEN1;
             }
             else 
              if(bTmp == ENQ)
             {
                 // connection test poll from NAC - send EOT 
                 BYTE bEot = EOT;
                 serial_put_bytes(&bEot, 1);
             }
             else 
               if(bTmp != ACK) // Acknowledge from terminal emulator
             {
            sprintf(cStr, "PINNACLE: serial_read communication error, expecting STX, received %0.2X.", bTmp);
            LogEvent(cStr, ERROR_MSG);
                  bState = STATE_STX;
             }
             break;

             case STATE_DATA_LEN1:
             bLen[0] = (BYTE)bTmp;
             bState = STATE_DATA_LEN2;
             break;

             case STATE_DATA_LEN2:
             bLen[1] = (BYTE)bTmp;
             wLength = (WORD)bcd2bin(bLen);
             if(wLength > wBufLen)
           {
            LogEvent("PINNACLE: serial_read, communication error. maximum message length exceeded", ERROR_MSG);
            bState = STATE_RESTART;
                 break;
             }

             bLrc = (BYTE)(bLen[0] ^ bLen[1]);
             wBufidx = 0;
              lpByteBuffer = (pBYTE)lpBuf;
             bState = wLength ? STATE_DATA : STATE_ETX;
             break;

             case STATE_DATA:
             lpByteBuffer[wBufidx++] = bTmp;
             bLrc ^= bTmp;

             if(wBufidx >= wLength)
               bState = STATE_ETX;

             break;

             case STATE_ETX:
             if(bTmp != ETX)
             {
            sprintf(cStr, "PINNACLE: serial_read communication error, expecting ETX, received %0.2X.", bTmp);
            LogEvent(cStr, ERROR_MSG);
                 bState = STATE_RESTART;
             }
             else
             {
               bLrc ^= bTmp;
               bState = STATE_LRC;
             }
             break;

             case STATE_LRC:
             if((BYTE)bTmp != bLrc)
             {
            LogEvent("PINNACLE: serial_read communication error, bad LRC", ERROR_MSG);
            bState = STATE_RESTART;
               break;
             }
              else
              {
               // Valid message!
               bState = STATE_STX;
               pfNotify(lpBuf, wLength, ipc_info);
               break;
             }

             case STATE_RESTART:
             // An error occurred - Flush the comm buffer
              PurgeComm(COMDEV( com ), PURGE_RXCLEAR);
             bState = STATE_STX;
             break;
          } // case
       }   // while getbyte()
    }     // if RX_CHAR
  }       // while CONNECTED
  LogEvent("PINNACLE: serial_read, exit loop, disconnected", INFO_MSG);
  logoff_xipc(DM_PORTTYPE_SERIAL); 
  _endthread(); 
}         // serial_read()


void CloseAllHandles()
{
  CloseHandle(READ_OS( com ).hEvent);
  CloseHandle(WRITE_OS( com ).hEvent);
  CloseHandle(COMDEV( com ));
}

void serial_close(int dummy)
{
  if (CONNECTED( com ) == TRUE)
  {
    // disable event notification and wait for thread
    // to halt
    SetCommMask( COMDEV( com ), 0 ) ;

    // drop DTR
    EscapeCommFunction( COMDEV( com ), CLRDTR ) ;

    // purge any outstanding reads/writes and close device handle

    PurgeComm( COMDEV( com ), PURGE_TXABORT | PURGE_RXABORT |
                              PURGE_TXCLEAR | PURGE_RXCLEAR ) ;

    CloseAllHandles();

    // set connected flag to FALSE
    CONNECTED( com ) = FALSE ;
  }
}

// Write data to the serial port.

BOOLEAN serial_write(const WORD len, const void * lpData)
{
   BYTE     bLrc;
   pBYTE lpByteData;
   BYTE     bHeader[3];     // Header bytes Length bytes BCD
   BYTE     bTrailer[2];    // trailer bytes
   int         i;


   // Compute the lrc and set up the Header and Trailer bytes 
   bLrc = 0;
   lpByteData = (pBYTE)lpData;
   bHeader[0] = STX;
   bin2bcd(&bHeader[1], len);
   bLrc ^= bHeader[1];
   bLrc ^= bHeader[2];
   for(i = 0; i < len; i++)
     bLrc ^= *(lpByteData + i);
   
   bLrc ^= bTrailer[0] = ETX;
   bTrailer[1] = bLrc;

   // Write out to the comm port. We'll catch any errors the next
   // time around.
   
  if(serial_put_bytes(bHeader, 3) == FALSE)
  return(FALSE);

   if(serial_put_bytes(lpByteData, (DWORD)len) == FALSE)
    return(FALSE);

   if(serial_put_bytes(bTrailer, 2) == FALSE)
  return(FALSE);

   return(TRUE);
      
}

BOOLEAN serial_init(const USER_SERIAL_STRUCT * ud, pIPC_INFO ipc_info)
{
   int     rc;
  DWORD   err;

   // Idiot check the user data 
   if(!ud)
   {
    LogEvent("PINNACLE: serial_init, bad user data pointer passed", ERROR_MSG);
    return(FALSE);
   }
   else if(!ud->lpBuf || !ud->pfNotify)
   {
    LogEvent("PINNACLE: serial_init, bad Function/Buffer pointer passed", ERROR_MSG);
      return(FALSE);
   }

  // Verify is already connected
  if (CONNECTED( com ) == TRUE)
  {
    LogEvent("PINNACLE: serial_init, connection already established", WARN_MSG); 
    return(FALSE);
  }
 
   // Set up the external function pointers
   port_write = serial_write;
   port_close = serial_close;
  
  // Reset Overlapped Structure
   
  memset(&(WRITE_OS( com )), 0, sizeof(OVERLAPPED));
  memset(&(READ_OS( com )), 0, sizeof(OVERLAPPED));

  // create I/O event used for overlapped reads/writes
  READ_OS( com ).hEvent = CreateEvent( NULL,    // no security
                                       TRUE,    // explicit reset req
                                       FALSE,   // initial event reset
                                       NULL ) ; // no name

  if (READ_OS( com ).hEvent == NULL)
  {
    err = GetLastError();
    LogEvent("PINNACLE: serial_ init read create event error", ERROR_MSG);
    return ( FALSE ) ;
  }
  
  WRITE_OS( com ).hEvent = CreateEvent( NULL,    // no security
                                        TRUE,    // explicit reset req
                                        FALSE,   // initial event reset
                                        NULL ) ; // no name

  if (NULL == WRITE_OS( com ).hEvent)
  {
    CloseHandle( READ_OS( com ).hEvent ) ;
    err = GetLastError();
    LogEvent("PINNACLE: serial_init write create event error", ERROR_MSG);
    return ( FALSE ) ;
  }

  // Open the COM port.
  // Changed OPEN ATTRIBUTES from OPEN_EXISTING to OPEN_ALWAYS
  if ((COMDEV( com ) =
      CreateFile( ud->cPort, GENERIC_READ | GENERIC_WRITE,
                  0,                    // exclusive access
                  NULL,                 // no security attrs
                  OPEN_EXISTING,        // if exists then open, else fail
                  FILE_ATTRIBUTE_NORMAL | 
                  FILE_FLAG_OVERLAPPED, // overlapped I/O
                  NULL )) == (HANDLE) -1 )
  {
      err = GetLastError();
    LogEvent("PINNACLE: serial_init, serial create file failed", ERROR_MSG);
      return(FALSE);
  }

  // get any early notifications
  SetCommMask( COMDEV( com ), EV_RXCHAR ) ;


   // Omit the call to SetupComm to use the default queue sizes.
  // setup device buffers
  // SetupComm( COMDEV( com ), 4096, 4096 ) ;

  // purge any information in the buffer
  PurgeComm( COMDEV( com ), PURGE_TXABORT | PURGE_RXABORT |
                            PURGE_TXCLEAR | PURGE_RXCLEAR ) ;
  
   // Get the current configuration.
   rc = GetCommState(COMDEV( com ), &(COMDCB( com )));

   if (!rc) 
   {
      err = GetLastError();
    LogEvent("PINNACLE: serial_init, get commstate failed", ERROR_MSG);
    return(FALSE);
  }

   // Fill in the DCB from user data.
   BAUDRATE( com ) = ud->dwBaudRate;
   BYTESIZE( com ) = ud->bByteSize;
   PARITY( com )   = ud->bParity;
   STOPBITS( com ) = ud->bStopBits;
  
   rc = SetCommState(COMDEV( com ), &(COMDCB( com )));

   if(!rc) 
   {
      err = GetLastError();
    LogEvent("PINNACLE: serial_init, set commstate failed", ERROR_MSG);
    CloseAllHandles();
      return(FALSE);
   }
  
  // Comm State Setup Successful 

  // Change connection status
  CONNECTED( com ) = TRUE ;

  // assert DTR
  EscapeCommFunction( COMDEV( com ), SETDTR );

   // Populate the callback function and data buffer variables
   pfNotify  = ud->pfNotify;
   lpBuf     = ud->lpBuf;
   wBufLen   = ud->wBufferLen;

   _beginthread(serial_read, 0, ipc_info);

   return(TRUE);
}

