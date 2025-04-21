/**
 * network_handler.c
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Test network TCP socket handlers
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 25.1 February 2025 (updated: 24/02/2025)
 */

#include <sage/sage.h>

#define DATA_BUFFER_SIZE      4096
#define DEFAULT_HOST          "localhost"
#define DEFAULT_PORT          7000

UBYTE data_buffer[DATA_BUFFER_SIZE];

BOOL server_stop;

VOID ReadFromClient(SAGE_BsdSocket *bsdsocket, APTR data)
{
  LONG response;

  response = SAGE_ReceiveString(bsdsocket, data_buffer, DATA_BUFFER_SIZE);
  if (response <= 0) {
    SAGE_AppliLog("Receive error or socket closed !");
    SAGE_CloseSocket(bsdsocket);
  } else {
    SAGE_AppliLog("Received from client <%s>", data_buffer);
    if (strcmp(data_buffer, "QUIT") == 0) {
      SAGE_AppliLog("Client quit");
      SAGE_CloseSocket(bsdsocket);
    } else if (strcmp(data_buffer, "STOP") == 0) {
      SAGE_AppliLog("Server stop");
      server_stop = TRUE;
      SAGE_CloseSocket(bsdsocket);
    } else {
      strcpy(data_buffer, "Ack from server");
      SAGE_AppliLog("Send aknowledgment to client");
      response = SAGE_SendString(bsdsocket, data_buffer);
      if (response <= 0) {
        SAGE_AppliLog("Send error or socket closed !");
        SAGE_CloseSocket(bsdsocket);
      }
    }
  }
}

VOID AcceptNewClient(SAGE_BsdSocket *bsdsocket, APTR data)
{
  SAGE_BsdSocket *cli_socket;
  
  if ((cli_socket = SAGE_AcceptClient(bsdsocket)) != NULL) {
    SAGE_AppliLog("Accept a new client #%d", cli_socket->socket_id);
    if (SAGE_AddSocketReadHandler(cli_socket, ReadFromClient, NULL)) {
      SAGE_AppliLog("Socket read handler added");
      return;
    }
  }
  SAGE_DisplayError();
}

VOID ReadFromServer(SAGE_BsdSocket *bsdsocket, APTR data)
{
}

LONG manage_server(UWORD port)
{
  SAGE_BsdSocket *srv_socket;
  
  if ((srv_socket = SAGE_OpenServer(SNET_TCP_PROTOCOL, port)) != NULL) {
    SAGE_AppliLog("Server socket ready #%d", srv_socket->socket_id);
    if (SAGE_AddSocketListenHandler(srv_socket, AcceptNewClient, NULL)) {
      SAGE_AppliLog("Socket listen handler added");
      server_stop = FALSE;
      while (!server_stop) {
        SAGE_AppliLog("Running event handler");
        if (!SAGE_HandleSocketEvents()) {
          SAGE_AppliLog("Error while handling events");
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
    SAGE_AppliLog("Small pause");
    SAGE_Pause(50*3);
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
