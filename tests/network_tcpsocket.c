/**
 * network_tcpsocket.c
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Test network TCP socket
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
  SAGE_BsdSocket *srv_socket, *cli_socket;
  LONG response;
  BOOL stop, quit;
  
  if ((srv_socket = SAGE_OpenServer(SNET_TCP_PROTOCOL, port)) != NULL) {
    stop = FALSE;
    while (!stop) {
      if ((cli_socket = SAGE_AcceptClient(srv_socket)) != NULL) {
        quit = FALSE;
        while (!quit) {
          SAGE_AppliLog("Waiting for client message");
          response = SAGE_ReceiveString(cli_socket, data_buffer, DATA_BUFFER_SIZE);
          if (response <= 0) {
            SAGE_AppliLog("Receive error or socket closed !");
            quit = TRUE;
            SAGE_CloseSocket(cli_socket);
          } else {
            SAGE_AppliLog("Received from client <%s>", data_buffer);
            if (strcmp(data_buffer, "QUIT") == 0) {
              SAGE_AppliLog("Client quit");
              quit = 1;
              SAGE_CloseSocket(cli_socket);
            } else if (strcmp(data_buffer, "STOP") == 0) {
              SAGE_AppliLog("Server stop");
              quit = TRUE;
              stop = TRUE;
              SAGE_CloseSocket(cli_socket);
            } else {
              strcpy(data_buffer, "Ack from server");
              SAGE_AppliLog("Send aknowledgment to client");
              response = SAGE_SendString(cli_socket, data_buffer);
              if (response <= 0) {
                SAGE_AppliLog("Send error or socket closed !");
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
  SAGE_BsdSocket *cli_socket;
  LONG response;
  
  if ((cli_socket = SAGE_OpenClient(SNET_TCP_PROTOCOL, host, port)) != NULL) {
    strcpy(data_buffer, message);
    SAGE_AppliLog("Send message <%s> to server", message);
    response = SAGE_SendString(cli_socket, data_buffer);
    if (response <= 0) {
      SAGE_AppliLog("Send error or socket closed !");
      SAGE_CloseSocket(cli_socket);
      return -1;
    }
    SAGE_AppliLog("Waiting for server acknowledgement");
    response = SAGE_ReceiveString(cli_socket, data_buffer, DATA_BUFFER_SIZE);
    if (response <= 0) {
      SAGE_AppliLog("Receive error or socket closed !");
      SAGE_CloseSocket(cli_socket);
      return -1;
    }
    SAGE_AppliLog("Ack <%s>", data_buffer);
    strcpy(data_buffer, "QUIT");
    SAGE_AppliLog("Send quit to server", message);
    response = SAGE_SendString(cli_socket, data_buffer);
    if (response <= 0) {
      SAGE_AppliLog("Send error or socket closed !");
      SAGE_CloseSocket(cli_socket);
      return -1;
    }
    SAGE_CloseSocket(cli_socket);
  }
  return 0;
}

void main(int argc, char **argv)
{
  char *message;

  SAGE_AppliLog("--------------------------------------------------------------------------------");
  SAGE_AppliLog("* SAGE library NETWORK test (TCPSOCKET) / %s", SAGE_GetVersion());
  SAGE_AppliLog("--------------------------------------------------------------------------------");
  if (SAGE_Init(SMOD_NETWORK)) {
    SAGE_AppliLog("Initialization successfull");
    if (argc < 2 || argc > 3) {
      SAGE_AppliLog("usage : %s SRV|CLI [message]", argv[0]);
    } else {
      if (argc == 3) {
        message = argv[2];
      } else {
        message = "Hello world !";
      }
      if (strcmp(argv[1], "SRV") == 0) {
        SAGE_AppliLog("Server return %d", manage_server(DEFAULT_PORT));
      } else if (strcmp(argv[1], "CLI") == 0) {
        SAGE_AppliLog("Client return %d", manage_client(DEFAULT_HOST, DEFAULT_PORT, message));
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
