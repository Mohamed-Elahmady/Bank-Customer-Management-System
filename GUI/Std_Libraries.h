/**
 * @file      : Std_libraries.h
 * @authors   : Mohammed-Elahmady, Hatem Ayman, Mayar Basha, A'laa Abd-Elhady
 * @brief     : calling Libraries to use in Project
 * @date      : Sat 28 Jun 2025
 */

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
#define delay(x) Sleep(x)
#else
#include <unistd.h>
#define delay(x) sleep(x / 1000)
#endif

/******************* Section 2 : Macros Declarations *******************/

#define delay_time 3000
#define FILE_PATH "/Users/mohammedel-ahmady/Data/IEEE-RAS-Embeddded-Project1/GUI/data_base.txt"

/******************* Section 3 : Macros Functions Declarations *******************/

/******************* Section 4 : User Defined Data Types & Variables Declarations *******************/

/******************* Section 5 : Software Interfaces Declarations (APIs) *******************/

#endif /* _STD_LIBRARIES_H */
