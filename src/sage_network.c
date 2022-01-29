/**
 * sage_network.c
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Input devices management
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 1.0 December 2020
 */

// @todo : check if socket_id < MAX_SOCKETS

#include "sage_debug.h"
#include "sage_error.h"
#include "sage_logger.h"
#include "sage_memory.h"
#include "sage_context.h"
#include "sage_network.h"

#include <clib/exec_protos.h>
#include <proto/socket.h>

/** @var  library */
struct Library * SocketBase = NULL;

/** SAGE context */
extern SAGE_Context SageContext;

/**
 * Init the network module
 * Open librairies
 *
 * @return Operation success
 */
BOOL SAGE_InitNetworkModule()
{
  SD(SAGE_DebugLog("Init Network module"));
  if ((SocketBase = OpenLibrary(SOCKETNAME, SOCKETVER)) == NULL) {
    SAGE_SetError(SERR_SOCKET_LIB);
    return FALSE;
  }
  if (!SAGE_AllocNetworkDevice()) {
    return FALSE;
  }
  return TRUE;
}

/**
 * Release the network module
 * Close librairies
 *
 * @return Operation success
 */
BOOL SAGE_ReleaseNetworkModule()
{
  SD(SAGE_DebugLog("Release Network module"));
  if (SageContext.SageNetwork != NULL) {
    SAGE_FreeNetworkDevice();
  }
  if (SocketBase != NULL) {
    CloseLibrary(SocketBase);
    SocketBase = NULL;
  }
  return TRUE;
}

/**
 * Allocate the network device
 *
 * @return Operation success
 */
BOOL SAGE_AllocNetworkDevice()
{
  SAGE_NetworkDevice * device;
  
  if ((device = SAGE_AllocMem(sizeof(SAGE_NetworkDevice))) == NULL) {
    return FALSE;
  }
  SageContext.SageNetwork = device;
  return TRUE;
}

/**
 * Free the network device
 *
 * @return Operation success
 */
BOOL SAGE_FreeNetworkDevice()
{
  SAGE_NetworkDevice * network;
  UWORD index;
  
  // Check for network device
  network = SageContext.SageNetwork;
  if (network == NULL) {
    SAGE_SetError(SERR_NO_NETDEVICE);
    return FALSE;
  }
  // Close all open socket
  for (index = 0;index < SNET_MAX_SOCKETS;index++) {
    if (network->sockets[index] != NULL) {
      SAGE_CloseSocket(network->sockets[index]);
    }
  }
}

/**
 * Open a server socket and start listenning
 *
 * @param protocol Socket protocol (TCP or UDP)
 * @param port     Listen port
 *
 * @return Server socket
 */
SAGE_BsdSocket * SAGE_OpenServer(UWORD protocol, UWORD port)
{
  SAGE_NetworkDevice * network;
  SAGE_BsdSocket * bsdsocket;
  
  // Check for network device
  network = SageContext.SageNetwork;
  SAFE(if (network == NULL) {
    SAGE_SetError(SERR_NO_NETDEVICE);
    return NULL;
  })
  bsdsocket = (SAGE_BsdSocket *) SAGE_AllocMem(sizeof(SAGE_BsdSocket));
  if (bsdsocket != NULL) {
    bsdsocket->type = SNET_SERVER_SOCKET;
    bsdsocket->protocol = protocol;
    SD(SAGE_DebugLog("Create server socket"));
    if (protocol == SNET_TCP_PROTOCOL) {
      bsdsocket->socket_id = socket(AF_INET, SOCK_STREAM, 0);
    } else {
      bsdsocket->socket_id = socket(AF_INET, SOCK_DGRAM, 0);
    }
    if (bsdsocket->socket_id == SNET_INVALID_SOCKET) {
      SAGE_SetError(SERR_NO_SOCKET);
      SAGE_FreeMem(bsdsocket);
      return NULL;
    }
    bsdsocket->socket_adr.sin_family = AF_INET;
    bsdsocket->socket_adr.sin_addr.s_addr = INADDR_ANY;
    bsdsocket->socket_adr.sin_port = htons(port);
    bsdsocket->adr_size = sizeof(bsdsocket->socket_adr);
    SD(SAGE_DebugLog("Bind server socket"));
    if (bind(bsdsocket->socket_id, (struct sockaddr *)&(bsdsocket->socket_adr), bsdsocket->adr_size) == SNET_SOCKET_ERROR) {
      SAGE_SetError(SERR_BIND_SOCKET);
      SAGE_FreeMem(bsdsocket);
      return NULL;
    }
    if (bsdsocket->protocol == SNET_TCP_PROTOCOL) {
      SD(SAGE_DebugLog("Listen server socket"));
      if (listen(bsdsocket->socket_id, 5) == SNET_SOCKET_ERROR) {
        SAGE_SetError(SERR_LISTEN_SOCKET);
        SAGE_FreeMem(bsdsocket);
        return NULL;
      }
    }
    bsdsocket->last_error = Errno();
    SD(SAGE_DebugLog("Server socket descriptor #%d", bsdsocket->socket_id));
    network->sockets[bsdsocket->socket_id] = bsdsocket;
    network->nb_sockets++;
    return bsdsocket;
  }
  return NULL;
}

/**
 * Open a client socket and connect to a server
 *
 * @param protocol Socket protocol (TCP or UDP)
 * @param host     Server hostname
 * @param port     Server port
 *
 * @return Client socket
 */
SAGE_BsdSocket * SAGE_OpenClient(UWORD protocol, STRPTR host, UWORD port)
{
  SAGE_NetworkDevice * network;
  struct hostent * hp;
  unsigned int addr;
  SAGE_BsdSocket * bsdsocket;
  
  // Check for network device
  network = SageContext.SageNetwork;
  SAFE(if (network == NULL) {
    SAGE_SetError(SERR_NO_NETDEVICE);
    return NULL;
  })
  bsdsocket = (SAGE_BsdSocket *) SAGE_AllocMem(sizeof(SAGE_BsdSocket));
  if (bsdsocket != NULL) {
    bsdsocket->type = SNET_CLIENT_SOCKET;
    bsdsocket->protocol = protocol;
    SD(SAGE_DebugLog("Create client socket"));
    if (bsdsocket->protocol == SNET_TCP_PROTOCOL) {
      bsdsocket->socket_id = socket(AF_INET, SOCK_STREAM, 0);
    } else {
      bsdsocket->socket_id = socket(AF_INET, SOCK_DGRAM, 0);
    }
    if (bsdsocket->socket_id == SNET_INVALID_SOCKET) {
      SAGE_SetError(SERR_NO_SOCKET);
      SAGE_FreeMem(bsdsocket);
      return NULL;
    }
    if (isalpha(host[0])) {
      hp = gethostbyname(host);
    } else {
      addr = inet_addr(host);
      hp = gethostbyaddr((char *)&addr, 4, AF_INET);
    }
    if (hp == NULL) {
      SAGE_SetError(SERR_RESOLVE_HOST);
      SAGE_FreeMem(bsdsocket);
      return NULL;
    }
    memset(&(bsdsocket->socket_adr), 0, sizeof(bsdsocket->socket_adr));
    memcpy(&(bsdsocket->socket_adr.sin_addr), hp->h_addr, hp->h_length);
    bsdsocket->socket_adr.sin_family = hp->h_addrtype;
    bsdsocket->socket_adr.sin_port = htons(port);
    bsdsocket->adr_size = sizeof(bsdsocket->socket_adr);
    if (protocol == SNET_TCP_PROTOCOL) {
      SD(SAGE_DebugLog("Connect client socket"));
      if (connect(bsdsocket->socket_id, (struct sockaddr*)&(bsdsocket->socket_adr), bsdsocket->adr_size) == SNET_SOCKET_ERROR) {
        SAGE_SetError(SERR_RESOLVE_HOST);
        SAGE_FreeMem(bsdsocket);
        return NULL;
      }
    }
    bsdsocket->last_error = Errno();
    SD(SAGE_DebugLog("Client socket descriptor #%d", bsdsocket->socket_id));
    network->sockets[bsdsocket->socket_id] = bsdsocket;
    network->nb_sockets++;
    return bsdsocket;
  }
  SAGE_SetError(SERR_NO_MEMORY);
  return NULL;
}

/**
 * Close a server or a client socket and release all resources
 *
 * @param bsdsocket Socket pointer
 *
 * @return Operation success
 */
BOOL SAGE_CloseSocket(SAGE_BsdSocket * bsdsocket)
{
  SAGE_NetworkDevice * network;

  // Check for network device
  network = SageContext.SageNetwork;
  SAFE(if (network == NULL) {
    SAGE_SetError(SERR_NO_NETDEVICE);
    return NULL;
  })
  if (bsdsocket != NULL) {
    if (bsdsocket->socket_id != SNET_INVALID_SOCKET) {
      SD(SAGE_DebugLog("Close socket #%d", bsdsocket->socket_id));
      CloseSocket(bsdsocket->socket_id);
      SAGE_FreeMem(network->handlers[bsdsocket->socket_id]);
      network->handlers[bsdsocket->socket_id] = NULL;
      network->sockets[bsdsocket->socket_id] = NULL;
      network->nb_sockets--;
      bsdsocket->socket_id = SNET_INVALID_SOCKET;
    }
    SAGE_FreeMem(bsdsocket);
    return TRUE;
  }
  SAGE_SetError(SERR_NULL_POINTER);
  return FALSE;
}

/**
 * Set a socket as non-blocking / blocking
 *
 * @param bsdsocket Socket pointer
 * @param flag      Non-blocking state
 *
 * @return Operation success
 */
BOOL SAGE_SetNonBlockingSocket(SAGE_BsdSocket * bsdsocket, BOOL flag)
{
  long state;
  
  if (bsdsocket != NULL) {
    if (flag) {
      state = 1;
    } else {
      state = 0;
    }
    if (IoctlSocket(bsdsocket->socket_id, FIONBIO, (char *)&state) == SNET_SOCKET_ERROR) {
      bsdsocket->last_error = Errno();
      SAGE_SetError(SERR_IOCTL_SOCKET);
      return FALSE;
    }
    return TRUE;
  }
  SAGE_SetError(SERR_NULL_POINTER);
  return FALSE;
}

/**
 * Accept a new client
 *
 * @param bsdsocket Server socket
 *
 * @return Client socket
 */
SAGE_BsdSocket * SAGE_AcceptClient(SAGE_BsdSocket * bsdsocket)
{
  SAGE_NetworkDevice * network;
  SAGE_BsdSocket * clisocket;

  // Check for network device
  network = SageContext.SageNetwork;
  SAFE(if (network == NULL) {
    SAGE_SetError(SERR_NO_NETDEVICE);
    return NULL;
  })
  if (bsdsocket != NULL) {
    if (bsdsocket->protocol == SNET_UDP_PROTOCOL) {
      return NULL;
    }
    clisocket = (SAGE_BsdSocket *) SAGE_AllocMem(sizeof(SAGE_BsdSocket));
    if (clisocket != NULL) {
      bsdsocket->type = SNET_CLIENT_SOCKET;
      bsdsocket->protocol = bsdsocket->protocol;
      SD(SAGE_DebugLog("Accept server socket"));
      clisocket->socket_id = accept(bsdsocket->socket_id, (struct sockaddr*)&(clisocket->socket_adr), &(clisocket->adr_size));
      bsdsocket->last_error = Errno();
      if (clisocket->socket_id == SNET_INVALID_SOCKET) {
        SAGE_FreeMem(clisocket);
        SAGE_SetError(SERR_ACCEPT_SOCKET);
        return NULL;
      }
      network->sockets[clisocket->socket_id] = clisocket;
      network->nb_sockets++;
      return clisocket;
    }
    return NULL;
  }
  SAGE_SetError(SERR_NULL_POINTER);
  return NULL;
}

/**
 * Send data on a socket
 *
 * @param bsdsocket Socket pointer
 * @param buffer    Data buffer
 * @param buflen    Data lenght
 *
 * @return Data len sent
 */
LONG SAGE_SendData(SAGE_BsdSocket * bsdsocket, APTR buffer, LONG buflen)
{
  LONG res;

  if (bsdsocket != NULL) {
    SD(SAGE_DebugLog("Send message"));
    if (bsdsocket->protocol == SNET_TCP_PROTOCOL) {
      res = send(bsdsocket->socket_id, buffer, buflen, 0);
    } else {
      res = sendto(bsdsocket->socket_id, buffer, buflen, 0, (struct sockaddr *)&(bsdsocket->socket_adr), bsdsocket->adr_size);
    }
    bsdsocket->last_error = Errno();
    if (res == SNET_SOCKET_ERROR) {
      SAGE_SetError(SERR_SEND_SOCKET);
      return SNET_SOCKET_ERROR;
    }
    return res;
  }
  SAGE_SetError(SERR_NULL_POINTER);
  return SNET_SOCKET_ERROR;
}

/**
 * Receive data from a socket
 *
 * @param bsdsocket Socket pointer
 * @param buffer    Data buffer
 * @param buflen    Size of the buffer
 *
 * @return Data len received
 */
LONG SAGE_ReceiveData(SAGE_BsdSocket * bsdsocket, APTR buffer, LONG buflen)
{
  LONG res;

  if (bsdsocket != NULL) {
    SD(SAGE_DebugLog("Receive message"));
    if (bsdsocket->protocol == SNET_TCP_PROTOCOL) {
      res = recv(bsdsocket->socket_id, buffer, buflen, 0);
    } else {
      res = recvfrom(bsdsocket->socket_id, buffer, buflen, 0, (struct sockaddr *)&(bsdsocket->socket_adr), &(bsdsocket->adr_size));
    }
    bsdsocket->last_error = Errno();
    if (res == 0) {
      SAGE_CloseSocket(bsdsocket);
      return SNET_SOCKET_CLOSE;
    }
    if (res == SNET_SOCKET_ERROR) {
      if (Errno() == EMSGSIZE) {
        return SNET_MESSAGE_PENDING;
      }
      SAGE_SetError(SERR_RECEIVE_SOCKET);
      return SNET_SOCKET_ERROR;
    }
    return res;
  }
  SAGE_SetError(SERR_NULL_POINTER);
  return SNET_SOCKET_ERROR;
}

/**
 * Send a string on a socket
 *
 * @param bsdsocket Socket pointer
 * @param string    String to send
 *
 * @return String len sent
 */
LONG SAGE_SendString(SAGE_BsdSocket * bsdsocket, STRPTR string)
{
  return SAGE_SendData(bsdsocket, (APTR)string, strlen(string));
}

/**
 * Receive a string from a socket
 *
 * @param bsdsocket Socket pointer
 * @param string    String buffer
 * @param len       String buffer size
 *
 * @return String len received
 */
LONG SAGE_ReceiveString(SAGE_BsdSocket * bsdsocket, STRPTR string, LONG len)
{
  LONG res;

  res = SAGE_ReceiveData(bsdsocket, (APTR)string, (len-1));
  if (res >= 0) {
    string[res] = '\0';
  }
  return res;
}

/**
 * Add a listen handler
 *
 * @param bsdsocket Socket pointer
 * @param handler   Handler function
 * @param user_data User data
 *
 * @return Operation success
 */
BOOL SAGE_AddSocketListenHandler(SAGE_BsdSocket * bsdsocket, VOID (*handler)(SAGE_BsdSocket *, APTR), APTR user_data)
{
  SAGE_NetworkDevice * network;
  SAGE_SocketHandler * sockhand;

  // Check for network device
  network = SageContext.SageNetwork;
  SAFE(if (network == NULL) {
    SAGE_SetError(SERR_NO_NETDEVICE);
    return FALSE;
  })
  if (bsdsocket != NULL) {
    if (bsdsocket->type == SNET_SERVER_SOCKET) {
      if (network->handlers[bsdsocket->socket_id] == NULL) {
        if ((sockhand = SAGE_AllocMem(sizeof(SAGE_SocketHandler))) == NULL) {
          return FALSE;
        }
        network->handlers[bsdsocket->socket_id] = sockhand;
      } else {
        sockhand = network->handlers[bsdsocket->socket_id];
      }
      sockhand->socket_handler = handler;
      sockhand->user_data = user_data;
      sockhand->type = SNET_LISTEN_HANDLER;
      return TRUE;
    }
    SAGE_SetError(SERR_NOT_SERVERSOCK);
    return FALSE;
  }
  SAGE_SetError(SERR_NULL_POINTER);
  return FALSE;
}

/**
 * Add a read handler
 *
 * @param bsdsocket Socket pointer
 * @param handler   Handler function
 * @param user_data User data
 *
 * @return Operation success
 */
BOOL SAGE_AddSocketReadHandler(SAGE_BsdSocket * bsdsocket, VOID (*handler)(SAGE_BsdSocket *, APTR), APTR user_data)
{
  SAGE_NetworkDevice * network;
  SAGE_SocketHandler * sockhand;

  // Check for network device
  network = SageContext.SageNetwork;
  SAFE(if (network == NULL) {
    SAGE_SetError(SERR_NO_NETDEVICE);
    return FALSE;
  })
  if (bsdsocket != NULL) {
    if (network->handlers[bsdsocket->socket_id] == NULL) {
      if ((sockhand = SAGE_AllocMem(sizeof(SAGE_SocketHandler))) == NULL) {
        return FALSE;
      }
      network->handlers[bsdsocket->socket_id] = sockhand;
    } else {
      sockhand = network->handlers[bsdsocket->socket_id];
    }
    sockhand->socket_handler = handler;
    sockhand->user_data = user_data;
    sockhand->type = SNET_READ_HANDLER;
    return TRUE;
  }
  SAGE_SetError(SERR_NULL_POINTER);
  return FALSE;
}

/** Handle socket events */
BOOL SAGE_HandleSocketEvents()
{
  SAGE_NetworkDevice * network;
  UWORD index;

  // Check for network device
  network = SageContext.SageNetwork;
  SAFE(if (network == NULL) {
    SAGE_SetError(SERR_NO_NETDEVICE);
    return FALSE;
  })
  FD_ZERO(&(network->read_ds));
  for (index = 0;index < SNET_MAX_SOCKETS;index++) {
    if (network->sockets[index] != NULL && network->handlers[index] != NULL) {
      FD_SET(network->sockets[index]->socket_id, &(network->read_ds));
    }
  }
  SD(SAGE_DebugLog("Wait for a socket event"));
  if (WaitSelect(network->nb_sockets + 1, &(network->read_ds), NULL, NULL, NULL, NULL) == SNET_SOCKET_ERROR) {
    SAGE_SetError(SERR_SELECT_SOCKET);
    return FALSE;
  }
  for (index = 0;index < SNET_MAX_SOCKETS;index++) {
    if (network->sockets[index] != NULL && network->handlers[index] != NULL) {
      SD(SAGE_DebugLog("Check event for socket #%d", network->sockets[index]->socket_id));
      if (FD_ISSET(network->sockets[index]->socket_id, &(network->read_ds))) {
        SD(SAGE_DebugLog("Calling handler for socket #%d", network->sockets[index]->socket_id));
        (*network->handlers[index]->socket_handler)(network->sockets[index], network->handlers[index]->user_data);
      }
    }
  }
  return TRUE;
}

/**
 * Get the last network error
 *
 * @return Last network error code
 */
LONG SAGE_GetLastNetworkError()
{
  return Errno();
}

/**
 * Get the last socket error
 */
LONG SAGE_GetLastSocketError(SAGE_BsdSocket * bsdsocket)
{
  if (bsdsocket != NULL) {
    return bsdsocket->last_error;
  }
  SAGE_SetError(SERR_NULL_POINTER);
  return SNET_SOCKET_ERROR;
}

/**
 * Dump network device
 */
VOID SAGE_DumpNetworkDevice()
{
}
