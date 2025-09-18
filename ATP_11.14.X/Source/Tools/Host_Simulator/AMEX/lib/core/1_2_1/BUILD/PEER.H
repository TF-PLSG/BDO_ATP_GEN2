/*.................................................................
 *..................................................................*/

#include "xipc.h"


/*.................................................................
 *..................................................................*/

#ifndef PEER_H
#define PEER_H

#define MAX_PEERS		10
#define MAX_INSTANCES	10
#define	WAIT			NULL
#define	MSGBUF_SIZE		1024

XINT	PeerEnvEnter C_PROTOARGS((VOID));
XINT	PeerSend C_PROTOARGS((VOID));
CHAR *	PeerRecv C_PROTOARGS((VOID));
XINT	PeerEnvEnter C_PROTOARGS((VOID));

#endif /* PEER_H */
