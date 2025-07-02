/**
 * @file      : Bankin_Operations.h
 * @authors   : Mohammed-Elahmady, Hatem Ayman, Mayar Basha, A'laa Abd-Elhady
 * @brief     : Declarations of our project Functions
 * @date      : Sat 28 Jun 2025
 */

/******************* Section 0 : File Guard *******************/

#ifndef BANKING_OPERATIONS_H
#define BANKING_OPERATIONS_H

/******************* Section 1 : Includes *******************/

#include "Std_Libraries.h"

/******************* Section 2 : Macros Declarations *******************/

#define System_Empty    0
#define Name_Size       1000
#define Max_Customers   1000000

/******************* Section 3 : Macros Functions Declarations *******************/

/******************* Section 4 : User Defined Data Types & Variables Declarations *******************/

typedef enum {
    OP_Failed  = 0,
    OP_Success
} Error_States;

typedef struct {
    sint8 name[Name_Size];
    uint32 ID;
    uint32 Phone_Number;
    uint32 Cash_Amount;
} Customer;

typedef struct {
    Customer Customers[Max_Customers];
    uint32 Customer_pointer;
} System;

/******************* Section 5 : Software Interfaces Declarations (APIs) *******************/

System *System_init(Error_States *state);
Error_States Create_Customer(System *Bank, FILE *data_base, const sint8 *name, uint32 id, uint32 phone, uint32 cash);
Error_States Load_Customers_From_File(System *Bank, FILE *data_base);
Error_States Edit_Customer(System *Bank, FILE *data_base, uint32 id, const sint8 *new_name, uint32 new_phone);
Error_States Display_Customer_information(System *Bank, uint32 id, sint8 *output);
Error_States Delete_Customer(System *Bank, FILE *data_base, uint32 id);
Error_States Customer_Transfer_Money(System *Bank, FILE *data_base, uint32 C_id, uint32 D_id, uint32 T_money);
Error_States Customer_Deposit_Money(System *Bank, FILE *data_base, uint32 id, uint32 money);
Error_States Customer_Withdraw_Money(System *Bank, FILE *data_base, uint32 id, uint32 money);

#endif /* BANKING_OPERATIONS_H */