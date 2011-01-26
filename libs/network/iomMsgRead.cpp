/* ****************************************************************************
*
* FILE                     iomMsgRead.cpp
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Oct 13 2010
*
*/
#include <unistd.h>             // write
#include <memory.h>             // memset
#include <sys/time.h>           // gettimeofday

#include "logMsg.h"             // LM_*
#include "traceLevels.h"        // LmtRead, ...

#include "Message.h"            // MessageType, Code, etc.
#include "Endpoint.h"           // Endpoint
#include "Packet.h"             // Packet
#include "Buffer.h"             // Buffer
#include "MemoryManager.h"      // MemoryManager
#include "iomMsgRead.h"         // Own interface
#include "logMsg.h" // Logs


/* ****************************************************************************
*
* full_read - read from a socket until completed or error
*/
ssize_t full_read(int fd, char* buf, ssize_t bufLen)
{
	ssize_t  tot = 0;

	while (tot < bufLen)
	{
		ssize_t nb;

		nb = read(fd, (char*) buf + tot , bufLen - tot);

		if (nb == -1)
		{
			if (errno == EBADF)
				LM_RE(-2, ("read(connection closed?): %s", strerror(errno)));

			LM_RE(-1, ("read: %s", strerror(errno)));
		}
		else if (nb == 0)
		{
			if (tot == 0)
			{
				LM_T(LmtRead, ("read 0 bytes - connection closed"));
				return -2;
			}

			LM_T(LmtRead, ("read %d bytes from fd %d", tot, fd));
			LM_READS("someone", "?header?", buf, tot, LmfByte);
			LM_RE(tot, ("last read gave 0 bytes (%d bytes read in total). Connection Closed ?", tot));
		}

		tot += nb;
	}

	LM_T(LmtRead, ("read %d bytes from fd %d", tot, fd));
	LM_READS("someone", "?header?", buf, tot, LmfByte);
	return tot;
}



/* ****************************************************************************
*
* iomMsgRead - read a message from an endpoint
*/
int iomMsgRead
(
	int                        fd,
	const char*                from,
	ss::Message::MessageCode*  msgCodeP,
	ss::Message::MessageType*  msgTypeP,
	void**                     dataPP,
	int*                       dataLenP,
	ss::Packet*                packetP,
	void*                      kvData,
	int*                       kvDataLenP
)
{
	int                  nb;
	ss::Message::Header  header;
	char*                inBuffer = (char*) *dataPP;

	*dataLenP = 0;
	nb        = read(fd, &header, sizeof(header));
	
	if (nb == -1)
		LM_RE(1, ("reading header from '%s'", from));

	if (nb == 0)
	{
		LM_T(LmtRead, ("read 0 bytes from '%s' - connection closed", from));
		return -2;
	}

	if (nb != sizeof(header))
		LM_RE(1, ("reading header from '%s' - read only %d bytes (need %d)", from, nb, sizeof(header)));

	LM_READS(from, "header", &header, nb, LmfByte);

	if (header.magic != 0xFEEDC0DE)	
	{
		int* iP = (int*) &header;

		LM_W(("Bad header ...(0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x", iP[0], iP[1], iP[2], iP[3], iP[4], iP[5], iP[6], iP[7]));
		LM_X(1, ("Bad magic number in header (0x%08x)", header.magic));
	}

	*msgCodeP = header.code;
	*msgTypeP = header.type;

	LM_T(LmtRead, ("read %d bytes of '%s' %s header from '%s' (fd %d)",
				   nb, messageCode(header.code), messageType(header.type), from, fd));

	LM_READS(from, "message header", &header, sizeof(header), LmfByte);

	if (header.dataLen != 0)
	{
		int           nb;
		unsigned int  tot;

		if (header.dataLen > (unsigned int) *dataLenP)
		{
			*dataPP = (char*) malloc(header.dataLen);
			if (*dataPP == NULL)
				LM_X(1, ("malloc(%d)", header.dataLen));

			inBuffer = (char*) *dataPP;
		}

		LM_T(LmtRead, ("reading %d bytes of primary message data", header.dataLen));

		int reads = 0;

		tot = 0;
		while (tot < header.dataLen)
		{
			nb = read(fd, &inBuffer[tot], header.dataLen - tot);
			++reads;
			if (nb == -1)
				LM_RP(1, ("read %d bytes from '%s'", header.dataLen, from));
			LM_T(LmtRead, ("read %d bytes of primary message data", nb));

			tot += nb;
		}

		*dataLenP = tot;

		LM_READS(from, "primary data", *dataPP, tot, LmfByte);
	}

	if (header.gbufLen != 0)
	{
		void* dataP = (void*)  malloc(header.gbufLen + 1);

		if (dataP == NULL)
			LM_X(1, ("malloc(%d)", header.gbufLen));

		LM_T(LmtRead, ("reading %d bytes of google protocol buffer data", header.gbufLen));
        nb = read(fd, dataP, header.gbufLen);
        if (nb == -1)
			LM_RP(1, ("read(%d bytes from '%s')", header.gbufLen, from));

		if (nb != (int) header.gbufLen)
			LM_X(1, ("read %d bytes instead of %d", nb, header.gbufLen));

		((char*) dataP)[nb] = 0;

		packetP->message.ParseFromArray(dataP, nb);
		if( packetP->message.IsInitialized() == false)
			LM_X(1, ("Error parsing Google Protocol Buffer when reading a message %s ! (not initialized)  ",ss::Message::messageCode(header.code)));

		LM_READS(from, "google protocol buffer", dataP, nb, LmfByte);
	}

	if (header.kvDataLen != 0)
	{
		char         name[128];
		static int   bIx = 0;

		sprintf(name, "Buffer%d", bIx);
		++bIx;

		packetP->buffer = ss::MemoryManager::shared()->newBuffer(name, header.kvDataLen);

		int    size  = header.kvDataLen;
		char*  kvBuf = packetP->buffer->getData();
		int    tot   = 0;
		int    nb;

		while (tot < size)
		{
			// msgAwait()
			nb = read(fd, &kvBuf[tot], size - tot);
			LM_T(LmtRead, ("read %d bytes KVDATA from '%s'", tot, from));
			if (nb == -1)
				LM_RE(-1, ("read(%d bytes) from '%s': %s", size - tot, from, strerror(errno)));
			tot += nb;
		}

		packetP->buffer->setSize(tot);
	}

	return 0;
}	



/* ****************************************************************************
*
* iomMsgRead - read a message from an endpoint
*/
int iomMsgRead
(
	ss::Endpoint*              ep,
	ss::Message::Header*       headerP,
	ss::Message::MessageCode*  msgCodeP,
	ss::Message::MessageType*  msgTypeP,
	void**                     dataPP,
	int*                       dataLenP,
	ss::Packet*                packetP,
	void*                      kvData,
	int*                       kvDataLenP
)
{
	int nb;
	struct timeval start;
	struct timeval end;

	gettimeofday(&start, NULL);

	*msgCodeP = headerP->code;
	*msgTypeP = headerP->type;

	if (dataPP == NULL)
		LM_X(1, ("dataPP == NULL"));

	if (headerP->magic != 0xFEEDC0DE)
	{
		LM_READS(ep->name.c_str(), "header", headerP, sizeof(ss::Message::Header), LmfByte);
		LM_X(1, ("bad magic number in message from %s@%s (fd: %d) ", ep->name.c_str(), ep->ip.c_str(), ep->rFd));
	}

	if (headerP->dataLen != 0)
	{
		if (headerP->dataLen > 1000)
			LM_X(1, ("Reading a '%s' %s from '%s' (dataLens: %d, %d, %d)",
					 ss::Message::messageCode(headerP->code), ss::Message::messageType(headerP->type), ep->name.c_str(),
					 headerP->dataLen, headerP->gbufLen, headerP->kvDataLen));

		LM_T(LmtRead, ("Reading %d bytes of data", headerP->dataLen));
		if (headerP->dataLen > (unsigned int) *dataLenP)
		{
			LM_W(("Allocating extra space for message"));
			*dataPP = (char*) malloc(headerP->dataLen);
			if (*dataPP == NULL)
				LM_X(1, ("malloc(%d)", headerP->dataLen));
		}

		LM_T(LmtRead, ("reading %d bytes of primary message data from '%s'", headerP->dataLen, ep->name.c_str()));
		nb = full_read(ep->rFd, (char*) *dataPP, headerP->dataLen);
		LM_T(LmtRead, ("read %d bytes DATA from '%s'", nb, ep->name.c_str()));
		if (nb == -1)
			LM_RP(1, ("read %d bytes from '%s' (wanted %d bytes)", nb, ep->name.c_str(), headerP->dataLen));
		LM_T(LmtRead, ("read %d bytes of primary message data", nb));

		if (nb != (int) headerP->dataLen)
			LM_E(("Read %d bytes from '%s', %d expected ...", nb, ep->name.c_str(), headerP->dataLen));

		*dataLenP = nb;

		LM_T(LmtRead, ("read %d bytes from '%s'", nb, ep->name.c_str()));
		LM_READS(ep->name.c_str(), "primary data", *dataPP, nb, LmfByte);
	}

	if (headerP->gbufLen != 0)
	{
		char* dataP = (char*) malloc(headerP->gbufLen + 1);
		int   tot   = 0;

		if (dataP == NULL)
			LM_X(1, ("malloc(%d)", headerP->gbufLen));

		LM_T(LmtRead, ("reading %d bytes of google protocol buffer data", headerP->gbufLen));
		while (tot < (int) headerP->gbufLen)
		{
			nb = read(ep->rFd, &dataP[tot], headerP->gbufLen - tot);
			LM_T(LmtRead, ("read %d bytes GPROTBUF from '%s'", nb, ep->name.c_str()));
			if (nb == -1)
				LM_RP(1, ("read(%d bytes from '%s')", headerP->gbufLen, ep->name.c_str()));
			tot += nb;
		}

		((char*) dataP)[tot] = 0;
		
		packetP->message.ParseFromArray(dataP, tot);
		if( packetP->message.IsInitialized() == false)
			LM_X(1, ("Error parsing Google Protocol Buffer of %d bytes because a message %s is not initialized!", tot , ss::Message::messageCode(headerP->code)));

		LM_READS(ep->name.c_str(), "google protocol buffer", dataP, tot, LmfByte);
	}

	if (headerP->kvDataLen != 0)
	{
		char         name[128];
		static int   bIx = 0;

		sprintf(name, "%s:%d", ep->name.c_str(), bIx);
		++bIx;

		packetP->buffer = ss::MemoryManager::shared()->newBuffer(name, headerP->kvDataLen);

		int    size   = headerP->kvDataLen;
		char*  kvBuf  = packetP->buffer->getData();
		int    size2  = packetP->buffer->getMaxSize();
		int    tot    = 0;
		int    nb;

		LM_T(LmtRead, ("reading a KV buffer of %d bytes", size2));
		while (tot < size)
		{
			// msgAwait() ... ?
			LM_T(LmtRead, ("trying to read %d bytes of KV buffer", size - tot));
			nb = read(ep->rFd, &kvBuf[tot], size - tot);
			LM_T(LmtRead, ("read %d bytes of KV buffer", nb));

			if (nb == -1)
				LM_RE(-1, ("read(%d bytes) from '%s': %s", size - tot, ep->name.c_str(), strerror(errno)));
			else if (nb == 0)
				LM_RE(-2, ("endpoint '%s' seems to have closed the connection", ep->name.c_str()));
			tot += nb;
		}

		packetP->buffer->setSize(tot);
	}

	int bytesRead = sizeof(ss::Message::Header) + headerP->dataLen + headerP->gbufLen + headerP->kvDataLen;

	ep->msgsIn  += 1;
	ep->bytesIn += bytesRead;

	if (bytesRead > 100)
	{
		struct timeval diff;
		int            usecs;

		gettimeofday(&end, NULL);
		diff.tv_sec  = end.tv_sec  - start.tv_sec;
		diff.tv_usec = end.tv_usec - start.tv_usec;

		if (diff.tv_usec < 0)
		{
			diff.tv_sec  -= 1;
			diff.tv_usec += 1000000;
		}

		usecs = diff.tv_sec * 1000000 + diff.tv_usec;

		if (usecs < 0)
			LM_X(1, ("usecs cannot be < 0 ..."));

		if (usecs == 0)
			return 0;

		ep->rMbps = bytesRead / usecs;

		ep->rAccMbps = (ep->rAccMbps * ep->reads + ep->rMbps) / (ep->reads + 1);
		ep->reads += 1;
	}
	
	return 0;
}	
