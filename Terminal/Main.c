/**
 * @file      : Main.c
 * @authors   : Mohammed-Elahmady, Hatem Ayman, Mayar Basha, A'laa Abd-Elhady
 * @brief     : Bank Customer Management System
 * @date      : Sat 28 Jun 2025
 */
#include "Banking_Operations.h"

Error_States retval = OP_Success;
uint16 OPCode;
uint32 target_customer, C_money;
uint32 C_id, D_id, t_money;

int main()
{
    System *Bank = System_init(&retval);
    FILE *data_base = fopen(FILE_PATH, "a+");
    Load_Customers_From_File(Bank, data_base);
    
    menu_label:
    print_line();
    print_centered("Please select the desired operation:");
    print_centered("To add a new customer, press '1'");
    print_centered("To edit customer information, press '2'");
    print_centered("To display customer information, press '3'");
    print_centered("To delete customer information, press '4'");
    print_centered("To transfer funds between customers, press '5'");
    print_centered("To deposit money, press '6'");
    print_centered("To withdraw money, press '7'");
    print_centered("To exit the system, press '8'");
    print_line();

    printf("|%*s", (MENU_WIDTH - 1) / 2, "");
    scanf("%hd", &OPCode);
    print_line();

    switch (OPCode)
    {
    case 1:
        print_centered("Creating new customer...");
        retval = Create_Customer(Bank, data_base);
        delay(delay_time);
        break;

    case 2:
        print_centered("Enter the customer ID to modify their data:");
        printf("|%*s", (MENU_WIDTH - 1) / 2, "");
        scanf("%06lu", &target_customer);
        retval = Edit_Customer(Bank, data_base, target_customer);
        target_customer = 0;
        delay(delay_time);
        break;

    case 3:
        print_centered("Enter the customer ID to view their information:");
        printf("|%*s", (MENU_WIDTH - 1) / 2, "");
        scanf("%06lu", &target_customer);
        retval = Display_Customer_information(Bank, target_customer);
        target_customer = 0;
        delay(delay_time);
        break;

    case 4:
        print_centered("Enter the customer ID to remove them from the system:");
        printf("|%*s", (MENU_WIDTH - 1) / 2, "");
        scanf("%06lu", &target_customer);
        retval = Delete_Customer(Bank, data_base, target_customer);
        target_customer = 0;
        delay(delay_time);
        break;

    case 5:
        print_centered("Enter the sender's customer ID:");
        printf("|%*s", (MENU_WIDTH - 1) / 2, "");
        scanf("%lu", &C_id);

        print_centered("Enter the recipient's customer ID:");
        printf("|%*s", (MENU_WIDTH - 1) / 2, "");
        scanf("%lu", &D_id);

        print_centered("Enter the amount you wish to transfer:");
        printf("|%*s", (MENU_WIDTH - 1) / 2, "");
        scanf("%lu", &t_money);

        retval = Customer_Transfer_Money(Bank, data_base, C_id, D_id, t_money);
        C_id = D_id = t_money = 0;
        delay(delay_time);
        break;

    case 6:
        print_centered("Enter the customer's ID to proceed with the deposit:");
        printf("|%*s", (MENU_WIDTH - 1) / 2, "");
        scanf("%06lu", &target_customer);

        print_centered("Enter the amount to deposit:");
        printf("|%*s", (MENU_WIDTH - 1) / 2, "");
        scanf("%lu", &C_money);

        retval = Customer_Deposit_Money(Bank, data_base, target_customer, C_money);
        target_customer = C_money = 0;
        delay(delay_time);
        break;

    case 7:
        print_centered("Enter the customer's ID to proceed with the withdrawal:");
        printf("|%*s", (MENU_WIDTH - 1) / 2, "");
        scanf("%06lu", &target_customer);

        print_centered("Enter the amount to withdraw:");
        printf("|%*s", (MENU_WIDTH - 1) / 2, "");
        scanf("%lu", &C_money);

        retval = Customer_Withdraw_Money(Bank, data_base, target_customer, C_money);
        target_customer = C_money = 0;
        delay(delay_time);
        break;

    case 8:
        print_centered("Thank you for using the system. Exiting...");
        print_line();
        exit(0);

    default:
        print_centered("Invalid operation selected. Please try again.");
        delay(delay_time);
        goto menu_label;
    }

    goto menu_label;
    return EXIT_SUCCESS;
}

void print_line()
{
    for (int i = 0; i < MENU_WIDTH; ++i)
        printf("-");
    printf("\n");
}

void print_centered(const char *message)
{
    int len = strlen(message);
    int padding = (MENU_WIDTH - len - 2) / 2;
    printf("|%*s%s%*s|\n", padding, "", message, padding + ((len % 2 == 0) ? 0 : 1), "");
}