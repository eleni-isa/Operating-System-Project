#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <time.h>
#include <errno.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/socket.h>
#include <semaphore.h>
#include <sys/un.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>


//key for the shared memory
#define SHM_KEY "4800"
//share memory size
#define SHM_SIZE 1024
//semaphore name
#define SEM_NAME "87955"
// SOCKET_NAME
#define SOCKET_NAME "44153"
//time of service
#define TIME_SPECIAL 0.15
#define TIME_MARGARITA 0.1
#define TIME_PEPPERONI 0.12
#define TIME_VERYLONG 0.5
#define TIME_MAKRIA 0.01
#define TIME_KONTA 0.005


//structure with the order data
struct Pelates {

    char apostasi[6]; //matrix with the distance of the client's house .
    int Nmax;          //number of pizzas
    int coke;           //variable with the Coke - 0 or 1
    char pizza[3][9]; // type of pizza
                        //each line of the 3 is the type of pizza - every client can have up to 3 pizzas
}Pel1;

//shared memory
struct Data
{
    struct Pelates Pel[100], Pel1;
}*data;//shared memory pointer

//**********************************************************************ΔΗΛΩΣΗ ΣΥΝΑΡΤΗΣΕΩΝ************************************************************************
//function for Ctrl+C
void quitprocess(void);

//chefs
int arxikopoiisiPsistes(int psistes[], int len_psi);

//deliver men
int arxikopoiisiDianomeis(int dianomeis[], int len_dian);

//check if there is available chef
int elegxosPsistes (int psistes[], int len_psi);

//check if there is available deliver
int elegxosDianomeis (int dianomeis[], int len_dian);

//occupy first available chef
int apasxolhshPsistes (int psistes[], int len_psi);

//occupy first available deliver man
int apasxolhshDianomeis (int dianomeis[], int len_dian);

//free the chef
int eleutherwshPsistes (int psistes[], int len_psi);

//free the deliver
int eleutherwshDianomeis (int dianomeis[], int len_dian);

//time for cooking
void pshsimo (char pizza[], int len_piz);

//time for delivering
void dianomh (char apostasi[], int len_apo);

 //variable for shared memory
int shm_id;


//*****************************************************************************MAIN******************************************************************************

int main (int argc, char *argv[])
{

    //socket name
    int sd;
    //socket for the "child"
    int newsd;
    //variable for socket and connect
    int addrlen, connlen;
    //structure for the socket
    struct sockaddr address;
    struct sockaddr client_addr;

    //variable for semaphores
    sem_t *my_sem;
    //
    int psistes[10];
    int dianomeis[10];
    int i, j, p, d, SUM, coke, k;


    //signal for ending the server
    signal(SIGINT, quitprocess);
    //signal for zombie threads
    signal(SIGCHLD, SIG_IGN);

    printf("For quiting press CTRL+C.");


    //create new socket  socket
    sd =socket(AF_UNIX, SOCK_STREAM, 0);
    //unlink socket
    unlink(SOCKET_NAME);


    //initilize socket
    bzero((char *) &address, sizeof(address));
    //name of socket in AF_UNIX
    address.sa_family=AF_UNIX;
    strcpy(address.sa_data, SOCKET_NAME);
    addrlen=sizeof(address.sa_data)+sizeof(address.sa_family);
    bzero((char *) & client_addr, sizeof(client_addr));
    connlen=sizeof(client_addr);



    //occupy the name of socket: bind and Listen
    if (bind(sd, &address, addrlen) <0)
    {
        printf("Error the socket and the server: %d\n", errno);
        perror("server:");
        exit(1);
    }
    if(listen(sd,3)<0)
    {
        printf("Error the socket and the server: %d\n", errno);
        perror("server:");
        exit(1);
    }


    //create shared memory
    if((shm_id = shmget(SHM_KEY, SHM_SIZE, 0600 | IPC_CREAT)) <0 )
    {
        printf("Error on creating the shared memory. \n");
        goto EXIT;
    }
    //shared memory structure
    data=(struct Data *)shmat(shm_id,0,0);

    //initilize shared memory
    for(i=0;i<100;i++)
    {
        for(j=0;j<3;j++)
        {
            strcpy(data->Pel[i].pizza[j],"");
        }
        strcpy(data->Pel[i].apostasi,"");
        data->Pel[j].Nmax=0;
        data->Pel[j].coke=0;
    }

    //create semaphore
    my_sem = sem_open(SEM_NAME, O_CREAT | O_RDWR, S_IRUSR |S_IWUSR, 1);
    if(my_sem == SEM_FAILED)
    {
        printf("Semaphore cannot open. \n");
        exit(1);
    }



    //create socket which communicate the child with the thread
    if((newsd = accept(sd,&client_addr,(socklen_t*)&connlen))<0)
    {
        printf("Error on accepting the server: %d\n", errno);
        perror("server:");
        goto EXIT;
    }



        int pid;
        pid = fork();
        printf("klh8hke h fork\n");
        if (pid == 0)
        { //code only executing from the child
            close(sd);
            if(read(newsd, &Pel1, sizeof(Pel1)) < 0)
            {
                printf("Read Error\n" );
                goto EXIT;
            }

            printf("data transfered\n");

            for (j=0;j<100;j++)
            {
                if(data->Pel[j].Nmax==0)
                {
                    sem_wait(my_sem);
                    data->Pel[j]=Pel1;
                    sem_post(my_sem);
                    break;
                }
            }//end for

            //after we have taken the data from the client, every available chef takes an order
            for (i=0;i<Pel1.Nmax;i++)
            {
                printf("Preparing pizza\n");
                SUM=0;

                label1:
                //check for available chefs
                p = elegxosPsistes (psistes,10);
                printf("Complete check \n");
                //if there is available chef
                if (p==1)
                {
                    printf("Available chef\n");
                    sem_wait(my_sem);
                    printf("Lock shared memory\n");
                    apasxolhshPsistes(psistes,10);
                    printf("Declare occupied chef\n");
                    pshsimo(Pel1.pizza[i], 9);
                    eleutherwshPsistes(psistes,10);
                    sem_post(my_sem);
                    printf("Free shared memory \n");
                }
                else
                {
                    printf("There is no available chef. Please wait!\n");
                    goto label1;
                }
                // different kinds of pizza
                if (strcmp(Pel1.pizza[i],"special"))
                    SUM=SUM+TIME_SPECIAL;
                else if (strcmp(Pel1.pizza[i],"margarita"))
                    SUM=SUM+TIME_MARGARITA;
                else
                    SUM=SUM+TIME_PEPPERONI;

                //different distance
                if (strcmp(Pel1.apostasi,"makria"))
                    SUM=SUM+TIME_MAKRIA;
                else
                    SUM=SUM+TIME_KONTA;

                if (SUM>TIME_VERYLONG)
                {
                    data->Pel[j].coke=1;
                    write(newsd, &data->Pel[j], sizeof(data->Pel[j]));
                }
                printf("End of preparing the pizza\n");

                label2:
                // check for available deliver
                d = elegxosDianomeis (dianomeis,10);
                printf("Check completed. \n");
                if (d==1)
                {
                    printf("Available deliver\n");
                    sem_wait(my_sem);
                    printf("Lock the shared memory\n");
                    apasxolhshDianomeis(dianomeis,10);
                    printf("Occupy the deliver");
                    dianomh(Pel1.apostasi, 6);
                    eleutherwshDianomeis(dianomeis, 10);
                    //free the shared memory
                    sem_post(my_sem);
                    printf("Free the shared memory\n");
                    printf("PIZZA IS HERE!!!!\n");
                }
                else
                {
                    printf("All the deliver is busy. Please wait!\n");

                    goto label2;
                }//end if
            }//end for
        }//end if



    	EXIT:
        close(sd);
        close(newsd);
        unlink(SOCKET_NAME);
        shmctl(shm_id,IPC_RMID,0);
        exit (0);


    return 0; //never here

}

//**********************************************************************DECLARE FUNCTION**********************************************************************

void quitprocess()
{
    printf("Press CTRL+C.\n");
    //unlink socket
    unlink(SOCKET_NAME);
    //delete semaphore
    sem_unlink(SEM_NAME);
    exit(0);
}


int arxikopoiisiPsistes(int psistes[], int len_psi)
{
    int i;
    for (i=0;i<10;i++)
    {
        //if available write 0
        psistes[i]=0;
    }
    return psistes;
}

int arxikopoiisiDianomeis(int dianomeis[], int len_dian)
{
    int i;
    for(i=0;i<10;i++)
    {
        //if available write 0
        dianomeis[i]=0;
    }
    return dianomeis;
}

int elegxosPsistes (int psistes[], int len_psi)
{

    int flag_psi=0;
    int i=0;
    while (i<len_psi && flag_psi==0)
    {

        if (psistes[i]==0)
        {
            flag_psi=1;
        }
        i++;
    }
    return flag_psi;
}

int elegxosDianomeis(int dianomeis[], int len_dian)
{

    int flag_dian=0;
    int i=0;
    while (i<len_dian && flag_dian==0)
    {

        if (dianomeis[i]==0)
        {
            flag_dian=1;
        }
        i++;
    }
    return flag_dian;
}

int apasxolhshPsistes (int psistes[], int len_psi)
{
    int i=0;

    int e=0;
    while (i<len_psi && e==0)
    {
        //first available chef
        if (psistes[i]==0)
        {
            //declare occupied
            psistes[i]=1;
            e=1;
        }
        i++;
    }
    return psistes;
}

int apasxolhshDianomeis (int dianomeis[], int len_dian)
{
    int i=0;
    int e=0;
    while(i<len_dian && e==0)
    {
        if (dianomeis[i]==0)
        {
            dianomeis[i]=1;
            e=1;
        }
        i++;
    }
    return dianomeis;
}

int eleutherwshPsistes (int psistes[], int len_psi)
{
    int i=0;
    int e=0;
    while (i<len_psi && e==0)
    {
        if (psistes[i]==1)
        {
            psistes[i]=0;
            e=1;
        }
        i++;
    }
    return psistes;
}

int eleutherwshDianomeis (int dianomeis[], int len_dian)
{
    int i=0;
    int e=0;
    while (i<len_dian && e==0)
    {
        if (dianomeis[i]==1)
        {
            //declare free
            dianomeis[i]=0;
            e=1;
        }
        i++;
    }
    return dianomeis;
}

void pshsimo (char pizza[], int len_piz)
{
    if (strcmp(pizza, "special"))
    {
        sleep(0.15);
    }
    else if (strcmp(pizza,"margarita"))
    {
        sleep(0.1);
    }
    else
    {
        sleep(0.12);
    }
}

void dianomh (char apostasi[], int len_apo)
{
    if (strcmp(apostasi, "makria"))
    {
        sleep(0.01);
    }
    else
    {
        sleep(0.005);
    }
}
