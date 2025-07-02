/**
 * @file      : Bankin_Operations.c
 * @authors   : Mohammed-Elahmady, Hatem Ayman, Mayar Basha, A'laa Abd-Elhady
 * @brief     : Definitions of our project Functions
 * @date      : Sat 28 Jun 2025
 */

/******************* Section 0 : Includes *******************/

#include "Banking_Operations.h"

/******************* Section 1 :  Variables Definitions *******************/

/******************* Section 2 :  Helper Functions Declarations *******************/

static Error_States Data_Base_Overwrite_data(System *Bank, FILE *data_base);
static Error_States Bank_Shift_Customers(System *Bank, uint32 del_Customer);
static sint32 find_customer_id(System *Bank, uint32 target_id);
static bool valid_phone_number(uint32 num);
static bool Check_Repeated_Customer_ID(System *Bank, uint32 id);
static bool Check_Repeated_Customer_Phone_Number(System *Bank, uint32 phone);
static bool Is_Phone_Unique_Except(System *Bank, uint32 phone, uint32 except_id);

/******************* Section 3 : Software Interfaces Definitions (APIs) *******************/

/**
 * @brief            : Initialize the banking system structure
 * @param state      : Pointer to a variable of type Error_States to store the status of the initialization
 * @return           : Pointer to a dynamically allocated System structure
 *      - (E_OK)     : The system was initialized successfully
 *      - (E_NOT_OK) : The initialization failed due to memory allocation issues or invalid state pointer
 */

System *System_init(Error_States *state)
{
    System *Bank;
    *state = OP_Success;
    if (NULL == state)
    {
        *state = OP_Failed;
    }
    else
    {
        Bank = (System *)malloc(sizeof(System));
        if (NULL == Bank)
        {
            *state = OP_Failed;
        }
        else
        {
            (Bank->Customer_pointer) = System_Empty;
        }
    }
    return Bank;
}

/**
 * @brief            : Create a new customer in the system and append their data to the database file
 * @param Bank       : Pointer to the System structure containing customer records
 * @param data_base  : File pointer to the database file where new customer data will be written
 * @param name       : Pointer to the new customer's name
 * @param id         : Unique ID to assign to the new customer
 * @param phone      : Phone number of the new customer
 * @param cash       : Initial cash amount for the new customer
 * @return           : Status of the function
 *      - (OP_Success) : Customer successfully created and data saved in file
 *      - (OP_Failed)  : Creation failed due to null inputs, full customer list, duplicate ID or phone, or invalid phone number
 */

Error_States Create_Customer(System *Bank, FILE *data_base, const sint8 *name, uint32 id, uint32 phone, uint32 cash)
{
    Error_States state = OP_Success;
    if (NULL == Bank || NULL == data_base || NULL == name)
    {
        state = OP_Failed;
    }
    else
    {
        if (Bank->Customer_pointer >= Max_Customers)
        {
            state = OP_Failed;
        }
        else
        {
            strncpy(Bank->Customers[Bank->Customer_pointer].name, name, Name_Size - 1);
            Bank->Customers[Bank->Customer_pointer].name[Name_Size - 1] = '\0';
            Bank->Customers[Bank->Customer_pointer].ID = id;
            Bank->Customers[Bank->Customer_pointer].Phone_Number = phone;
            Bank->Customers[Bank->Customer_pointer].Cash_Amount = cash;

            if (!Check_Repeated_Customer_ID(Bank, id))
            {
                state = OP_Failed;
            }
            else
            {
                if (!valid_phone_number(phone) || !Check_Repeated_Customer_Phone_Number(Bank, phone))
                {
                    state = OP_Failed;
                }
                else
                {
                    fprintf(data_base, "Customer [%lu]\n", (Bank->Customer_pointer) + 1);
                    fprintf(data_base, "Customer's Name : %s\n", Bank->Customers[Bank->Customer_pointer].name);
                    fprintf(data_base, "Customer's ID : %06lu\n", Bank->Customers[Bank->Customer_pointer].ID);
                    fprintf(data_base, "Customer's Phone Number : 0%010lu\n", Bank->Customers[Bank->Customer_pointer].Phone_Number);
                    fprintf(data_base, "Customer's Cash Amount : %lu\n", Bank->Customers[Bank->Customer_pointer].Cash_Amount);
                    fprintf(data_base, "------------------------------------------------------------------------\n");
                    
                    Bank->Customer_pointer++;
                    fflush(data_base);
                }
            }
        }
    }
    return (state);
}

/**
 * @brief            : Load all customer records from the database file into memory
 * @param Bank       : Pointer to the System structure to store loaded customer data
 * @param data_base  : File pointer to the database file for reading customer information
 * @return           : Status of the function
 *      - (E_OK)     : Customers loaded successfully
 *      - (E_NOT_OK) : Failed due to null pointers or file read error
 */

Error_States Load_Customers_From_File(System *Bank, FILE *data_base)
{
    Error_States state = OP_Success;
    if (NULL == Bank || NULL == data_base)
    {
        state = OP_Failed;
    }
    else
    {
        rewind(data_base);
        sint8 line[1024];
        uint32 index = 0;

        while (fgets(line, sizeof(line), data_base) != NULL)
        {
            if (strncmp(line, "Customer [", 10) == 0)
            {
                // Read name
                if (!fgets(line, sizeof(line), data_base))
                    break;
                sscanf(line, "Customer's Name : %[^\n]", Bank->Customers[index].name);

                // Read ID
                if (!fgets(line, sizeof(line), data_base))
                    break;
                sscanf(line, "Customer's ID : %lu", &Bank->Customers[index].ID);

                // Read phone
                if (!fgets(line, sizeof(line), data_base))
                    break;
                sscanf(line, "Customer's Phone Number : %lu", &Bank->Customers[index].Phone_Number);

                // Read cash
                if (!fgets(line, sizeof(line), data_base))
                    break;
                sscanf(line, "Customer's Cash Amount : %lu", &Bank->Customers[index].Cash_Amount);

                // Skip separator line
                fgets(line, sizeof(line), data_base);
                index++;
            }
        }
        Bank->Customer_pointer = index;
    }
    return (state);
}

/**
 * @brief            : Edit a customer's name and phone number in the system and update the database file
 * @param Bank       : Pointer to the System structure containing customer records
 * @param data_base  : File pointer to the database file to update customer records
 * @param id         : Unique ID of the customer to be edited
 * @param new_name   : Pointer to the new name string for the customer
 * @param new_phone  : New phone number to be assigned to the customer
 * @return           : Status of the function
 *      - (OP_Success) : Customer successfully edited and file updated
 *      - (OP_Failed)  : Editing failed due to invalid input, non-existing ID, or phone validation failure
 */

Error_States Edit_Customer(System *Bank, FILE *data_base, uint32 id, const sint8 *new_name, uint32 new_phone)
{
    Error_States state = OP_Success;
    if (NULL == Bank || NULL == data_base)
    {
        state = OP_Failed;
    }
    else
    {
        sint32 index = find_customer_id(Bank, id);
        if (index == -1)
        {
            state = OP_Failed;
        }
        else
        {
            if (!valid_phone_number(new_phone) || !Is_Phone_Unique_Except(Bank, new_phone, id))
            {
                state = OP_Failed;
            }
            else
            {
                strncpy(Bank->Customers[index].name, new_name, Name_Size - 1);
                Bank->Customers[index].name[Name_Size - 1] = '\0';
                Bank->Customers[index].Phone_Number = new_phone;

                state = Data_Base_Overwrite_data(Bank, data_base);
            }
        }
    }
    return state;
}

/**
 * @brief            : Retrieve and format customer information into a string buffer
 * @param Bank       : Pointer to the System structure containing customer records
 * @param id         : Unique ID of the customer whose information is to be displayed
 * @param output     : Pointer to a character buffer where the formatted information will be stored
 * @return           : Status of the function
 *      - (OP_Success) : Customer information successfully retrieved and stored in output
 *      - (OP_Failed)  : Retrieval failed due to invalid input or non-existing ID
 */

Error_States Display_Customer_information(System *Bank, uint32 id, sint8 *output)
{
    Error_States state = OP_Success;
    if (NULL == Bank || NULL == output)
    {
        state = OP_Failed;
    }
    else
    {
        sint32 index = find_customer_id(Bank, id);
        if (index == -1)
        {
            state = OP_Failed;
        }
        else
        {
            snprintf(output, 1024,
                     "Customer's Name : %s\n"
                     "Customer's ID : %06lu\n"
                     "Customer's Phone Number : 0%010lu\n"
                     "Customer's Cash Amount : %lu",
                     Bank->Customers[index].name,
                     Bank->Customers[index].ID,
                     Bank->Customers[index].Phone_Number,
                     Bank->Customers[index].Cash_Amount);
        }
    }
    return (state);
}

/**
 * @brief            : Delete a customer from the system and remove their data from the database file
 * @param Bank       : Pointer to the System structure containing customer records
 * @param data_base  : File pointer to the database file to update customer records
 * @param id         : Unique ID of the customer to be deleted
 * @return           : Status of the function
 *      - (E_OK)     : Customer successfully deleted and file updated
 *      - (E_NOT_OK) : Deletion failed due to invalid ID, shifting error, or file access issues
 */

Error_States Delete_Customer(System *Bank, FILE *data_base, uint32 id)
{
    Error_States state = OP_Success;
    if (NULL == Bank || NULL == data_base)
    {
        state = OP_Failed;
    }
    else
    {
        sint32 index = find_customer_id(Bank, id);
        if (index == -1)
        {
            state = OP_Failed;
        }
        else
        {
            Error_States state = Bank_Shift_Customers(Bank, index);
            if (state != OP_Success)
            {
                state = state;
            }
            else
            {
                Bank->Customer_pointer--;
                state = Data_Base_Overwrite_data(Bank, data_base);
            }
        }
    }
    return (state);
}

/**
 * @brief            : Transfer a specific amount of money from one customer to another
 * @param Bank       : Pointer to the System structure containing customer data
 * @param data_base  : File pointer to the database file to update balances
 * @param C_id       : Unique ID of the customer sending the money
 * @param D_id       : Unique ID of the customer receiving the money
 * @param T_money    : Amount of money to be transferred
 * @return           : Status of the function
 *      - (E_OK)     : Transfer completed and changes saved to the database
 *      - (E_NOT_OK) : Transfer failed due to insufficient funds, invalid IDs, or file issues
 */

Error_States Customer_Transfer_Money(System *Bank, FILE *data_base, uint32 C_id, uint32 D_id, uint32 T_money)
{
    Error_States state = OP_Success;
    if (NULL == Bank || NULL == data_base || T_money == 0)
    {
        state = OP_Failed;
    }
    else
    {
        sint32 sender_index = find_customer_id(Bank, C_id);
        sint32 receiver_index = find_customer_id(Bank, D_id);

        if (sender_index == -1 || receiver_index == -1)
        {
            state = OP_Failed;
        }
        else
        {
            if (Bank->Customers[sender_index].Cash_Amount < T_money)
            {
                state = OP_Failed;
            }
            else
            {
                Bank->Customers[sender_index].Cash_Amount -= T_money;
                Bank->Customers[receiver_index].Cash_Amount += T_money;
                state = Data_Base_Overwrite_data(Bank, data_base);
            }
        }
    }
    return (state);
}

/**
 * @brief            : Deposit a specified amount of money into a customer's account
 * @param Bank       : Pointer to the System structure containing customer records
 * @param data_base  : File pointer to the database file to update customer balance
 * @param id         : Unique ID of the customer receiving the deposit
 * @param money      : Amount of money to deposit
 * @return           : Status of the function
 *      - (E_OK)     : Deposit successful and data updated
 *      - (E_NOT_OK) : Deposit failed due to invalid ID, zero amount, or file/pointer errors
 */

Error_States Customer_Deposit_Money(System *Bank, FILE *data_base, uint32 id, uint32 money)
{
    Error_States state = OP_Success;
    if (NULL == Bank || NULL == data_base || money == 0)
    {
        state = OP_Failed;
    }
    else
    {
        sint32 index = find_customer_id(Bank, id);
        if (index == -1)
        {
            state = OP_Failed;
        }
        else
        {
            Bank->Customers[index].Cash_Amount += money;
            state = Data_Base_Overwrite_data(Bank, data_base);
        }
    }
    return (state);
}

/**
 * @brief            : Withdraw a specified amount of money from a customer's account
 * @param Bank       : Pointer to the System structure containing customer records
 * @param data_base  : File pointer to the database file to update customer balance
 * @param id         : Unique ID of the customer making the withdrawal
 * @param money      : Amount of money to withdraw
 * @return           : Status of the function
 *      - (E_OK)     : Withdrawal successful and data updated
 *      - (E_NOT_OK) : Withdrawal failed due to insufficient funds, invalid ID, or file/pointer issues
 */

Error_States Customer_Withdraw_Money(System *Bank, FILE *data_base, uint32 id, uint32 money)
{
    Error_States state = OP_Success;
    if (NULL == Bank || NULL == data_base || money == 0)
    {
        state = OP_Failed;
    }
    else
    {
        sint32 index = find_customer_id(Bank, id);
        if (index == -1)
        {
            state = OP_Failed;
        }
        else
        {
            if (Bank->Customers[index].Cash_Amount < money)
            {
                state = OP_Failed;
            }
            else
            {
                Bank->Customers[index].Cash_Amount -= money;
                state = Data_Base_Overwrite_data(Bank, data_base);
            }
        }
    }
    return (state);
}

/******************* Section 2 :  Helper Functions Definitions *******************/

/**
 * @brief            : Overwrite the entire database file with current customer data from memory
 * @param Bank       : Pointer to the System structure containing customer records
 * @param data_base  : File pointer to the database file to overwrite
 * @return           : Status of the function
 *      - (E_OK)     : File successfully overwritten
 *      - (E_NOT_OK) : Operation failed due to null pointers
 */

static Error_States Data_Base_Overwrite_data(System *Bank, FILE *data_base)
{
    Error_States state = OP_Success;
    if (NULL == Bank || NULL == data_base)
    {
        state = OP_Failed;
    }
    else
    {        
        freopen(FILE_PATH, "w", data_base);
        for (uint32 i = 0; i < Bank->Customer_pointer; i++)
        {
            fprintf(data_base, "Customer [%lu]\n", i + 1);
            fprintf(data_base, "Customer's Name : %s\n", Bank->Customers[i].name);
            fprintf(data_base, "Customer's ID : %06lu\n", Bank->Customers[i].ID);
            fprintf(data_base, "Customer's Phone Number : 0%010lu\n", Bank->Customers[i].Phone_Number);
            fprintf(data_base, "Customer's Cash Amount : %lu\n", Bank->Customers[i].Cash_Amount);
            fprintf(data_base, "------------------------------------------------------------------------\n");
            fflush(data_base);
        }
    }
    return (state);
}

/**
 * @brief            : Shift customer entries to remove a deleted customer from memory
 * @param Bank           : Pointer to the System structure
 * @param del_Customer   : Index of the customer to delete
 * @return           : Status of the function
 *      - (E_OK)     : Shifting completed successfully
 *      - (E_NOT_OK) : Operation failed due to null pointer
 */

static Error_States Bank_Shift_Customers(System *Bank, uint32 del_Customer)
{
    Error_States state = OP_Success;
    if (NULL == Bank)
    {
        state = OP_Failed;
    }
    else
    {        
        for (uint32 i = del_Customer; i < (Bank->Customer_pointer) - 1; i++)
        {
            Bank->Customers[i] = Bank->Customers[i + 1];
        }
    }
    return (state);
}

/**
 * @brief            : Find the index of a customer in the system using their unique ID
 * @param Bank       : Pointer to the System structure
 * @param target_id  : Customer ID to search for
 * @return           : Index of the customer if found, -1 otherwise
 */

static sint32 find_customer_id(System *Bank, uint32 target_id)
{
    sint32 index = -1;
    for (uint32 i = 0; i < Bank->Customer_pointer; i++)
    {
        if (target_id == Bank->Customers[i].ID)
        {
            index = i;
            break;
        }
    }
    return (index);
}

/**
 * @brief            : Validate the phone number format based on Egyptian prefixes
 * @param num        : Phone number to validate
 * @return           : true if valid, false if invalid
 */

static bool valid_phone_number(uint32 num)
{
    bool flag = true;
    for (uint32 i = 0; i < 8; i++)
    {
        num /= 10;
    }
    if (num == 10 || num == 11 || num == 12 || num == 15)
    {
        flag = true;
    }
    else
    {
        flag = false;
    }
    return (flag);
}

/**
 * @brief            : Check if a given customer ID already exists in the system
 * @param Bank       : Pointer to the System structure
 * @param id         : Customer ID to check
 * @return           : true if the ID is unique, false if it already exists
 */

static bool Check_Repeated_Customer_ID(System *Bank, uint32 id)
{
    bool flag = true;
    if (NULL == Bank)
    {
        flag = false;
    }
    for (uint32 i = 0; i < Bank->Customer_pointer; i++)
    {
        if (id == Bank->Customers[i].ID)
        {
            flag = false;
            break;
        }
    }
    return (flag);
}

/**
 * @brief            : Check if a given phone number is already registered to a customer
 * @param Bank       : Pointer to the System structure
 * @param phone      : Phone number to check
 * @return           : true if the number is unique, false if it already exists
 */

static bool Check_Repeated_Customer_Phone_Number(System *Bank, uint32 phone)
{
    bool flag = true;
    if (NULL == Bank)
    {
        flag = false;
    }
    for (uint32 i = 0; i < Bank->Customer_pointer; i++)
    {
        if (phone == Bank->Customers[i].Phone_Number)
        {
            flag = false;
        }
    }
    return (flag);
}

/**
 * @brief            : Check if a phone number is unique among all customers except a specific customer
 * @param Bank       : Pointer to the System structure containing customer records
 * @param phone      : Phone number to check for uniqueness
 * @param except_id  : ID of the customer to exclude from the uniqueness check
 * @return           : true if phone number is unique among other customers, false otherwise
 */

static bool Is_Phone_Unique_Except(System *Bank, uint32 phone, uint32 except_id)
{
    bool flag = true;
    if (NULL == Bank)
    {
        flag = false;
    }
    for (uint32 i = 0; i < Bank->Customer_pointer; i++)
    {
        if (Bank->Customers[i].ID == except_id)
            continue;
        if (phone == Bank->Customers[i].Phone_Number)
        {
            flag = false;
            break;
        }
    }
    return (flag);
}
