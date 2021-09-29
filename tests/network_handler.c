/**
 * network_handler.c
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Test network TCP socket handlers
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 1.0 December 2020
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "/src/sage.h"

#define DATA_BUFFER_SIZE      4096
#define DEFAULT_HOST          "localhost"
#define DEFAULT_PORT          7000

UBYTE data_buffer[DATA_BUFFER_SIZE];

BOOL server_stop;

VOID ReadFromClient(SAGE_BsdSocket * bsdsocket, APTR data)
{
  LONG response;

  response = SAGE_ReceiveString(bsdsocket, data_buffer, DATA_BUFFER_SIZE);
  if (response <= 0) {
    printf("Receive error or socket closed !\n");
    SAGE_CloseSocket(bsdsocket);
  } else {
    printf("Received from client <%s>\n", data_buffer);
    if (strcmp(data_buffer, "QUIT") == 0) {
      printf("Client quit\n");
      SAGE_CloseSocket(bsdsocket);
    } else if (strcmp(data_buffer, "STOP") == 0) {
      printf("Server stop\n");
      server_stop = TRUE;
      SAGE_CloseSocket(bsdsocket);
    } else {
      strcpy(data_buffer, "Ack from server");
      printf("Send aknowledgment to client\n");
      response = SAGE_SendString(bsdsocket, data_buffer);
      if (response <= 0) {
        printf("Send error or socket closed !\n");
        SAGE_CloseSocket(bsdsocket);
      }
    }
  }
}

VOID AcceptNewClient(SAGE_BsdSocket * bsdsocket, APTR data)
{
  SAGE_BsdSocket * cli_socket;
  
  if ((cli_socket = SAGE_AcceptClient(bsdsocket)) != NULL) {
    printf("Accept a new client #%d\n", cli_socket->socket_id);
    if (SAGE_AddSocketReadHandler(cli_socket, ReadFromClient, NULL)) {
      printf("Socket read handler added\n");
      return;
    }
  }
  SAGE_DisplayError();
}

VOID ReadFromServer(SAGE_BsdSocket * bsdsocket, APTR data)
{
}

LONG manage_server(UWORD port)
{
  SAGE_BsdSocket * srv_socket;
  
  if ((srv_socket = SAGE_OpenServer(SNET_TCP_PROTOCOL, port)) != NULL) {
    printf("Server socket ready #%d\n", srv_socket->socket_id);
    if (SAGE_AddSocketListenHandler(srv_socket, AcceptNewClient, NULL)) {
      printf("Socket listen handler added\n");
      server_stop = FALSE;
      while (!server_stop) {
        printf("Running event handler\n");
        if (!SAGE_HandleSocketEvents()) {
          printf("Error while handling events\n");
          server_stop = TRUE;
        }
      }
    } else {
      SAGE_DisplayError();
    }
    SAGE_CloseSocket(srv_socket);
  }
  return 0;
}

LONG manage_client(STRPTR host, UWORD port, STRPTR message)
{
  SAGE_BsdSocket * cli_socket;
  LONG response;
  
  if ((cli_socket = SAGE_OpenClient(SNET_TCP_PROTOCOL, host, port)) != NULL) {
    strcpy(data_buffer, message);
    printf("Send message <%s> to server\n", message);
    response = SAGE_SendString(cli_socket, data_buffer);
    if (response <= 0) {
      printf("Send error or socket closed !\n");
      SAGE_CloseSocket(cli_socket);
      return -1;
    }
    printf("Waiting for server acknowledgement\n");
    response = SAGE_ReceiveString(cli_socket, data_buffer, DATA_BUFFER_SIZE);
    if (response <= 0) {
      printf("Receive error or socket closed !\n");
      SAGE_CloseSocket(cli_socket);
      return -1;
    }
    printf("Ack <%s>\n", data_buffer);
    printf("Small pause\n");
    SAGE_Pause(50*3);
    strcpy(data_buffer, "QUIT");
    printf("Send quit to server\n", message);
    response = SAGE_SendString(cli_socket, data_buffer);
    if (response <= 0) {
      printf("Send error or socket closed !\n");
      SAGE_CloseSocket(cli_socket);
      return -1;
    }
    SAGE_CloseSocket(cli_socket);
  }
  return 0;
}

void main(int argc, char ** argv)
{
  char * message;

  printf("--------------------------------------------------------------------------------\n");
  printf("* SAGE library NETWORK test (TCPSOCKET) / %s\n", SAGE_GetVersion());
  printf("--------------------------------------------------------------------------------\n");
  if (SAGE_Init(SMOD_NETWORK)) {
    printf("Initialization successfull\n");
    if (argc < 2 || argc > 3) {
      printf("usage : %s SRV|CLI [message]\n", argv[0]);
    } else {
      if (argc == 3) {
        message = argv[2];
      } else {
        message = "Hello world !";
      }
      if (strcmp(argv[1], "SRV") == 0) {
        printf("Server return %d\n", manage_server(DEFAULT_PORT));
      } else if (strcmp(argv[1], "CLI") == 0) {
        printf("Client return %d\n", manage_client(DEFAULT_HOST, DEFAULT_PORT, message));
      } else {
        printf("Bad parameter\n");
      }
    }
  } else {
    printf("Initialization failed\n");
  }
  SAGE_Exit();
  printf("End of test\n");
}
