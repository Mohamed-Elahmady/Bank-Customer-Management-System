#include "Banking_Operations.h"

Error_States retval = OP_Success;
uint8 OPCode;
uint32 target_customer = 0;
uint32 C_id,D_id,t_money;

int main(){

    System *Bank = System_init(&retval); 
    FILE *data_base = fopen(FILE_PATH, "a+");

    Load_Customers_From_File(Bank, data_base);
    label:
    printf("------------------------------------------------------------------------\n");
    printf("What is the Operation u need : \n");
    printf("For Add new Customer Press \'1\' \n");
    printf("For Edit Customer Information \'2\'\n");
    printf("For Print Customer Information Press \'3\' \n");
    printf("For Delete Customer Information Press \'4\'\n");
    printf("For Transfer Function to Another Customer Press \'5\'\n");
    printf("For Depositing Money Press \'6\' \n");
    printf("For Withdrawing Money Press \'7\'\n");
    printf("For Exit System Press \'8\'\n");
    printf("------------------------------------------------------------------------\n");

    scanf("%d", &OPCode);
    switch (OPCode)
    {
    case 1:
        retval = Create_Customer(Bank, data_base);
        delay(delay_time);
        break;
    case 2:
        printf("Enter your ID: ");
        scanf("%ld", &target_customer);
        retval = Edit_Customer(Bank,data_base,target_customer);
        break;
    case 3:
        printf("Please Enter Customer ID : ");
        scanf("%ld", &target_customer);
        retval = Display_Customer_information(Bank,target_customer);
        break;
    case 4:
          printf("Please Enter Customer ID : ");
        scanf("%ld", &target_customer);

if (data_base == NULL)
{
    printf("Unable to open file.\n");
}else {
    if (Delete_Customer(Bank,data_base,target_customer) == OP_Success)
        printf("Customer deleted successfully.\n");
    else
        printf("Failed to delete customer.\n");

    fclose(data_base);
}
        break;
    case 5:
          printf("Enter the sender ID: ");
     scanf("%ld", &C_id);
       printf("Enter the receiver ID: ");
     scanf("%ld", &D_id);
       printf("Enter tne tranfer money: ");
     scanf("%ld", &t_money);
     if (data_base == NULL)
{
    printf("Unable to open file.\n");
}else{
if (Customer_Transfer_Money(Bank,data_base,C_id,D_id,t_money) == OP_Success){
   printf("operation success\n");

}
    else
        printf("operation Failed .\n");
        fclose(data_base);
        }
        break;
    case 6:
        break;
    case 7:
        break;
    case 8:
        exit(1);
        break;
    default:
        printf("invalid operation, please try again !!\n");
        goto label;
        break;
    }
    goto label;

    return (EXIT_SUCESS);
}
