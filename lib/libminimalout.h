#ifndef LIBMINIMALOUT_H
#define LIBMINIMALOUT_H


#include "options.h"
#include "utils.h"
#include <stdio.h>


void FINISH(CanvasOpts * opts,
	    Datum *** dataa,
	    int datal,
	    FILE ** filea,
	    int filel);


int VALIDATE(Datum *** dataa,
	    int datal,
	    FILE ** filea,
	    int filel);



#endif /* LIBMINIMALOUT_H */
