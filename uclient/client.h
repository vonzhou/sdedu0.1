/*
 * client.h
 *
 *  Created on: 6.24, 2013
 *      Author: Yifan Yang
 */


/* define the type of fingerprint */
#ifndef RECIPE_H_
typedef unsigned char Fingerprint[20];

/* define the type of chunk address */

typedef char Chunkaddress[64]; 
#endif
/*
typedef struct Client{
	char username[USERNAME_LENGTH];
    char password[PASSWORD_LENGTH];
	int fd;
    JCR *jcr;
}Client;
*/
/* functions */
void test_data(int socket,char *filename);
void usage(char *program);
//Client *create_client();
//void free_client(Client *c);


