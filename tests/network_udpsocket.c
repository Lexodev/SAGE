/**
 * network_udpsocket.c
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Test network UDP socket
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 24.2 June 2024 (updated: 27/06/2024)
 */

#include <sage/sage.h>

#define DATA_BUFFER_SIZE      4096
#define DEFAULT_HOST          "localhost"
#define DEFAULT_PORT          7000

UBYTE data_buffer[DATA_BUFFER_SIZE];

LONG manage_server(UWORD port)
{
  SAGE_BsdSocket *srv_socket;
  LONG response;
  BOOL stop;
  
  if ((srv_socket = SAGE_OpenServer(SNET_UDP_PROTOCOL, port)) != NULL) {
    stop = FALSE;
    while (!stop) {
      SAGE_AppliLog("Waiting for client message");
      response = SAGE_ReceiveString(srv_socket, data_buffer, DATA_BUFFER_SIZE);
      if (response <= 0) {
        SAGE_AppliLog("Receive error or socket closed !");
        stop = TRUE;
      } else {
        SAGE_AppliLog("Received from client <%s>", data_buffer);
        if (strncmp(data_buffer, "STOP", 4) == 0) {
          SAGE_AppliLog("Server stop");
          stop = TRUE;
        }
      }
    }
    SAGE_CloseSocket(srv_socket);
  }
  return 0;
}

LONG manage_client(STRPTR host, UWORD port, int nbmes, char **messages)
{
  SAGE_BsdSocket *cli_socket;
  LONG response, mes;
  
  if ((cli_socket = SAGE_OpenClient(SNET_UDP_PROTOCOL, host, port)) != NULL) {
    for (mes = 2;mes < nbmes;mes++) {
      strcpy(data_buffer, messages[mes]);
      SAGE_AppliLog("Send message <%s> to server", messages[mes]);
      response = SAGE_SendString(cli_socket, data_buffer);
      if (response <= 0) {
        SAGE_AppliLog("Send error or socket closed !");
        SAGE_CloseSocket(cli_socket);
        return -1;
      }
    }
    SAGE_CloseSocket(cli_socket);
  }
  return 0;
}

void main(int argc, char **argv)
{
  SAGE_AppliLog("--------------------------------------------------------------------------------");
  SAGE_AppliLog("* SAGE library NETWORK test (TCPSOCKET) / %s", SAGE_GetVersion());
  SAGE_AppliLog("--------------------------------------------------------------------------------");
  if (SAGE_Init(SMOD_NETWORK)) {
    SAGE_AppliLog("Initialization successfull");
    if (argc < 2) {
      SAGE_AppliLog("usage : %s SRV|CLI [message1] ... [messageN]", argv[0]);
    } else {
      if (strcmp(argv[1], "SRV") == 0) {
        SAGE_AppliLog("Server return %d", manage_server(DEFAULT_PORT));
      } else if (strcmp(argv[1], "CLI") == 0) {
        SAGE_AppliLog("Client return %d", manage_client(DEFAULT_HOST, DEFAULT_PORT, argc, argv));
      } else {
        SAGE_AppliLog("Bad parameter");
      }
    }
  } else {
    SAGE_AppliLog("Initialization failed");
  }
  SAGE_Exit();
  SAGE_AppliLog("End of test");
}
