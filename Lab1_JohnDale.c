/***********************************************
*   Hw1.cpp File which does the tasks in HW1
*
* Revision History
*Date       Author      Revision
*2/11/22    John Dale   initial code write
************************************************/

# include <stdio.h>

binary_conv(int val){
    int bit1,bit2,bit3,bit4,bit5,bit6,bit7,bit8;
        bit1 = val%2;
            val = val/2;
        bit2 = val%2;
            val = val/2;
        bit3 = val%2;
            val = val/2;
        bit4 = val%2;
            val = val/2;
        bit5 = val%2;
            val = val/2;
        bit6 = val%2;
            val= val/2;
        bit7 = val%2;
            val = val/2;
        bit8 = val%2;
    printf(" %d%d%d%d%d%d%d%d\n",bit8,bit7,bit6,bit5,bit4,bit3,bit2,bit1);
    return 0 ;
}


int main(void){
    int input_value1=1;
    int input_value2=1;


    while (input_value1!=0 & input_value2!=0){
    printf("Please enter number & base separated by a space (enter 0 0 to quit...)");
    scanf("%d %d", &input_value1, &input_value2);

    if (input_value2==10){
        printf("%d in base %d = %d\n",input_value1,input_value2,input_value1);
        continue;}
    else if (input_value2==16){
        printf("%d in base %d = %X\n",input_value1,input_value2,input_value1);
        continue;}
    else if (input_value2==8){
        printf("%d in base %d = %o\n",input_value1,input_value2,input_value1);
        continue;}
    else if (input_value2==2){
        printf("%d in base %d =",input_value1,input_value2);
        binary_conv(input_value1);
        continue;}
    else if (input_value1==0 & input_value2==0){
        break;}
    else {
    printf("%d is not a valid base, please choose 16, 10, 8 or 2\n",input_value2);
        continue;} 
    }
    
    printf("You Have exited");
}

