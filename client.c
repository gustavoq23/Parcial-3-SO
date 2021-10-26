#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include <pthread.h>

#define SERVER_KEY_PATHNAME "./mqueue_server_key"
#define PROJECT_ID 'M'

struct mensaje {
    long tipoMensaje;
    int qid;
    char buffer[300];
};

key_t server_queue_key;
int server_qid;
int myqid;

void *escucharsub(void *pargs)
{
    struct mensaje mensajeServidor;
    while (1) {
        if (msgrcv (myqid, &mensajeServidor, sizeof(struct mensaje), 3, 0) == -1) {
            perror ("msgrcv\n");
            exit (EXIT_FAILURE);
        }
        printf("%s\n", mensajeServidor.buffer);
    }
}

void *escucharunsub(void *pargs)
{
    struct mensaje mensajeServidor;
    while (1) {
        if (msgrcv (myqid, &mensajeServidor, sizeof(struct mensaje), 4, 0) == -1) {
            perror ("msgrcv\n");
            exit (EXIT_FAILURE);
        }
        printf("%s\n", mensajeServidor.buffer);
    }
}

void *escucharlist (void *pargs)
{
    struct mensaje mensajeServidor;
    while (1) {
        if (msgrcv (myqid, &mensajeServidor, sizeof(struct mensaje), 6, 0) == -1) {
            perror ("msgrcv\n");
            exit (EXIT_FAILURE);
        }
        printf("%s\n", mensajeServidor.buffer);
    }
}

void *escucharask (void *pargs)
{
    struct mensaje mensajeServidor;
    while (1) {
        if (msgrcv (myqid, &mensajeServidor, sizeof(struct mensaje), 5, 0) == -1) {
            perror ("msgrcv\n");
            exit (EXIT_FAILURE);
        }
        printf("%s\n", mensajeServidor.buffer);
    }
}

void *escuchartrigger (void *pargs)
{
    struct mensaje mensajeServidor;
    while (1) {
        if (msgrcv (myqid, &mensajeServidor, sizeof(struct mensaje), 2, 0) == -1) {
            perror ("msgrcv\n");
            exit (EXIT_FAILURE);
        }
        printf("%s\n", mensajeServidor.buffer);
    }
}

void *escucharexit (void *pargs)
{
struct mensaje mensajeServidor;
    while (1) {
        if (msgrcv (myqid, &mensajeServidor, sizeof(struct mensaje), 1, 0) == -1) {
            perror ("msgrcv\n");
            exit (EXIT_FAILURE);
        }
        printf("cerrando\n");
        exit(EXIT_SUCCESS);
    }
}

int main (int argc, char **argv)
{
    struct mensaje mensajeCliente;
    struct mensaje mensajeServidor;
    char bufferInput[200];
    char *token;
    char *separador = " ";
    char *sub = "sub";
    char *unsub = "unsub";
    char *ask = "ask";
    char *cerrar = "exit";
    char *list = "list";
    pthread_t hilosub;
    pthread_t hilounsub;
    pthread_t hilolist;
    pthread_t hiloask;
    pthread_t hilotrigger;
    pthread_t hiloexit;
    pthread_create(&hiloexit, NULL, escucharexit, NULL);
    pthread_create(&hilotrigger, NULL, escuchartrigger, NULL);
    pthread_create(&hiloask, NULL, escucharask, NULL);
    pthread_create(&hilolist, NULL, escucharlist, NULL);
    pthread_create(&hilounsub, NULL, escucharunsub, NULL);
    pthread_create(&hilosub, NULL, escucharsub, NULL);

    if ((myqid = msgget (IPC_PRIVATE, 0660)) == -1) {
        perror ("msgget: myqid");
        exit (1);
    }

    if ((server_queue_key = ftok (SERVER_KEY_PATHNAME, PROJECT_ID)) == -1) {
        perror ("ftok");
        exit (1);
    }

    if ((server_qid = msgget (server_queue_key, 0)) == -1) {
        perror ("msgget: server_qid");
        exit (1);
    }
    mensajeCliente.qid = myqid;
    mensajeCliente.tipoMensaje = 7;
    sprintf(mensajeCliente.buffer, "%s", "hola");
    if (msgsnd (server_qid, &mensajeCliente, sizeof(struct mensaje), 0) == -1) {  
        perror ("msgget");
        exit (1);
    }
    if (msgrcv (myqid, &mensajeServidor, sizeof(struct mensaje), 7, 0) == -1) {
        perror ("msgrcv");
        exit (EXIT_FAILURE);
    }
    printf("%s\n\n", mensajeServidor.buffer);
    printf("Soy el cliente: %d\n", myqid);
    while (fgets (bufferInput, 200, stdin))
    {
        int length = strlen(bufferInput);
        if (bufferInput[length - 1] == '\n')
        {
            bufferInput[length - 1] = '\0';
        }
        token = strtok(bufferInput, separador);
        
        if (strcmp(token, sub) == 0)
        {
            mensajeCliente.tipoMensaje = 3;
        }
        if (strcmp(token, unsub) == 0)
        {
            mensajeCliente.tipoMensaje = 4;
        }
        if (strcmp(token, ask) == 0)
        {
            mensajeCliente.tipoMensaje = 5;
        }
        if (strcmp(token, cerrar) == 0)
        {
            printf("cerrando\n");
            exit(EXIT_SUCCESS);
        }
        if (strcmp(token, list) == 0)
        {
            mensajeCliente.tipoMensaje = 6;
        }
        token = strtok(NULL, separador);
        sprintf(mensajeCliente.buffer, "%s", token);
        if (msgsnd (server_qid, &mensajeCliente, sizeof(struct mensaje), 0) == -1) {
            perror ("client: msgsnd");
            exit (EXIT_FAILURE);
        }
    }
    if (msgctl (myqid, IPC_RMID, NULL) == -1) {
        perror ("client: msgctl");
        exit (1);
    }
}