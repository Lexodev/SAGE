/**
 * network_udpsocket.c
 * 
 * SAGE (Small Amiga Game Engine) project
 * Test network UDP socket
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
  SAGE_BsdSocket * srv_socket;
  LONG response;
  BOOL stop;
  
  if ((srv_socket = SAGE_OpenServer(SNET_UDP_PROTOCOL, port)) != NULL) {
    stop = FALSE;
    while (!stop) {
      printf("Waiting for client message\n");
      response = SAGE_ReceiveString(srv_socket, data_buffer, DATA_BUFFER_SIZE);
      if (response <= 0) {
        printf("Receive error or socket closed !\n");
        stop = TRUE;
      } else {
        printf("Received from client <%s>\n", data_buffer);
        if (strncmp(data_buffer, "STOP", 4) == 0) {
          printf("Server stop\n");
          stop = TRUE;
        }
      }
    }
    SAGE_CloseSocket(srv_socket);
  }
  return 0;
}

LONG manage_client(STRPTR host, UWORD port, int nbmes, char ** messages)
{
  SAGE_BsdSocket * cli_socket;
  LONG response, mes;
  
  if ((cli_socket = SAGE_OpenClient(SNET_UDP_PROTOCOL, host, port)) != NULL) {
    for (mes = 2;mes < nbmes;mes++) {
      strcpy(data_buffer, messages[mes]);
      printf("Send message <%s> to server\n", messages[mes]);
      response = SAGE_SendString(cli_socket, data_buffer);
      if (response <= 0) {
        printf("Send error or socket closed !\n");
        SAGE_CloseSocket(cli_socket);
        return -1;
      }
    }
    SAGE_CloseSocket(cli_socket);
  }
  return 0;
}

void main(int argc, char ** argv)
{
  printf("--------------------------------------------------------------------------------\n");
  printf("* SAGE library NETWORK test (TCPSOCKET) / %s\n", SAGE_GetVersion());
  printf("--------------------------------------------------------------------------------\n");
  if (SAGE_Init(SMOD_NETWORK)) {
    printf("Initialization successfull\n");
    if (argc < 2) {
      printf("usage : %s SRV|CLI [message1] ... [messageN]\n", argv[0]);
    } else {
      if (strcmp(argv[1], "SRV") == 0) {
        printf("Server return %d\n", manage_server(DEFAULT_PORT));
      } else if (strcmp(argv[1], "CLI") == 0) {
        printf("Client return %d\n", manage_client(DEFAULT_HOST, DEFAULT_PORT, argc, argv));
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
