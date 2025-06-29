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
        
        break;
    case 3:
        printf("Please Enter Customer ID : ");
        scanf("%ld", &target_customer);
        retval = Display_Customer_information(Bank,target_customer);
        break;
    case 4:
        break;
    case 5:
        
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
