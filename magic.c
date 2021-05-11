
#include <stdio.h>
int main() {
   int num = 1414942034;
   
   printf("%02X:%02X:%02X:%02X", ((unsigned char *)(&num))[0],((unsigned char *)(&num))[1], ((unsigned char *)(&num))[2], ((unsigned char *)(&num))[3]);  
   return 0;
}