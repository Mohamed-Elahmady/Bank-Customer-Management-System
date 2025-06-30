// gcc Main.c Banking_Operations.c -o Main 
// ./Main

#include "Banking_Operations.h"

Error_States retval = OP_Success;
sint8 OPCode;
uint32 target_customer, C_money; // for only on customer operations
uint32 C_id,D_id,t_money;

int main(){

    System *Bank = System_init(&retval); 
    FILE *data_base = fopen(FILE_PATH, "a+");

    Load_Customers_From_File(Bank, data_base);
    label:
    printf("------------------------------------------------------------------------\n");
    printf("Please select the desired operation:\n");
    printf("To add a new customer, press '1'\n");
    printf("To edit customer information, press '2'\n");
    printf("To display customer information, press '3'\n");
    printf("To delete customer information, press '4'\n");
    printf("To transfer funds between customers, press '5'\n");
    printf("To deposit money, press '6'\n");
    printf("To withdraw money, press '7'\n");
    printf("To exit the system, press '8'\n");
    printf("------------------------------------------------------------------------\n");

    scanf("%c", &OPCode);
    switch (OPCode)
    {
    case '1':
        retval = Create_Customer(Bank, data_base);
        delay(delay_time);
        break;
    case '2':
        printf("Enter the customer ID to modify their data: ");
        scanf("%06lu", &target_customer);
        retval = Edit_Customer(Bank, data_base, target_customer);
        target_customer = 0;
        delay(delay_time);
        break;
    case '3':
        printf("Enter the customer ID to view their information: ");
        scanf("%06lu", &target_customer);
        retval = Display_Customer_information(Bank, target_customer);
        target_customer = 0;
        delay(delay_time);
        break;
    case '4':
        printf("Enter the customer ID to remove them from the system: ");
        scanf("%06lu", &target_customer);
        retval = Delete_Customer(Bank, data_base, target_customer);
        target_customer = 0;
        delay(delay_time);
        break;
    case '5':
        printf("Enter the sender's customer ID: ");
        scanf("%lu", &C_id);
        printf("Enter the recipient's customer ID: ");
        scanf("%lu", &D_id);
        printf("Enter the amount you wish to transfer: ");
        scanf("%lu", &t_money);
        retval = Customer_Transfer_Money(Bank, data_base, C_id, D_id, t_money);
        C_id = 0;
        D_id = 0;
        t_money = 0;
        delay(delay_time);
        break;
    case '6':
        printf("Enter the customer's ID to proceed with the deposit: ");
        scanf("%06lu", &target_customer);
        printf("Enter the amount to deposit: ");
        scanf("%lu",&C_money);
        retval = Customer_Deposit_Money(Bank, data_base, target_customer, C_money);
        target_customer = 0;
        C_money = 0;
        delay(delay_time);
        break;
    case '7':
        printf("Enter the customer's ID to proceed with the withdrawal: ");
        scanf("%06lu", &target_customer);
        printf("Enter the amount to withdraw: ");
        scanf("%lu",&C_money);
        retval = Customer_Withdraw_Money(Bank, data_base, target_customer, C_money);
        target_customer = 0;
        C_money = 0;
        delay(delay_time);
        break;
    case '8':
        exit(1);
        break;
    default:
        printf("Invalid operation selected. Please try again.\n");
        goto label;
        break;
    }
    goto label;

    return (EXIT_SUCESS);

}