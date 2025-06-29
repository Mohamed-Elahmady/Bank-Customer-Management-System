/******************* Section 0 : File Guard *******************/

#ifndef _STD_LIBRARIES_H
#define _STD_LIBRARIES_H

/******************* Section 1 : Includes *******************/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include "Platform.h"
#ifdef _WIN32
#include <windows.h>
#define delay(x)  Sleep(x)
#else 
#include <unistd.h>
#define delay(x)  sleep(x/1000)
#endif


#define delay_time      3000
#define FILE_PATH       "/Users/mohammedel-ahmady/Data/Bank-Customer-Management-System/data_base.txt"

/******************* Section 2 : Macros Declarations *******************/



/******************* Section 3 : Macros Functions Declarations *******************/



/******************* Section 4 : User Defined Data Types & Variables Declarations *******************/



/******************* Section 5 : Software Interfaces Declarations (APIs) *******************/


#endif /* _STD_LIBRARIES_H */