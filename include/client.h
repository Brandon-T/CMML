#ifndef __client_h_
#define __client_h_

#include <stdbool.h>
#include "iomanager.h"
#include "finder.h"

typedef struct Client_t
{
	bool ownIO;
	IOManager *io;
	Finder finder;

	//OCR ocr;
	//DTMs dtms;
	//Bitmaps bitmaps;
} Client;

extern Client initClient(IOManager *io);
extern void freeClient(Client *client);

#endif // __client_h_
