#include "global.h"

#define SERV_PORT 1234
#define CMD_PORT 4321

#define MAX 1448
#define CMD_LEN 20

char path[300];  // file to transfer
int cmd_sockfd, sockfd;

struct DeduHead{
		unsigned short int flag;      //decide whether need to deduplication
		unsigned short int private;   //the reserved area
		unsigned int chunkID;         //#chunks
		unsigned char fp[20];         //fingerprint 
		char filename[20];       // other metadata
};


void  transfer_file(int sockfd,const struct sockaddr *servaddr,int servlen){
		int err, len, chunkid;
		struct sockaddr *reply_addr;
		char buf[1500];
		unsigned char *he;
		char hash[41]; 
		char command[CMD_LEN];  // cmd from server or sdn controller
		FILE *fp,*fout,*fget;
		struct DeduHead *phead;

		len=servlen;
		/*
		 * File Chunk 
		 */
		//get the fingerprint for each chunk
		FingerChunk *fingerchunk;
		FileInfo *fileinfo=NULL;
		fileinfo=file_new();
		memcpy(fileinfo->file_path,path,strlen(path));
		err=SHA1File(path,fileinfo->file_hash);
		if(err!=shaSuccess)
		{
				printf("calculate file hash faild");
				return ;
		}  
		err=chunk_file(fileinfo);
		if(err==0)
		{
				printf("chunk failed");
				return ;
		}

		fp=fopen(fileinfo->file_path,"r");
		// error checking ...

		fingerchunk = fileinfo->first;
		chunkid = 0;

		/*
		 * Construct The First Packet to transfer 
		 * The first packet is file metadata, ie,fp, file name...
		 */
		phead=(struct DeduHead *)malloc(sizeof(struct DeduHead));
		memset(phead, 0, sizeof(struct DeduHead));
		memset(buf, 0, MAX);
		phead->chunkID = chunkid;
		memcpy(phead->fp, fileinfo->file_hash, 20);
		memcpy(phead->filename, path, strlen(path));

		sendto(sockfd, phead, sizeof(struct DeduHead),0,servaddr,servlen);  // first packet !!!

		memset(hash,0,41);
		memset(command, 0, CMD_LEN);
		recvfrom(cmd_sockfd,command,CMD_LEN,0, NULL, NULL); // get a reply from sever/controller
		printf("%s\n",command);

		// if redundant 
		if(!strcmp(command, "ACK")){
				printf("File exists.Transfer stopped!\n");
		} else{
		// New file , transfer all the chunks , do not need the header
				while(fingerchunk != NULL)
				{
						memset(buf,0,1500); // ??

						chunkid ++;
						printf("chunk %d ..\n", chunkid);
						//read the data form fget
						fread(buf, sizeof(char), fingerchunk->chunklen, fp);

						sendto(sockfd, buf, sizeof(buf), 0, servaddr, servlen);

						fingerchunk=fingerchunk->next;

				}
		}
		fclose(fp);
}

int main(int argc,char **argv)
{
		struct sockaddr_in servaddr, controladdr;

		if(argc != 3)
		{
				printf("Usage : ./client <IP> <File>\n");
				exit(-1);
		}
		if((sockfd=socket(AF_INET,SOCK_DGRAM,0))<0)
		{
				printf("socket error\n");
				exit(-1);
		}
		bzero(&servaddr,sizeof(servaddr));
		servaddr.sin_family=AF_INET;

		servaddr.sin_port=htons(SERV_PORT);
		if(inet_pton(AF_INET,argv[1],&servaddr.sin_addr)<=0)
		{
				printf("inet_pton error\n");
				exit(-1);
		}
		//Get Command socket
		cmd_sockfd = socket(AF_INET,SOCK_DGRAM,0); /*create a socket for command */

		/*init servaddr*/
		bzero(&controladdr,sizeof(controladdr));
		controladdr.sin_family = AF_INET;
		controladdr.sin_addr.s_addr = htonl(INADDR_ANY);
		controladdr.sin_port = htons(CMD_PORT);

		//bind address and port to local control  socket*
		if(bind(cmd_sockfd,(struct sockaddr *)&controladdr,sizeof(controladdr)) == -1)
		{
				perror("bind error");
				exit(-1);
		}
		
		strcpy(path, argv[2]);

		// Run
		transfer_file(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr));

		exit(0);
}





