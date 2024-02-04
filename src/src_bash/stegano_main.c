#include <stdlib.h>
#include <stdio.h>
#include <string.h>
//#include "pngwrap.h"
//#include "message.h"

int user_friendly(char** argv);
void auto_friendly(char** argv);



int main(int argc, char** argv)
{
    if (argc < 2)
    {
        printf("Pas assez d'arguments\n");
        return 1;
    }

    else
    {
        if (strcmp(argv[1], "-user") == 0)
        {
            user_friendly(argv);
        } 
        else if ((strcmp(argv[1], "-auto") == 0))
        {
          //  auto_friendly(argv);
        }
    }

    return 0;
}



int user_friendly(char** argv)
{
    if (strcmp(argv[1], "-txt") == 0)
    {
        printf("Option -txt selected\n");
        printf("Input Text Path: \n>> ");
        

    }
 
    else if (strcmp(argv[1], "-png") == 0)
    {
        printf("Option -png selected\n");
        printf("Input Text Path: \n>> ");
        
    
    }
 
    else
    {
        printf("Wrong option\n");
        return 1;
    }

}