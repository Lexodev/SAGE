/**
 * network_tcpsocket.c
 * 
 * SAGE (Small Amiga Game Engine) project
 * Test network TCP socket
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

LONG manage_server(UWORD port)
{
  SAGE_BsdSocket * srv_socket, * cli_socket;
  LONG response;
  BOOL stop, quit;
  
  if ((srv_socket = SAGE_OpenServer(SNET_TCP_PROTOCOL, port)) != NULL) {
    stop = FALSE;
    while (!stop) {
      if ((cli_socket = SAGE_AcceptClient(srv_socket)) != NULL) {
        quit = FALSE;
        while (!quit) {
          printf("Waiting for client message\n");
          response = SAGE_ReceiveString(cli_socket, data_buffer, DATA_BUFFER_SIZE);
          if (response <= 0) {
            printf("Receive error or socket closed !\n");
            quit = TRUE;
            SAGE_CloseSocket(cli_socket);
          } else {
            printf("Received from client <%s>\n", data_buffer);
            if (strcmp(data_buffer, "QUIT") == 0) {
              printf("Client quit\n");
              quit = 1;
              SAGE_CloseSocket(cli_socket);
            } else if (strcmp(data_buffer, "STOP") == 0) {
              printf("Server stop\n");
              quit = TRUE;
              stop = TRUE;
              SAGE_CloseSocket(cli_socket);
            } else {
              strcpy(data_buffer, "Ack from server");
              printf("Send aknowledgment to client\n");
              response = SAGE_SendString(cli_socket, data_buffer);
              if (response <= 0) {
                printf("Send error or socket closed !\n");
                quit = TRUE;
                SAGE_CloseSocket(cli_socket);
              }
            }
          }
        }
      } else {
        stop = TRUE;
      }
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
