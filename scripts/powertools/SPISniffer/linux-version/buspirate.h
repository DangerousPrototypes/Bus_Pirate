#ifndef BUSPIRATE_h_
#define BUSPIRATE_h_

#include <stdint.h>


uint32_t BP_WriteToPirate(int fd, char * val);
void BP_EnableMode(int fd, char bbmode);

#endif


