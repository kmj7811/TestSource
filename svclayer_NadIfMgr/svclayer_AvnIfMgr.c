/*===================================================

                  AVN Interface Mgr


GENERAL DESCRIPTION




Copyright (c) 2018 by Digen, Incorporated.  All Rights Reserved.
Digen Confidential and Proprietary.
=====================================================*/

/*====================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ----------------------------------------------------------


*/

/*=====================================================

                     INCLUDE FILES FOR MODULE

=====================================================*/
#include "ql_oe.h"

#include <termios.h>
#include <time.h>

 #include "svclayer_AvnIfMgr.h"
 #include "svclayer_ProcAvnPkt.h"
 #include "tcu_interface.h"
 
#define _GNU_SOURCE

pthread_mutex_t mutexForSerial;

//char* default_port = "dev/ttyGS1";  //--> ttyUSB4
//char* default_port = "/dev/ttyHSL0";  //--> debug port
char* default_port = "/dev/ttyGS0";  // --> NMEA port

static int fd_uart = -1;
int mExitThread=0;


void LOCK_SERIAL()
{
    LOGI("LOCK SERIAL");
    TCU_PLATFORM_MUTEX_LOCK(&mutexForSerial);	
}
void UNLOCK_SERIAL()
{
    LOGI("UNLOCK SERIAL"); 
    TCU_PLATFORM_MUTEX_UNLOCK(&mutexForSerial);	
}


static int serial_resolve_baud(int baud);

/**
 * Recieve data.
 * Retrieves data from the serial device.
 * @param s - serial structure.
 * @param data - pointer to a buffer to read data into.
 * @param maxLength - size of input buffer.
 * @return amount of data recieved.
 */
static int serial_recieve(serial_t* obj, uint8_t data[], int maxLength);

/**
 * @brief Serial Listener Thread.
 * This blocks waiting for data to be recieved from the serial device,
 * and calls the serial_callback method with appropriate context when
 * data is recieved.
 * Exits when close method is called, or serial error occurs.
 * @param param - context passed from thread instantiation.
 */
static void *serial_data_listener(void *param);

/**
 * @brief Start the serial threads.
 * This spawns the listening and transmitting threads
 * if they are not already running.
 * @param s - serial structure.
 * @return 0 on success, or -1 on error.
 */
static int serial_start(serial_t* s);

/**
 * Stop serial listener thread.
 * @param s - serial structure.
 * @return 0;
 */
static int serial_stop(serial_t* s);

/**
 * Callback to handle recieved data.
 * Puts recieved data into the rx buffer.
 * @param s - serial structure.
 * @param data - data to be stored.
 * @param length - length of recieved data.
 */
static void serial_rx_callback(serial_t* s, char data[], int length);

// Put character in rx buffer.
static int buffer_put(serial_t* s, char c)
{
    //if there is space in the buffer
    if ( s->end != ((s->start + BUFF_SIZE - 1) % BUFF_SIZE)) {
        s->rxbuff[s->end] = c;
        s->end ++;
        s->end = s->end % BUFF_SIZE;
        //printf("Put: %x start: %d, end: %d\r\n", c, s->start, s->end);
        return 0;        //No error
    } else {
        //buffer is full, this is a bad state
        return -1;        //Report error
    }
}

// Get character from rx buffer.
static char buffer_get(serial_t* s)
{
    char c = (char)0;
    //if there is data to process
    if (s->end != s->start) {
        c = (s->rxbuff[s->start]);
        s->start ++;
        //wrap around
        s->start = s->start % BUFF_SIZE;
    } else {
    }
    //printf("Get: %x start: %d, end: %d\r\n", c, s->start, s->end);
    return c;
}

//Get data available in the rx buffer.
static int buffer_available(serial_t* s)
{
    return (s->end - s->start + BUFF_SIZE) % BUFF_SIZE;
}

// ---------------        External Functions        ---------------

//Create serial object.
serial_t* serial_create()
{
    //Allocate serial object.
    serial_t* s = malloc(sizeof(serial_t));
    //Reconfigure buffer object.
    s->start = 0;
    s->end = 0;
    //Return pointer.
    return s;
}


void serial_destroy(serial_t* s)
{
    free(s);
}

//Connect to serial device.
int serial_connect(serial_t* s, char device[], int baud)
{
    struct termios oldtio;

    LOGI("serial_connect...");
    // Resolve baud.
    int speed = serial_resolve_baud(baud);
    if (speed < 0) {
        printf("Error: Baud rate not recognized.\r\n");
        return -1;
    }

    //Open device.
    s->fd = open(device, O_RDWR | O_NOCTTY );
    //Catch file open error.
    if (s->fd < 0) {
        perror(device);
        return -2;
    }
    //Retrieve settings.
    tcgetattr(s->fd, &oldtio);
    //Set baud rate.
    cfsetspeed(&oldtio, speed);
    //Flush cache.
    tcflush(s->fd, TCIFLUSH);
    //Apply settings.
    tcsetattr(s->fd, TCSANOW, &oldtio);

    //Start listener thread.
    int res = serial_start(s);
    //Catch error.
    if (res < 0) {
        printf("Error: serial thread could not be spawned\r\n");
        return -3;
    }

    //Indicate connection was successful.
    s->state = 1;
    return 0;
}

//Send data.
int serial_send(serial_t* s, uint8_t data[], int length)
{
    //LOCK_SERIAL();
    int res = write(s->fd, data, length);
    //UNLOCK_SERIAL();
	
    return res;
}
int send_to_serial(avn_data_t* data, int length)
{
    //LOCK_SERIAL();
    int res = write(serial_fd->fd, (uint8_t*)data, length);
    //UNLOCK_SERIAL();
    return res;	
}

void serial_put(serial_t* s, uint8_t data)
{
    char arr[1];
    arr[0] = data;
    write(s->fd, arr, 1);
}

//Determine characters available.
int serial_available(serial_t* s)
{
    return buffer_available(s);
}

//Fetch a character.
char serial_get(serial_t* s)
{
    char c = buffer_get(s);

    return c;
}

char serial_blocking_get(serial_t* s)
{
    while (serial_available(s) == 0);
    return serial_get(s);
}

void serial_clear(serial_t* s)
{
    //Clear the buffer.
    while (buffer_available(s)) {
        buffer_get(s);
    }
}

//Close serial port.
int serial_close(serial_t* s)
{
    //Stop thread.
    serial_stop(s);
    return 0;
}

// ---------------        Internal Functions        --------------

//Stop serial listener thread.
static int serial_stop(serial_t* s)
{
    s->running = 0;
    return 0;
}

// Resolves standard baud rates to linux constants.
static int serial_resolve_baud(int baud)
{
    int speed;
    // Switch common baud rates to temios constants.
    switch (baud) {
    case 9600:
        speed = B9600;
        break;
    case 19200:
        speed = B19200;
        break;
    case 38400:
        speed = B38400;
        break;
    case 57600:
        speed = B57600;
        break;
    case 115200:
        speed = B115200;
        break;
    default:
        speed = -1;
        break;
    }
    // Return.
    return speed;
}


// Start serial listener.
static int serial_start(serial_t* s)
{
    //int result = 0;
    int status;	
    //pthread_t serialThread;	
	
    //Only start if it is not currently running.
    LOGI("serial_start...");
    
    if (s->running != 1) {
        //Set running.
        s->running = 1;
        //Spawn thread.
        int res;
        LOGI("Create Listener thread...");		
        res = pthread_create(&s->rx_thread, NULL, serial_data_listener, (void*) s);
        //res = pthread_create(&serialThread, NULL, serial_data_listener, (void*) s);
        LOGI("thread creation result:%d",res);		
        if (res != 0) {
            return -2;
        }

        pthread_join(s->rx_thread, (void **)&status);		
        //pthread_join(serialThread, (void **)&status);
        LOGI("Thread End %d", status);	
		
        sleep(5);		

    } else {
        return -1;
    }
}



//Recieve data.
static int serial_recieve(serial_t* s, uint8_t data[], int maxLength)
{
    return read(s->fd, data, maxLength);
}

//Callback to store data in buffer.
static void serial_rx_callback(serial_t* s, char data[], int length)
{
    //Put data into buffer.
    int i;
    //Put data into buffer.
    for (i = 0; i < length; i++) {
        buffer_put(s, data[i]);
    }
}

#if 0
long getMicrotime(){
	struct timeval currentTime;
	gettimeofday(&currentTime, NULL);
	return currentTime.tv_sec * (int)1e6 + currentTime.tv_usec;
}
#endif

//Serial data listener thread.
static void *serial_data_listener(void *param)
{
    int res = 0;
    int err = 0;
    fd_set fdset;
    struct timeval timeout = {3, 0};
    uint8_t buff[BUFF_SIZE];


    //Retrieve paramaters and store locally.
    serial_t* serial = (serial_t*) param;
    int fd = serial->fd;

    //Run until ended.
    LOGI("Start Serial Listner...");	
    
    while (serial->running != 0)
    {
		FD_ZERO(&fdset);
		FD_SET(serial->fd, &fdset);
		res = select(serial->fd + 1, &fdset, NULL, NULL, &timeout);
		//If data was recieved.
		if (-1 == res ) 
		{
			LOGI("< Failed to select >");
			exit(-1);
		}
		else if(0 == res)
		{
			//LOGI("<no data>");
			timeout.tv_sec = 3;
			timeout.tv_usec = 0;
		}
		else
		{
			if(FD_ISSET(serial->fd, &fdset))
			{
				//Fetch the data.
				
				int count = serial_recieve(serial, buff, BUFF_SIZE - 1);

				//printf("System Date is: %02d/%02d/%04d\n",tm.tm_mday, tm.tm_mon+1, tm.tm_year+1900);
				//printf("System Time is: %02d:%02d:%02d\n",tm.tm_hour, tm.tm_min, tm.tm_sec);	
				//printf("System Time ms: %ld\n",getMicrotime());	


				
				//If data was recieved.
				if (count > 0) 
				{
					//Pad end of buffer to ensure there is a termination symbol.
					buff[count] = '\0';

			  #if 0		
					// Call the serial callback.
					serial_rx_callback(serial, (char *)buff, count);
					#endif
					
					/////////////////////////////////////////////
					#if 1
						int j = 0;
						LOGI("Rcv leng = [%d]  ",count);
						buff[count] = '\0';					
						LOGI("Rcv String = %s  ",buff);
											
						for(j = 0; j<count; j++)
						{
						  LOGI("data[%d] = %c[0x%x]", j, (uint8_t)buff[j],(uint8_t)buff[j]);
						}                	
						
						if(((uint8_t)buff[0] == SOFRAME_AVN) && ((uint8_t)buff[count-1] == EOFRAME_AVN))
						{
							LOGI("This is AVN packet");
							process_avn_pkt((avn_data_t*) buff);	 //future todo						
						}
					
						
					#endif                /////////////////////////////////////////////
					
					//If an error occured.
				} 
				else if (count < 0) 
				{
					//Inform user and exit thread.
					LOGI("Error: Serial disconnect");
					err = 1;
					break;
				}
			}
		}
	}
 

    //If there was an error, close socket.
    if (err) {
        serial_close(serial);
        //raise(SIGLOST);
    }
    //Close file.
    res = close(serial->fd);

    return NULL;
}





void *avnIfMgrThreadFn(void *fn)
{
    int msqid;
   // int ret ;
    boolean bReturn;	
    avn_cmdq_t  rcv_data;

    
    
    LOGI("--- avnIfMgrThreadFn Start\n");
    
    msqid = msgget( (key_t)FRAME2AVNIF_MSGQ_KEY, IPC_CREAT | 0666);
	
    if(msqid<0)
    {
      LOGI("avnIfMgrThreadFn msgget error\n");
      return;
    }
    
    while(mExitThread!=1) 
    {
      memset(&rcv_data,0,sizeof(avn_cmdq_t));
      if (msgrcv( msqid, (void *)&rcv_data, sizeof(avn_cmdq_t)- sizeof(long), FRAME_TO_AVNIFMGR_TYPE, 0) == -1)
      {
        LOGI("SENDDAT_KEY msgrcv fail\n");
        usleep(10000);
        continue;
      }

      bReturn = process_tcuAvnIfMgr_pkt(&rcv_data);

    }
    
    msgctl(msqid, IPC_RMID, 0);
    LOGI("---avnIfMgrThreadFn Error -- Exit\n");
    return;
	
}



void sigquitavnif(int sig)
{
    LOGI("CTRL+C init and exit program");
    //serial_close(serial_fd);
    close(serial_fd->fd);
    //serial_destroy(serial_fd);	
}


int startAvnIfMgr(void)
{
    EXCEPTION_INIT
    int result = -1;		
    pthread_t avnIfMgrThread;
    pthread_attr_t attr;

    LOGI("...............................................");
    LOGI("AVN Interface Mgr Start Version %s", AVNMGRIF_VERSION);
    LOGI("...............................................");
 

    signal(SIGINT, sigquitavnif);

    TCU_PLATFORM_MUTEX_INIT(&mutexForSerial);	

    //Do any varialbe Initialization process

    serial_fd = serial_create();


//=============================================================
    serial_clear(serial_fd);	
    memset(&attr,0,sizeof(pthread_attr_t));
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);  
    if( pthread_create(&avnIfMgrThread,&attr, avnIfMgrThreadFn,NULL) < 0)
    {
      LOGI("avnIfMgrThreadFn Thread create error\n");
      THROW();
    }
    pthread_attr_destroy(&attr);
//================================================================

#if 1
    LOGI("Connect port %s",default_port);
    if(serial_connect(serial_fd,default_port,DEFAULT_BAUD_RATE) < 0)
    {
        LOGI("Fail to Connect Serial port %s",default_port);
        serial_destroy(serial_fd);
        THROW();	  
    }
    else
    {
        LOGI("Success to Connect Serial port %s",default_port);
    }
 #endif
 
    CATCH_ERR()
    {
      result = 1;
    }


    LOGI("...................Serial END....................\n");
  
  return result;


}


