/*
* 
* Copyright (C) 2012, All rights reserved.
* Jorge Garrido <jorge.garrido@morelosoft.com>
* ehecatl_api_sc.c 
*
*/

/* include libraries */
#include <stdio.h>
#include <stdlib.h>
#ifdef __APPLE__
#include
#include <PCSC/wintypes.h>
#else
#include <winscard.h>
#endif

/* check some errors on ehecatl_api_sc (when read card) */
#define EHECATL_API_SC_(f, rv) \
 if (SCARD_S_SUCCESS != rv) \
 { \
  return "error"; \
 }

/* function prototypes
*
* name : ehecatl_api_sc
* args : int command, it will be:
*	 1 - read
*	 2 - write
* return : char *
*/
char * ehecatl_api_sc(int command);

/* ehecatl_api_sc */ 
char * ehecatl_api_sc(int command) {
    LONG rv;
    
    SCARDCONTEXT hContext;
    LPTSTR mszReaders;
    SCARDHANDLE hCard;
    DWORD dwReaders, dwActiveProtocol, dwRecvLength;

    SCARD_IO_REQUEST pioSendPci;
    BYTE pbRecvBuffer[258];

    // command to read binary
    BYTE read[] = { 0xFF, 0xB0, 0x00, 0x08, 0x20};

    // commmand to write binary
    BYTE write[] = { 0xFF, 0xD6, 0x00, 0x08, 0x10, 0x68, 0x6F, 0x6C, 0x61 };

    unsigned int i;
   
    rv = SCardEstablishContext(SCARD_SCOPE_SYSTEM, NULL, NULL, &hContext);
    EHECATL_API_SC_("SCardEstablishContext", rv)

    #ifdef SCARD_AUTOALLOCATE
    dwReaders = SCARD_AUTOALLOCATE;

    rv = SCardListReaders(hContext, NULL, (LPTSTR)&mszReaders, &dwReaders);
    EHECATL_API_SC_("SCardListReaders", rv)
    #else

    rv = SCardListReaders(hContext, NULL, NULL, &dwReaders);
    EHECATL_API_SC_("SCardListReaders", rv)

    mszReaders = calloc(dwReaders, sizeof(char));
    rv = SCardListReaders(hContext, NULL, mszReaders, &dwReaders);
    EHECATL_API_SC_("SCardListReaders", rv)
    #endif

    rv = SCardConnect(hContext, mszReaders, SCARD_SHARE_SHARED,
    SCARD_PROTOCOL_T0 | SCARD_PROTOCOL_T1, &hCard, &dwActiveProtocol);
    EHECATL_API_SC_("SCardConnect", rv)

    switch(dwActiveProtocol) {
        case SCARD_PROTOCOL_T0:
            pioSendPci = *SCARD_PCI_T0;
            break;

        case SCARD_PROTOCOL_T1:
            pioSendPci = *SCARD_PCI_T1;
            break;
    }

    dwRecvLength = sizeof(pbRecvBuffer);

    // read or write on connected card
    if (command == 1) 
        rv = SCardTransmit(hCard, &pioSendPci, read, sizeof(read), NULL, pbRecvBuffer, &dwRecvLength);
    else 
        rv = SCardTransmit(hCard, &pioSendPci, write, sizeof(write), NULL, pbRecvBuffer, &dwRecvLength);

    EHECATL_API_SC_("SCardTransmit", rv)

    // ret val to erlang port
    char ehecatl_got_card_value[dwRecvLength];
    
    // fill the ret val
    for(i=0; i<dwRecvLength; i++)
        ehecatl_got_card_value[i] = pbRecvBuffer[i];

    rv = SCardDisconnect(hCard, SCARD_LEAVE_CARD);
    EHECATL_API_SC_("SCardDisconnect", rv)

    #ifdef SCARD_AUTOALLOCATE
    rv = SCardFreeMemory(hContext, mszReaders);
    EHECATL_API_SC("SCardFreeMemory", rv)

    #else
    free(mszReaders);
    #endif

    rv = SCardReleaseContext(hContext);

    EHECATL_API_SC_("SCardReleaseContext", rv)

    // return ret val
    return ehecatl_got_card_value;
    
}

