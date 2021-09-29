/**
 * sage_network.h
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Network management
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 1.0 December 2020
 */

#ifndef _SAGE_NETWORK_H_
#define _SAGE_NETWORK_H_

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include <sys/socket.h>
#include <sys/errno.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <netdb.h>
#include <bsdsocket/socketbasetags.h>

#define SOCKETVER             4

#define SNET_TCP_PROTOCOL     1
#define SNET_UDP_PROTOCOL     2

#define SNET_UNDEF_SOCKET     0
#define SNET_SERVER_SOCKET    1
#define SNET_CLIENT_SOCKET    2

#define SNET_INVALID_SOCKET   -1
#define SNET_SOCKET_CLOSE     0
#define SNET_SOCKET_ERROR     -1
#define SNET_MESSAGE_PENDING  -2

#define SNET_NO_CLIENT        EAGAIN

#define SNET_LISTEN_HANDLER   1
#define SNET_READ_HANDLER     2

#define SNET_MAX_SOCKETS      16

/** SAGE network socket */
typedef struct {
  UWORD type, protocol;
  long socket_id;
  struct sockaddr_in socket_adr;
  long adr_size, last_error;
} SAGE_BsdSocket;

/** SAGE socket handler */
typedef struct {
  /** Socket handler */
  VOID (*socket_handler)(SAGE_BsdSocket *, APTR);
  /** User data */
  APTR user_data;
  /** Handler type */
  UWORD type;
} SAGE_SocketHandler;

/** SAGE network device */
typedef struct {
  /** System sockets */
  SAGE_BsdSocket * sockets[SNET_MAX_SOCKETS];
  UWORD nb_sockets;
  /** Socket handlers */
  SAGE_SocketHandler * handlers[SNET_MAX_SOCKETS];
  /** Read descriptor set */
  fd_set read_ds;
} SAGE_NetworkDevice;

/** Init the network module */
BOOL SAGE_InitNetworkModule(VOID);

/** Release the network module */
BOOL SAGE_ReleaseNetworkModule(VOID);

/** Allocate the network device */
BOOL SAGE_AllocNetworkDevice(VOID);

/** Free the network device */
BOOL SAGE_FreeNetworkDevice(VOID);

/** Open a server socket */
SAGE_BsdSocket * SAGE_OpenServer(UWORD, UWORD);

/** Open a client socket and connect to a server */
SAGE_BsdSocket * SAGE_OpenClient(UWORD, STRPTR, UWORD);

/** Close a server or a client socket */
BOOL SAGE_CloseSocket(SAGE_BsdSocket *);

/** Set a socket as non-blocking / blocking */
BOOL SAGE_SetNonBlockingSocket(SAGE_BsdSocket *, BOOL);

/** Accept a new client */
SAGE_BsdSocket * SAGE_AcceptClient(SAGE_BsdSocket *);

/** Send data on a socket */
LONG SAGE_SendData(SAGE_BsdSocket *, APTR, LONG);

/** Receive data from a socket */
LONG SAGE_ReceiveData(SAGE_BsdSocket *, APTR, LONG);

/** Send a string on a socket */
LONG SAGE_SendString(SAGE_BsdSocket *, STRPTR);

/** Receive a string from a socket */
LONG SAGE_ReceiveString(SAGE_BsdSocket *, STRPTR, LONG);

/** Add a listen handler */
BOOL SAGE_AddSocketListenHandler(SAGE_BsdSocket *, VOID (*handler)(SAGE_BsdSocket *, APTR), APTR);

/** Add a read handler */
BOOL SAGE_AddSocketReadHandler(SAGE_BsdSocket *, VOID (*handler)(SAGE_BsdSocket *, APTR), APTR);

/** Handle socket events */
BOOL SAGE_HandleSocketEvents(VOID);

/** Get the last network error */
LONG SAGE_GetLastNetworkError(VOID);

/** Get the last socket error */
LONG SAGE_GetLastSocketError(SAGE_BsdSocket *);

/** Dump network device */
VOID SAGE_DumpNetworkDevice(VOID);

#endif
