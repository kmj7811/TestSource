/*===================================================

                  LOG CTRL HEADER


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

#ifndef __LOG_MAIN_H
#define __LOG_MAIN_H



#define TCU_LOG_BUFFER_SIZE 512
#define TCU_LOG_FILE_SIZE 500*1024 //500Kbyte
#define TCU_LOG_FILE_CNT 500


void logSysLog(int log_select,const char* fn,  int ln, const char *format, ...);


#endif //__LOG_MAIN_H

