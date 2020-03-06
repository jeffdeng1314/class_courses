#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

//This function generates the random key for the encryption and decryption
int main(int argc, char *argv[])
{

    //time seed
    srand(time(NULL));
    int x = atoi(argv[1]);
    int random_number;
    int array[27], random_array[x];
    for(int i=0;i<26;i++) //randomly picks from the alphabet array
        array[i] = 'A'+i;
    array[26] = ' ';



    for(int i=0;i<x;i++){

        random_number = rand()%27;
        random_array[i] = array[random_number];
        printf("%c",random_array[i]); //print them out to a key file
    }

    printf("\n");

    
    return 0;
}
