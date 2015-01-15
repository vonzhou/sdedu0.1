#ifndef _RABIN_H
#define _RABIN_H


#define MIN_CHUNK_SIZE 1024  /*1k */

#define MAX_CHUNK_SIZE  1400 /*2k */

 void chunk_alg_init();
 void chunk_finger(unsigned char *buf, uint32_t len,unsigned char hash[]);
  int   chunk_data (unsigned char *p, int n);
void digestToHash(unsigned char digest[20],char hash[41]);

#endif

