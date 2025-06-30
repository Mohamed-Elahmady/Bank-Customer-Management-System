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
    System *Bank = NULL;
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
    return (Bank);
}

/**
 * @brief            : Create a new customer and add their data to the system and file
 * @param Bank       : Pointer to the System structure that holds customer data
 * @param data_base  : File pointer to the database file for saving customer information
 * @return           : Status of the function
 *      - (E_OK)     : Customer created and saved successfully
 *      - (E_NOT_OK) : Failed to create customer due to invalid input, duplicate ID/phone, or null pointers
 */

Error_States Create_Customer(System *Bank, FILE *data_base)
{
    Error_States state = OP_Success;

    if (NULL == Bank || NULL == data_base)
    {
        state = OP_Failed;
    }
    else
    {
        print_line();
        print_centered("Creating a New Customer Account");
        print_line();

        // 1. Name
        print_centered("Please enter the customer's full name:");
        printf("|%*s", (MENU_WIDTH - 1) / 2, "");
        getchar();
        fgets(Bank->Customers[Bank->Customer_pointer].name,
              sizeof(Bank->Customers[Bank->Customer_pointer].name), stdin);

        uint32 len = strlen(Bank->Customers[Bank->Customer_pointer].name);
        if (len > 0 && Bank->Customers[Bank->Customer_pointer].name[len - 1] == '\n')
        {
            Bank->Customers[Bank->Customer_pointer].name[len - 1] = '\0';
        }

        // 2. ID
        print_centered("Please enter the customer's unique ID:");
        printf("|%*s", (MENU_WIDTH - 1) / 2, "");
        scanf("%lu", &(Bank->Customers[Bank->Customer_pointer].ID));

        if ((Bank->Customers[Bank->Customer_pointer].ID) >= Max_Customers ||
            !Check_Repeated_Customer_ID(Bank, Bank->Customers[Bank->Customer_pointer].ID))
        {
            print_centered("The entered ID is either invalid or already exists.");
            state = OP_Failed;
        }

        // 3. Phone Number
        print_centered("Please enter the customer's phone number:");
        printf("|%*s", (MENU_WIDTH - 1) / 2, "");
        scanf("%lu", &(Bank->Customers[Bank->Customer_pointer].Phone_Number));

        if (!valid_phone_number(Bank->Customers[Bank->Customer_pointer].Phone_Number) ||
            !Check_Repeated_Customer_Phone_Number(Bank, Bank->Customers[Bank->Customer_pointer].Phone_Number))
        {
            print_centered("The phone number entered is invalid or already exists.");
            state = OP_Failed;
        }

        // 4. Cash Amount
        print_centered("Please enter the initial cash amount:");
        printf("|%*s", (MENU_WIDTH - 1) / 2, "");
        scanf("%lu", &(Bank->Customers[Bank->Customer_pointer].Cash_Amount));
        getchar();

        if (state == OP_Success)
        {
            // Write to database
            fprintf(data_base, "Customer [%lu]\n", (Bank->Customer_pointer) + 1);
            fprintf(data_base, "Customer's Name : %s\n", Bank->Customers[Bank->Customer_pointer].name);
            fprintf(data_base, "Customer's ID : %06lu\n", Bank->Customers[Bank->Customer_pointer].ID);
            fprintf(data_base, "Customer's Phone Number : 0%010lu\n", Bank->Customers[Bank->Customer_pointer].Phone_Number);
            fprintf(data_base, "Customer's Cash Amount : %lu\n", Bank->Customers[Bank->Customer_pointer].Cash_Amount);
            fprintf(data_base, "------------------------------------------------------------------------\n");

            Bank->Customer_pointer++;
            fflush(data_base);
            print_centered("Customer created successfully.");
        }
        else
        {
            print_centered("Customer creation failed. Please try again.");
        }

        print_line();
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

    rewind(data_base);

    sint8 line[1024];
    uint32 index = 0;

    while (fgets(line, sizeof(line), data_base) != NULL)
    {
        if (strncmp(line, "Customer [", 10) == 0)
        {
            // READ NAME
            if (fgets(line, sizeof(line), data_base) == NULL)
                break;
            sscanf(line, "Customer's Name : %[^\n]", Bank->Customers[index].name);

            // READ ID
            if (fgets(line, sizeof(line), data_base) == NULL)
                break;
            sscanf(line, "Customer's ID : %lu", &Bank->Customers[index].ID);

            // READ PHONE NUMBER
            if (fgets(line, sizeof(line), data_base) == NULL)
                break;
            sscanf(line, "Customer's Phone Number : %lu", &Bank->Customers[index].Phone_Number);

            // READ CASH
            if (fgets(line, sizeof(line), data_base) == NULL)
                break;
            sscanf(line, "Customer's Cash Amount : %lu", &Bank->Customers[index].Cash_Amount);

            // IGNORE LINE
            fgets(line, sizeof(line), data_base);

            index++;
        }
    }

    Bank->Customer_pointer = index;

    return (state);
}

/**
 * @brief            : Edit an existing customer's name and phone number
 * @param Bank       : Pointer to the System structure that holds customer data
 * @param data_base  : File pointer to the database file for updating customer information
 * @param id         : The unique ID of the customer to be edited
 * @return           : Status of the function
 *      - (E_OK)     : Customer updated successfully and changes saved to file
 *      - (E_NOT_OK) : Failed due to invalid ID, duplicate/invalid phone number, or null pointers
 */

Error_States Edit_Customer(System *Bank, FILE *data_base, uint32 id) {
    Error_States state = OP_Success;

    if (NULL == Bank || NULL == data_base) {
        state = OP_Failed;
    } else {
        sint32 index = find_customer_id(Bank, id);

        print_line();
        print_centered("Edit Customer Information");
        print_line();

        if (index == -1) {
            char msg[100];
            snprintf(msg, sizeof(msg), "Customer with ID %lu not found.", id);
            print_centered(msg);
            state = OP_Failed;
        } else {
            print_centered("Please update the customer's name:");
            printf("|%*s", (MENU_WIDTH - 1) / 2, "");
            getchar();
            fgets(Bank->Customers[index].name, sizeof(Bank->Customers[index].name), stdin);

            uint32 len = strlen(Bank->Customers[index].name);
            if (len > 0 && Bank->Customers[index].name[len - 1] == '\n') {
                Bank->Customers[index].name[len - 1] = '\0';
            }

            print_centered("Please update the customer's phone number:");
            printf("|%*s", (MENU_WIDTH - 1) / 2, "");
            scanf("%lu", &(Bank->Customers[index].Phone_Number));

            if (!valid_phone_number(Bank->Customers[index].Phone_Number) ||
                !Check_Repeated_Customer_Phone_Number(Bank, Bank->Customers[Bank->Customer_pointer].Phone_Number)) {
                print_centered("The phone number entered is invalid or already exists.");
                state = OP_Failed;
            } else {
                state = Data_Base_Overwrite_data(Bank, data_base);
                if (state == OP_Success) {
                    print_centered("Customer data has been successfully updated.");
                } else {
                    print_centered("Failed to update customer data.");
                }
            }
        }

        print_line();
    }

    return state;
}

/**
 * @brief            : Display the information of a specific customer by ID
 * @param Bank       : Pointer to the System structure containing all customer records
 * @param id         : Unique ID of the customer to display
 * @return           : Status of the function
 *      - (E_OK)     : Customer found and information displayed
 *      - (E_NOT_OK) : Customer not found or Bank pointer is NULL
 */

Error_States Display_Customer_information(System *Bank, uint32 id)
{
    Error_States state = OP_Success;

    if (NULL == Bank)
    {
        state = OP_Failed;
    }
    else
    {
        sint32 index = find_customer_id(Bank, id);

        print_line();
        print_centered("Customer Information");
        print_line();

        if (index == -1)
        {
            char msg[100];
            snprintf(msg, sizeof(msg), "Customer with ID %lu not found.", id);
            print_centered(msg);
            state = OP_Failed;
        }
        else
        {
            char buffer[256];

            snprintf(buffer, sizeof(buffer), "Customer's Name : %s", Bank->Customers[index].name);
            print_centered(buffer);

            snprintf(buffer, sizeof(buffer), "Customer's ID : %06lu", Bank->Customers[index].ID);
            print_centered(buffer);

            snprintf(buffer, sizeof(buffer), "Phone Number : 0%010lu", Bank->Customers[index].Phone_Number);
            print_centered(buffer);

            snprintf(buffer, sizeof(buffer), "Cash Amount : %lu", Bank->Customers[index].Cash_Amount);
            print_centered(buffer);
        }

        print_line();
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

        print_line();
        print_centered("Delete Customer");
        print_line();

        if (index == -1)
        {
            char msg[100];
            snprintf(msg, sizeof(msg), "Customer with ID %06lu not found.", id);
            print_centered(msg);
            state = OP_Failed;
        }
        else
        {
            state = Bank_Shift_Customers(Bank, index);
            if (state == OP_Success)
            {
                Bank->Customer_pointer--;

                state = Data_Base_Overwrite_data(Bank, data_base);
                if (state == OP_Success)
                {

                    print_centered("The customer has been successfully removed from the system.");
                }
                else
                {
                    print_centered("An error occurred while removing the customer's data.");
                }
            }
            else
            {
                print_centered("An error occurred while shifting customer records.");
            }
        }

        print_line();
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

    if (NULL == Bank || NULL == data_base)
    {
        state = OP_Failed;
    }
    else
    {
        sint32 C1 = find_customer_id(Bank, C_id);
        sint32 C2 = find_customer_id(Bank, D_id);

        print_line();
        print_centered("Transfer Funds Between Customers");
        print_line();

        if (C1 == -1 || C2 == -1)
        {
            print_centered("One or both customer IDs not found.");
            state = OP_Failed;
        }
        else
        {
            if (Bank->Customers[C1].Cash_Amount >= T_money)
            {
                char buffer[256];

                Bank->Customers[C1].Cash_Amount -= T_money;

                snprintf(buffer, sizeof(buffer),
                         "Funds transferred from %s (ID: %06lu): %lu",
                         Bank->Customers[C1].name,
                         Bank->Customers[C1].ID,
                         T_money);
                print_centered(buffer);

                snprintf(buffer, sizeof(buffer),
                         "Updated balance for sender: %lu",
                         Bank->Customers[C1].Cash_Amount);
                print_centered(buffer);

                Bank->Customers[C2].Cash_Amount += T_money;

                snprintf(buffer, sizeof(buffer),
                         "Funds received by %s (ID: %06lu) from %s (ID: %06lu)",
                         Bank->Customers[C2].name,
                         Bank->Customers[C2].ID,
                         Bank->Customers[C1].name,
                         Bank->Customers[C1].ID);
                print_centered(buffer);

                snprintf(buffer, sizeof(buffer),
                         "Updated balance for receiver: %lu",
                         Bank->Customers[C2].Cash_Amount);
                print_centered(buffer);

                state = Data_Base_Overwrite_data(Bank, data_base);
                if (state == OP_Success)
                {

                    print_centered("Transfer completed successfully.");
                }
                else
                {
                    print_centered("Transfer failed during data update.");
                }
            }
            else
            {
                print_centered("Insufficient balance to complete the transfer.");
                state = OP_Failed;
            }
        }

        print_line();
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

    print_line();
    print_centered("Deposit Money");
    print_line();

    if (NULL == Bank || NULL == data_base || money == 0)
    {
        state = OP_Failed;
    }
    else
    {
        sint32 index = find_customer_id(Bank, id);
        if (index == -1)
        {
            char msg[100];
            snprintf(msg, sizeof(msg), "Customer with ID %lu not found.", id);
            print_centered(msg);
            state = OP_Failed;
        }
        else
        {
            Bank->Customers[index].Cash_Amount += money;
            state = Data_Base_Overwrite_data(Bank, data_base);
            if (state == OP_Success)
            {
                char msg[100];

                snprintf(msg, sizeof(msg), "Deposit completed successfully. New balance: %lu",
                         Bank->Customers[index].Cash_Amount);
                print_centered(msg);
            }
            else
            {
                print_centered("Deposit failed.");
            }
        }
    }

    print_line();
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

    print_line();
    print_centered("Withdraw Money");
    print_line();

    if (NULL == Bank || NULL == data_base || money == 0)
    {
        state = OP_Failed;
    }
    else
    {
        sint32 index = find_customer_id(Bank, id);
        if (index == -1)
        {
            char msg[100];
            snprintf(msg, sizeof(msg), "Customer with ID %lu not found.", id);
            print_centered(msg);
            state = OP_Failed;
        }
        else
        {
            if (Bank->Customers[index].Cash_Amount >= money)
            {
                Bank->Customers[index].Cash_Amount -= money;
                state = Data_Base_Overwrite_data(Bank, data_base);
                if (state == OP_Success)
                {
                    char msg[100];
                    snprintf(msg, sizeof(msg), "Withdrawal completed successfully. New balance: %lu",
                             Bank->Customers[index].Cash_Amount);
                    print_centered(msg);
                }
                else
                {
                    print_centered("Withdrawal failed.");
                }
            }
            else
            {
                print_centered("Insufficient balance to perform the withdrawal.");
                state = OP_Failed;
            }
        }
    }

    print_line();
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
    sint32 index = 0;
    for (uint32 i = 0; i <= Bank->Customer_pointer; i++)
    {
        if (target_id == Bank->Customers[i].ID)
        {
            index = i;
            break;
        }
        else
        {
            index = -1;
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
    else
    {
        for (uint32 i = 0; i < Bank->Customer_pointer; i++)
        {
            if (id == Bank->Customers[i].ID)
            {
                flag = false;
                break;
            }
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
    else
    {
        for (uint32 i = 0; i < Bank->Customer_pointer; i++)
        {
            if (phone == Bank->Customers[i].Phone_Number)
            {
                flag = false;
                break;
            }
        }
    }
    return (flag);
}