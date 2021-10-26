#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <pthread.h>

#define SERVER_KEY_PATHNAME "./mqueue_server_key"
#define PROJECT_ID 'M'
#define QUEUE_PERMISSIONS 0660
#define cantidadTotalEventos 200

struct mensaje {
    long tipoMensaje;
    int qid;
    char buffer[300];
};

struct eventos {
    char nombreEvento[200];
    int cantidadClientes[200];
    int suscritosAlEvento;
};

struct eventos *eventosServidor;
int cantidadEventos;
key_t msg_queue_key;
int qid;
int personas[200];
int cantidadPersonas;

void *sub (void *pargs) 
{
    //3
    struct mensaje mensajeCliente;
    struct mensaje mensajeServidor;
    int clienteID;
    char *token;
    char *separador = " ";
    mensajeServidor.qid = qid;
    mensajeServidor.tipoMensaje = 3;
    while (1)
    {
        if (msgrcv (qid, &mensajeCliente, sizeof(struct mensaje), 3, 0) == -1) {
            perror ("msgrcv");
            exit (EXIT_FAILURE);
        }
        clienteID = mensajeCliente.qid;
        token = strtok(mensajeCliente.buffer, separador);
        for (int i = 0; i < cantidadEventos; i++)
        {
            if (strcmp(token, eventosServidor[i].nombreEvento) == 0)
            {
                eventosServidor[i].cantidadClientes[eventosServidor[i].suscritosAlEvento] = clienteID;
                eventosServidor[i].suscritosAlEvento++;
                sprintf(mensajeServidor.buffer, "%s", "suscrito");
                break;
            }
            else
            {
                sprintf(mensajeServidor.buffer, "%s", "no se encontro un evento con ese nombre");
            }
        }
        if (msgsnd (clienteID, &mensajeServidor, sizeof(struct mensaje), 0) == -1)
        {  
            perror ("msgget");
            exit (1);
        }
    }
}

void *unsub (void *pargs)
{
    //4
    struct mensaje mensajeCliente;
    struct mensaje mensajeServidor;
    int clienteID;
    char *token;
    char *separador = " ";
    mensajeServidor.qid = qid;
    mensajeServidor.tipoMensaje = 4;
    int posicionCliente = 0;
    while (1)
    {
        if (msgrcv (qid, &mensajeCliente, sizeof(struct mensaje), 4, 0) == -1) {
            perror ("msgrcv");
            exit (EXIT_FAILURE);
        }
        clienteID = mensajeCliente.qid;
        token = strtok(mensajeCliente.buffer, separador);
        for (size_t i = 0; i < cantidadEventos; i++)
        {
            if (strcmp(token, eventosServidor[i].nombreEvento) == 0)
            {
                for (size_t j = 0; j < eventosServidor[i].suscritosAlEvento; j++)
                {
                    if (clienteID == eventosServidor[i].cantidadClientes[j])
                    {
                        posicionCliente = j;
                    }
                }
                for (size_t j = posicionCliente; j < eventosServidor[i].suscritosAlEvento; j++)
                {
                    eventosServidor[i].cantidadClientes[j] = eventosServidor[i].cantidadClientes[j + 1];
                }
                eventosServidor[i].suscritosAlEvento--;
                break;
            }
            else
            {
                sprintf(mensajeServidor.buffer, "%s", "no se encontro un evento con ese nombre");
            }
        }
        if (msgsnd (clienteID, &mensajeServidor, sizeof(struct mensaje), 0) == -1)
        {  
            perror ("msgget");
            exit (1);
        }
    }
}

void *ask (void *pargs)
{
    //5
    struct mensaje mensajeCliente;
    struct mensaje mensajeServidor;
    mensajeServidor.qid = qid;
    mensajeServidor.tipoMensaje = 5;
    int clienteID;
    while (1)
    {
        if (msgrcv (qid, &mensajeCliente, sizeof(struct mensaje), 5, 0) == -1) {
            perror ("msgrcv");
            exit (EXIT_FAILURE);
        }
        clienteID = mensajeCliente.qid;
        for (size_t i = 0; i < cantidadEventos; i++)
        {
            sprintf(mensajeServidor.buffer, "%s", eventosServidor[i].nombreEvento);
            if (msgsnd (clienteID, &mensajeServidor, sizeof(struct mensaje), 0) == -1)
            {  
                perror ("msgget");
                exit (1);
            }
        }
    }
}

void *list (void *pargs)
{
    //6
    struct mensaje mensajeCliente;
    struct mensaje mensajeServidor;
    mensajeServidor.qid = qid;
    mensajeServidor.tipoMensaje = 6;
    int clienteID;
    while (1)
    {
        if (msgrcv (qid, &mensajeCliente, sizeof(struct mensaje), 6, 0) == -1) {
            perror ("msgrcv");
            exit (EXIT_FAILURE);
        }
        clienteID = mensajeCliente.qid;
        for (size_t i = 0; i < cantidadEventos; i++)
        {
            for (size_t j = 0; j < eventosServidor[i].suscritosAlEvento; j++)
            {
                if (clienteID == eventosServidor[i].cantidadClientes[j])
                {
                    sprintf(mensajeServidor.buffer, "%s", eventosServidor[i].nombreEvento);
                    if (msgsnd (clienteID, &mensajeServidor, sizeof(struct mensaje), 0) == -1) {  
                        perror ("msgget");
                        exit (1);
                    }
                    break;
                }
            }
        }
    }
}

void *conectar (void *pargs)
{
    //7
    struct mensaje mensajeCliente;
    struct mensaje mensajeServidor;
    mensajeServidor.qid = qid;
    mensajeServidor.tipoMensaje = 7;
    int clienteID;
    cantidadPersonas = 0;
    while (1)
    {
        if (msgrcv (qid, &mensajeCliente, sizeof(struct mensaje), 7, 0) == -1) {
            perror ("msgrcv");
            exit (EXIT_FAILURE);
        }

        clienteID = mensajeCliente.qid;
        personas[cantidadPersonas] = clienteID;
        cantidadPersonas++;
        printf("Nuevo cliente\n");
        sprintf(mensajeServidor.buffer, "%s", "Bienvenido");

        if (msgsnd (clienteID, &mensajeServidor, sizeof(struct mensaje), 0) == -1) {  
            perror ("msgget");
            exit (1);
        }
    }
}

int main (int argc, char **argv)
{
    FILE *mqueue_server_key = fopen(SERVER_KEY_PATHNAME,"w+");
    if (mqueue_server_key == NULL){
        perror("open file fails: ");
        return(EXIT_FAILURE);
    }

    struct mensaje mensajeServidor;
    pthread_t hiloSub;
    pthread_t hiloUnsub;
    pthread_t hiloList;
    pthread_t hiloAsk;
    pthread_t hiloConectar;
    pthread_create(&hiloSub, NULL, sub, NULL);
    pthread_create(&hiloUnsub, NULL, unsub, NULL);
    pthread_create(&hiloList, NULL, list, NULL);
    pthread_create(&hiloAsk, NULL, ask, NULL);
    pthread_create(&hiloConectar, NULL, conectar, NULL);
    char *token;
    char terminal_input[200];
    char *separador = " ";
    char *cerrar = "exit";
    char *add = "add";
    char *remove = "remove";
    char *trigger = "trigger";
    char *list = "list";

    if ((msg_queue_key = ftok (SERVER_KEY_PATHNAME, PROJECT_ID)) == -1)
    {
        perror ("ftok");
        exit (1);
    }

    if ((qid = msgget (msg_queue_key, IPC_CREAT | QUEUE_PERMISSIONS)) == -1)
    {
        perror ("msgget");
        exit (1);
    }

    printf("INICIANDO SERVIDOR\n");
    cantidadEventos = 0;
    eventosServidor = malloc(sizeof(struct eventos) * cantidadTotalEventos);
    while (fgets (terminal_input, 200, stdin))
    {
        int length = strlen (terminal_input);
        if (terminal_input [length - 1] == '\n')
        {
            terminal_input [length - 1] = '\0';
        }
        token = strtok(terminal_input, separador);
        // exit tipoMensaje = 1
        if (strcmp(token, cerrar) == 0)
        {
            mensajeServidor.qid = qid;
            mensajeServidor.tipoMensaje = 1;
            sprintf(mensajeServidor.buffer, "%s", "Cerrando el servidor");
            for (size_t i = 0; i < cantidadPersonas; i++)
            {
                if (msgsnd (personas[i], &mensajeServidor, sizeof(struct mensaje), 0) == -1) {  
                    perror ("msgget");
                    exit (EXIT_FAILURE);
                }
            }
            free(eventosServidor);
            if (msgctl (qid, IPC_RMID, NULL) == -1) {
                perror ("server: msgctl");
                exit (EXIT_FAILURE);
            }
        }
        // add
        if (strcmp(token, add) == 0)
        {
            token = strtok(NULL, separador);
            strcpy(eventosServidor[cantidadEventos].nombreEvento, token);
            eventosServidor[cantidadEventos].suscritosAlEvento = 0;
            cantidadEventos++;
            printf("Se agrego un evento\n");
        }
        // remove
        if (strcmp(token, remove) == 0)
        {
            token = strtok(NULL, separador);
            int posicionEvento;
            for (size_t i = 0; i < cantidadEventos; i++)
            {
                if (strcmp(token, eventosServidor[i].nombreEvento) == 0)
                {
                    posicionEvento = i;
                }
            }
            for (int i = posicionEvento; i < cantidadEventos; i++)
            {
                eventosServidor[i] = eventosServidor[i + 1];
            }
            cantidadEventos--;
            printf("Se removio un evento\n");
        }
        // trigger = 2
        if(strcmp(token, trigger) == 0)
        {
            token = strtok(NULL, separador);
            mensajeServidor.qid = qid;
            mensajeServidor.tipoMensaje = 2;
            sprintf(mensajeServidor.buffer, "Evento %s iniciado.", token);
            for (int i = 0; i < cantidadEventos; i++)
            {
                if (strcmp(token, eventosServidor[i].nombreEvento) == 0)
                {
                    for (int j = 0; j < eventosServidor[i].suscritosAlEvento; j++)
                    {
                        if (msgsnd (eventosServidor[i].cantidadClientes[j], &mensajeServidor, sizeof(struct mensaje), 0) == -1) {
                            perror ("msgget");
                        }
                    }
                    break;
                }
            }
            printf("Trigger enviado\n");
        }
        // list
        if(strcmp(token, list) == 0)
        {
            token = strtok(NULL, separador);
            for (size_t i = 0; i < cantidadEventos; i++)
            {
                if (strcmp(token, eventosServidor[i].nombreEvento) == 0)
                {
                    printf("Suscritos al evento %s:\n", eventosServidor[i].nombreEvento);
                    for (size_t j = 0; j < eventosServidor[i].suscritosAlEvento; j++)
                    {
                        printf("%d\n", eventosServidor[i].cantidadClientes[j]);
                    }
                }
            }
        }
    }
}