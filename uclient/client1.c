#include "global.h"
char path[300];
#define MAX 1448
void  str_cli(int sockfd){
   int err,i,j;
   
   unsigned int *mid;
   char buf[1448];
   unsigned char *he;
   char hash[41]; 
   FILE *fin,*fout,*fget;
   
   //计算指纹（分块计算指纹）
   FingerChunk *fingerchunk;
   FileInfo *fileinfo=NULL;
   memset(path,0,300);
   //请输入传输文件的路径
   gets(path);
   fileinfo=file_new();
   memcpy(fileinfo->file_path,path,strlen(path));
   err=SHA1File(path,fileinfo->file_hash);
   if(err!=shaSuccess)
   {
     printf("calculate file hash faild");
     return 0;
   }  
   fileinfo->is_new=0;
  if(fileinfo->is_new==0)
  {
     err=chunk_file(fileinfo);
     if(err==0)
      {
        printf("chunk failed");
        return 0;
      }
  }
   fget=fopen(fileinfo->file_path,"r");
   fout=fopen("/home/wulinqian/wulinqian/experiment/test3/result1","a");
   fin=fopen("/home/wulinqian/wulinqian/experiment/test3/result","a");
   fingerchunk=fileinfo->first;
   fprintf(fin,"chunknum:%d:\n",fileinfo->chunknum);
    j=0;
   //输出所有指纹到文件中fin
 while(fingerchunk!=NULL)
   {
     i=0;
    
     memset(buf,0,1500);
     digestToHash(fingerchunk->chunk_hash,hash);
     fprintf(fin,"checksize:%d   ",fingerchunk->chunklen);
     he=(unsigned char *)(fingerchunk->chunk_hash);
     fprintf(fin,"fingerprint1:");
  
    //传输数据中加入fp；
     while(i<20)
     {
        fprintf(fin,"%d:",he[i]);
        buf[i]=he[i];
        i++;
     }

     //传输数据中加入ID
      mid=(unsigned int *)(&buf[20]);
      *mid=j+1;
      j++;
   
    //读入数据fget
     fread((unsigned char *)(&buf[24]),sizeof(char),fingerchunk->chunklen,fget);

    //输出数据fout
     fwrite((unsigned char *)(&buf[24]),sizeof(char),fingerchunk->chunklen,fout);

    //传输数据
      i=24+fingerchunk->chunklen;
      write(sockfd,buf,i);
      read(sockfd,hash,3);
      printf("%s\n",hash); 
      fprintf(fin,"    fingerprint:%s\n",hash);
      fingerchunk=fingerchunk->next;
     
   }
   fclose(fin);
   fclose(fout);
   fclose(fget);
}

int main(int argc,char **argv){
      int sockfd,n;
      int i,len;
      char recvline[100];
      const char chopt=1;
      struct sockaddr_in servaddr;

      if(argc!=2)
      {
         printf("miss ip");
         return 1;
      }     
      if((sockfd=socket(AF_INET,SOCK_STREAM,0))<0)
      {
         printf("socket error");
         return 2;
      }
        
      bzero(&servaddr,sizeof(servaddr));
      servaddr.sin_family=AF_INET;
      servaddr.sin_port=htons(1235);
      
      if(inet_pton(AF_INET,argv[1],&servaddr.sin_addr)<=0)
      {
         printf("inet_pton error");
         return 3;
      }
      
      if(connect(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr))<0)
      {
          printf("connect error");
          return 4;
      }
      
      str_cli(sockfd);

      exit(0);
}






