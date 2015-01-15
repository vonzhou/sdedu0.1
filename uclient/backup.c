#include "global.h"


/*int  walk_dir(Client *c, char *path) 
{
	struct stat state;
	if (stat(path, &state) != 0) {
		err_msg1("file does not exist! ignored!");
		return 0;
	}
	if (S_ISDIR(state.st_mode)) {
		DIR *dir = opendir(path);
		struct dirent *entry;
		char newpath[512];
		memset(newpath,0,512);
		if (strcmp(path + strlen(path) - 1, "/")) {
			strcat(path, "/");
		}

		while ((entry = readdir(dir)) != 0) {
		//	ignore . and ..
			if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, ".."))//(entry->d_name[0]=='.')
				continue;
			strcpy(newpath, path);
			strcat(newpath, entry->d_name);
			if (walk_dir(c, newpath) != 0) {
				return -1;
			}
		}
		//printf("*** out %s direcotry ***\n", path);
		closedir(dir);
	} 
	else if (S_ISREG(state.st_mode)) {
		file_dudup(c, path);

	} else {
		err_msg1("illegal file type! ignored!");
		return 0;
	}
	return 0;
}
*/


int chunk_file(FileInfo *fileinfo) 
{
	int subFile;
    FILE *fin;
    int32_t srclen=0, left_bytes = 0;
    int32_t size=0,len=0; 
    int32_t n = MAX_CHUNK_SIZE;

	unsigned char *p;
	FingerChunk *fc;
    unsigned char *src = (unsigned char *)malloc(MAX_CHUNK_SIZE*2);	

    chunk_alg_init();
	if(src == NULL) {
        printf("Memory allocation failed");
		return 0;
    }

	if ((subFile = open(fileinfo->file_path, O_RDONLY)) < 0) {
	    printf("%s,%d: open file %s error\n",__FILE__, __LINE__, fileinfo->file_path);
		free((char*)src);
		return 0;
	}

	while(1) 
	{
	 	if((srclen = readn(subFile, (char *)(src+left_bytes), MAX_CHUNK_SIZE)) <= 0)
		    break;

		if (left_bytes > 0){ 
			srclen += left_bytes;
			left_bytes = 0;
		} 

		if(srclen < MIN_CHUNK_SIZE)
		 	break;
		
		p = src;
		len = 0;
		while (len < srclen) 
		{
          	n = srclen -len;
			size = chunk_data(p, n);
                     /*   fin=fopen("/home/wulinqian/wulinqian/experiment/test3/result","a");
                        fprintf(fin,"size:%d\n",size);
                        fclose(fin); */         
			if(n==size && n < MAX_CHUNK_SIZE)
			{ 	
          		memmove(src, src+len, n );
          		left_bytes = n;
                break;
			}  
      			
			fc = fingerchunk_new();
			chunk_finger(p, size, fc->chunk_hash);
			fc->chunklen = size;
			file_append_fingerchunk(fileinfo, fc);
                       /*
		        fin=fopen("/home/wulinqian/wulinqian/experiment/test3/result","a");
                        fprintf(fin,"hash:%x\n",fc->chunk_hash);
                        fclose(fin);	
                       */  
			p = p + size;
			len += size;
		}
    }
	
	/******more******/
	len = 0;
	if(srclen > 0)
	    len=srclen;
	else if(left_bytes > 0)
	 	len=left_bytes;
	if(len > 0){
	 	p= src;
	 	fc = fingerchunk_new();
		chunk_finger(p, len, fc->chunk_hash);
		fc->chunklen = len;
		file_append_fingerchunk(fileinfo, fc);
	 }	
	 close(subFile);
   	 free(src);
   	 return 1;
}
