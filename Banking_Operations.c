******************* Section 0 : Includes *******************/
/
#include "Banking_Operations.h"

/******************* Section 1 :  Variables Definitions *******************/

/******************* Section 2 :  Helper Functions Declarations *******************/

static bool valid_phone_number(uint32 num);
static sint32 find_customer_id(System *Bank, uint32 target_id);
static Error_States Data_Base_Overwrite_data(System *Bank, FILE *data_base);
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
            Bank->Customer_pointer = System_Empty;
            (Bank->Customer_pointer) = 0;
        }
    }
    return (Bank);
}

Error_States Create_Customer(System *Bank, FILE *data_base){
    Error_States state = OP_Success;
    if (NULL == Bank || NULL == data_base)
    {
        state = OP_Failed;
    }
    else
    {
        printf("Please Enter Customer Name : ");
        getchar();
        fgets(Bank->Customers[Bank->Customer_pointer].name, sizeof(Bank->Customers[Bank->Customer_pointer].name), stdin);

        uint32 len = strlen(Bank->Customers[Bank->Customer_pointer].name);
        if (len > 0 && Bank->Customers[Bank->Customer_pointer].name[len - 1] == '\n')
        {
            Bank->Customers[Bank->Customer_pointer].name[len - 1] = '\0';
        }

        printf("Please Enter Customer ID : ");
        scanf("%ld", &(Bank->Customers[Bank->Customer_pointer].ID));

        if ((Bank->Customers[Bank->Customer_pointer].ID) >= Max_Customers || !Check_Repeated_Customer_ID(Bank, Bank->Customers[Bank->Customer_pointer].ID))
        {
            printf("Invalid or Repeated Customer ID. Please try again.\n");
            state = OP_Failed;
        }

        printf("Please Enter Customer Phone Number : ");
        scanf("%ld", &(Bank->Customers[Bank->Customer_pointer].Phone_Number));
        if (!valid_phone_number(Bank->Customers[Bank->Customer_pointer].Phone_Number) || !Check_Repeated_Customer_Phone_Number(Bank, (Bank->Customers[Bank->Customer_pointer].Phone_Number)))
        {
            printf("Invalid or Repeated Customer Phone Number.\n");
            state = OP_Failed;
        }

        printf("Please Enter Customer Cash Amount : ");
        scanf("%ld", &(Bank->Customers[Bank->Customer_pointer].Cash_Amount));
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

    char line[1024];
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
        printf("Error while checking data, try again later!\n");
        state = OP_Failed;
    }   
    sint32 index = find_customer_id(Bank,id);
    // if user is not found
    if (index == -1)    
    {
        printf("Customer Id Not Found, try again!\n");
        return state;
    }    
    // if user is found
    else
    {
        printf("Customer Report for %s:\n", Bank->Customers[index].name);
        printf("Customer's ID: %lu\n", Bank->Customers[index].ID);
        printf("Customer's Phone Number: %lu\n", Bank->Customers[index].Phone_Number);
        printf("Customer's Cash Amoubt: %lu\n", Bank->Customers[index].Cash_Amount);   
    }
    return state;
}
//Delete Customer
Error_States Delete_Customer(System *Bank, FILE *data_base, uint32 id)
{
   Error_States state = OP_Failed;


    sint32 index = find_customer_id(Bank, id);
    if (index == -1)
    {
        printf("Customer with ID %lu not found.\n", id);
        return OP_Failed;
    }

    for (uint32 i = index; i < Bank->Customer_pointer - 1; i++)
    {
        Bank->Customers[i] = Bank->Customers[i + 1];
    }

    Bank->Customer_pointer--;

    state = Data_Base_Overwrite_data(Bank, data_base);

    return state;
}
//transfer money
Error_States Customer_Transfer_Money(System *Bank, FILE *data_base, uint32 C_id, uint32 D_id, uint32 T_money)
{
    Error_States state = OP_Failed;
sint32 index_1 = find_customer_id(Bank, C_id);
    if (index_1 == -1)
    {
        printf("Customer with ID %lu not found.\n", C_id);
        return OP_Failed;
    }
    sint32 index_2 = find_customer_id(Bank, D_id);
    if (index_2 == -1)
    {
        printf("Customer with ID %lu not found.\n", D_id);
        return OP_Failed;
    }
    if ( T_money > Bank->Customers[index_1].Cash_Amount){
        printf("you don't have enough money");
        return OP_Failed;
    }else{
     Bank->Customers[index_1].Cash_Amount -= T_money;
        Bank->Customers[index_2].Cash_Amount += T_money;
    state = Data_Base_Overwrite_data(Bank, data_base);
  return state;
    }
}
/******************* Section 2 :  Helper Functions Definitions *******************/

static bool valid_phone_number(uint32 num){
    bool flag = true;
    for (int i = 0; i < 8; i++)
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

static sint32 find_customer_id(System *Bank, uint32 target)
{
    sint32 index = 0;
    for (uint32 i = 0; i <= Bank->Customer_pointer; i++)
    {
        if (target == Bank->Customers[i].ID)
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

static bool Check_Repeated_Customer_ID(System *Bank, uint32 id){
    bool flag = true;
    if(NULL == Bank){
        flag = false;
    }
    else
    {
        for (int i = 0; i < Bank->Customer_pointer; i++)
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

static bool Check_Repeated_Customer_Phone_Number(System *Bank, uint32 phone){
    bool flag = true;
    if(NULL == Bank){
        flag = false;
    }
    else
    {
        for (int i = 0; i < Bank->Customer_pointer; i++)
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
