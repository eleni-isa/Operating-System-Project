#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/stat.h>

//socket name
#define SOCKET_NAME "44153"

//structure with the order details
struct Pelates {

    char apostasi[6];
    int Nmax;
    int coke;
    char pizza[3][9];
}Pel[100], Pel1;



//*************************************************************DECLARE FUNCTIONS*************************************************************
void printMenu();

//create new order
int dimiourgiaPelates();

//initilize
void arxikopoiisi();


//*******************************************************************MAIN***********************************************************************

int main (int arg, char *argv[])
{
    int sd;
    int addrlen;
    struct sockaddr address;
    int i, u, k;

    //***************************************************CREATE AND LINK SOCKET******************************************************
    //create new socket
    sd=socket(AF_UNIX, SOCK_STREAM, 0);
    //init
    bzero(&address, sizeof(address));
    //type of socket as AF_UNIX
    address.sa_family = AF_UNIX;
    //file name to socket
    strcpy(address.sa_data,SOCKET_NAME);
    addrlen=sizeof(address.sa_data)+sizeof(address.sa_family);
    //connect with server
    if((connect(sd, &address, addrlen)) < 0)
    {
        printf("Error on connecting to client: %d\n", errno);
        perror("Client: ");
        exit(1);
    }
    //*******************************************************************************************************************************************

    //init clients
    arxikopoiisi();

    //check if we want menu
    if (arg==1)
    {
        //create new order
        k=dimiourgiaPelates();
    }
    else
    {
        //change a char to an int with atoi()
        Pel[k].Nmax = atoi (argv[1]);
        for (i=0;i<argv[1];i++)
        {
            u=2+i;
            if (argv[u]=="special")
            {
                strcpy(Pel[k].pizza[i], "special");
            }
            else if (argv[u]=="margarita")
            {
                strcpy(Pel[k].pizza[i], "margarita");
            }
            else
            {
                strcpy(Pel[k].pizza[i], "pepperoni");
            }
        }
        u++;
        strcpy(Pel[k].apostasi, argv[u]);
    }

    //send the data of each client to server
    if (write(sd, &Pel[k], sizeof(Pel[k])) <0)
    {
        printf("Error in sending data to server!\n");
        exit(0);
    }

    //take from the server if the client will have a coca cola
    read(sd, &Pel[k], sizeof(Pel[k]));
    if(Pel[k].coke==1)
    {
        printf("Sorry for the delay! You will have a free coca cola!");
    }
}//end main

//**********************************************************FUNCTIONS*******************************************************************

void printMenu()
{
    printf("1) special\n2) margarita\n3) pepperoni\n");
}

int dimiourgiaPelates()
{
    int k, i;
    for(k=0;k<100;k++)
    {
        if (Pel[k].Nmax==0)
        {
            printf("How many pizzas do you want?\n");
            do
            {
                scanf("%d", &Pel[k].Nmax);
                if(Pel[k].Nmax<0 || Pel[k].Nmax>3)
                {
                    printf("Error. You can order up to 3 pizzas!\n");
                }
            } while (Pel[k].Nmax<0 || Pel[k].Nmax>3);

            for (i=0;i<Pel[k].Nmax;i++)
            {
                printMenu();

                do
                {
                    scanf("%s", Pel[k].pizza[i]);

                    if(strcmp(Pel[k].pizza[i], "special") && strcmp(Pel[k].pizza[i], "margarita") && strcmp(Pel[k].pizza[i], "pepperoni"))
                    {
                        printf("Error! Try again. \n");
                    }
                } while (strcmp(Pel[k].pizza[i], "special") && strcmp(Pel[k].pizza[i], "margarita") && strcmp(Pel[k].pizza[i], "pepperoni") );
            }//end for


            printf("Are you far/close? makria/konta.\n");
            do
            {
                scanf("%s", Pel[k].apostasi);
                if(strcmp(Pel[k].apostasi, "makria") && strcmp(Pel[k].apostasi, "konta"))
                {
                    printf("Error! Try again. \n");
                }
            } while ( strcmp(Pel[k].apostasi, "makria") && strcmp(Pel[k].apostasi, "konta") );
            break;
        }//end if
    }//end for
    printf("Order ok!\n");
    return k;
}//end function


void arxikopoiisi()
{
    int i, k;
    for (k=0;k<100;k++)
    {
        Pel[k].coke=0;

        strcpy(Pel[k].apostasi,"");

        for(i=0;i<3;i++)
        {
             strcpy(Pel[k].pizza[i],"");
        }//end for
    }//end for
}
