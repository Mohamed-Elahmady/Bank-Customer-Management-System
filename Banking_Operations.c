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

Error_States Create_Customer(System *Bank, FILE *data_base)
{
    Error_States state = OP_Success;
    if (NULL == Bank || NULL == data_base)
    {
        state = OP_Failed;
    }
    else
    {
            printf("Please enter the customer's full name: ");
        getchar();
        fgets(Bank->Customers[Bank->Customer_pointer].name, sizeof(Bank->Customers[Bank->Customer_pointer].name), stdin);

        uint32 len = strlen(Bank->Customers[Bank->Customer_pointer].name);
        if (len > 0 && Bank->Customers[Bank->Customer_pointer].name[len - 1] == '\n')
        {
            Bank->Customers[Bank->Customer_pointer].name[len - 1] = '\0';
        }

        printf("Please enter the customer's unique ID from (%06d to %06d): ", System_Empty, (Max_Customers-1));
        scanf("%lu", &(Bank->Customers[Bank->Customer_pointer].ID));

        if ((Bank->Customers[Bank->Customer_pointer].ID) >= Max_Customers || !Check_Repeated_Customer_ID(Bank, Bank->Customers[Bank->Customer_pointer].ID))
        {
            printf("The entered ID is either invalid or already exists. Please try again.\n");
            state = OP_Failed;
        }

        printf("Please enter the customer's phone number: ");
        scanf("%lu", &(Bank->Customers[Bank->Customer_pointer].Phone_Number));
        if (!valid_phone_number(Bank->Customers[Bank->Customer_pointer].Phone_Number) || !Check_Repeated_Customer_Phone_Number(Bank, (Bank->Customers[Bank->Customer_pointer].Phone_Number)))
        {
            printf("The phone number entered is invalid or already exists.\n");
            state = OP_Failed;
        }

        printf("Please enter the initial cash amount for the customer: ");
        scanf("%lu", &(Bank->Customers[Bank->Customer_pointer].Cash_Amount));
        getchar();

        fprintf(data_base, "Customer [%lu]\n", (Bank->Customer_pointer) + 1);
        fprintf(data_base, "Customer's Name : %s\n", Bank->Customers[Bank->Customer_pointer].name);
        fprintf(data_base, "Customer's ID : %06lu\n", Bank->Customers[Bank->Customer_pointer].ID);
        fprintf(data_base, "Customer's Phone Number : 0%010lu\n", Bank->Customers[Bank->Customer_pointer].Phone_Number);
        fprintf(data_base, "Customer's Cash Amount : %lu\n", Bank->Customers[Bank->Customer_pointer].Cash_Amount);
        fprintf(data_base, "------------------------------------------------------------------------\n");

        (Bank->Customer_pointer)++;
    }

    return (state);
}

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

Error_States Edit_Customer(System *Bank, FILE *data_base, uint32 id)
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
            printf("Customer with ID %lu not found.\n", id);
            state = OP_Failed;
        }
        else
        {
            printf("Please update the customer's name: ");
            getchar();
            fgets(Bank->Customers[index].name, sizeof(Bank->Customers[index].name), stdin);
            uint32 len = strlen(Bank->Customers[index].name);
            if (len > 0 && Bank->Customers[index].name[len - 1] == '\n')
            {
                Bank->Customers[index].name[len - 1] = '\0';
            }

            printf("Please update the customer's phone number: ");
            scanf("%lu", &(Bank->Customers[index].Phone_Number));

            if (!valid_phone_number(Bank->Customers[index].Phone_Number) || !Check_Repeated_Customer_Phone_Number(Bank, (Bank->Customers[Bank->Customer_pointer].Phone_Number)))
            {
                printf("The phone number entered is invalid or already exists.\n");
                state = OP_Failed;
            }
            else
            {
                state = Data_Base_Overwrite_data(Bank, data_base);
                if (OP_Success == state)
                {
                    printf("Customer data has been successfully updated.\n");
                }
                else
                {
                    printf("Failed to update customer data.\n");
                }
            }
        }
    }
    return (state);
}

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
        if (index == -1)
        {
            printf("Customer with ID %lu not found.\n", id);
            state = OP_Failed;
        }
        else
        {
            printf("Customer's Name : %s\n", Bank->Customers[index].name);
            printf("Customer's ID : %06lu\n", Bank->Customers[index].ID);
            printf("Customer's Phone Number : 0%010lu\n", Bank->Customers[index].Phone_Number);
            printf("Customer's Cash Amount : %lu\n", Bank->Customers[index].Cash_Amount);
        }
    }
    return (state);
}

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
            printf("Customer with ID %lu not found.\n", id);
            state = OP_Failed;
        }
        else
        {

            state = Bank_Shift_Customers(Bank, index);
            if (OP_Success == state)
            {
                (Bank->Customer_pointer)--;
                state = Data_Base_Overwrite_data(Bank, data_base);
                if (OP_Success == state)
                {
                    printf("The customer has been successfully removed from the system.\n");
                }
                else
                {
                    printf("An error occurred while removing the customer's data.\n");
                }
            }
            else
            {
                printf("An error occurred while shifting customer records.\n");
            }
        }
    }
    return (state);
}

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

        if (C1 == -1 || C2 == -1)
        {
            printf("One or both customer IDs not found.\n");
            state = OP_Failed;
        }
        else
        {
            if (Bank->Customers[C1].Cash_Amount >= T_money)
            {
                printf("Funds have been transferred from %s (ID: %06lu) in the amount of %lu\n",
                       Bank->Customers[C1].name,
                       Bank->Customers[C1].ID,
                       T_money);

                Bank->Customers[C1].Cash_Amount -= T_money;

                printf("Updated balance for %s (ID: %06lu): %lu\n",
                       Bank->Customers[C1].name,
                       Bank->Customers[C1].ID,
                       Bank->Customers[C1].Cash_Amount);

                printf("Transfer successful.\n");

                Bank->Customers[C2].Cash_Amount += T_money;

                printf("Funds received by %s (ID: %06lu) from %s (ID: %06lu)\n",
                       Bank->Customers[C2].name,
                       Bank->Customers[C2].ID,
                       Bank->Customers[C1].name,
                       Bank->Customers[C1].ID);

                printf("Updated balance for %s (ID: %06lu): %lu\n",
                       Bank->Customers[C2].name,
                       Bank->Customers[C2].ID,
                       Bank->Customers[C2].Cash_Amount);

                state = Data_Base_Overwrite_data(Bank, data_base);
                if (OP_Success == state)
                {
                    printf("Transfer completed successfully.\n");
                }
                else
                {
                    printf("Transfer failed during data update.\n");
                }
            }
            else
            {
                printf("Insufficient balance to complete the transfer.\n");
                state = OP_Failed;
            }
        }
    }
    return state;
}

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
            printf("Customer with ID %lu not found.\n", id);
            state = OP_Failed;
        }
        else
        {
            Bank->Customers[index].Cash_Amount += money;
            state = Data_Base_Overwrite_data(Bank, data_base);
            if (OP_Success == state)
            {
                printf("Deposit completed successfully. New balance: %lu\n", Bank->Customers[index].Cash_Amount);
            }
            else
            {
                printf("Deposit failed.\n");
            }
        }
    }
    return (state);
}

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
            printf("Customer with ID %lu not found.\n", id);
            state = OP_Failed;
        }
        else
        {
            if (Bank->Customers[index].Cash_Amount >= money)
            {
                Bank->Customers[index].Cash_Amount -= money;
                state = Data_Base_Overwrite_data(Bank, data_base);
                if (OP_Success == state)
                {
                    printf("Withdrawal completed successfully. New balance: %lu\n", Bank->Customers[index].Cash_Amount);
                }
                else
                {
                    printf("Withdrawal failed.\n");
                }
            }
            else
            {
                printf("Insufficient balance to perform the withdrawal.\n");
            }
        }
    }
    return (state);
}

/******************* Section 2 :  Helper Functions Definitions *******************/

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
        }
    }
    return (state);
}

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