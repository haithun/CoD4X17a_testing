#define UPDT_HTML_REQUEST "GET %s HTTP/1.1 \r\nHost: update.iceops.in \r\nUser-Agent: CoD4v1.7a self updater \r\nAccept-Encoding: \r\nConnection: keep-alive\r\n\r\n"

#define UPDT_DOWNLOAD_REQUEST "GET %s HTTP/1.1 \r\nHost: update.iceops.in \r\nUser-Agent: CoD4v1.7a self updater \r\nAccept: text/html\r\nAccept-Encoding: \r\nConnection: keep-alive\r\n\r\n"

#define UPDT_USAGE "<sys_update> [-h,-v,-u]\n"

#define UPDT_MAX_RETRIES 3

// Symbols from autoupdate.asm
void UPDT_SwapAndExec(char *execFilePath, char *execArguments, int n, char **fileNames); 
extern int UPDT_CODE_START, UPDT_CODE_END;

typedef enum {
    UPDT_ACTION_IDLE = 0,		// idle
    UPDT_ACTION_VR = 1,		// version request
    UPDT_ACTION_AR = 2,		// address request
    UPDT_ACTION_DL = 4,		// downloading
    UPDT_ACTION_ERROR = 8		// an error occured
}action_t;

typedef struct{
    char name[256];
    char value[256];
}httpHeaderField_t;

typedef struct{
    double Http;
    int status;
    httpHeaderField_t *fields;
    int nFields;
}httpHeader_t;

typedef struct{
    int n;
    char **file;
}files_t;

enum flags_e{
    UPDT_FLAG_VERSION = 0,
    UPDT_FLAG_UPDATE = 1
};

typedef struct{
    int flags;
    int retries;
    action_t currAction;
    int buffsize;
    char *buff;
    files_t files;
    int filen;
    FILE *fp;
    qboolean firstPacket;
    long overalSize;
    long progress;
    int socket;
    qboolean update_restart;
}updateVars_t;
updateVars_t updt_vars;
qboolean UPDT_UpdateRestart()
{
    return updt_vars.update_restart;
}
qboolean UPDT_CopyFile(char *old, char *new)
{
    FILE *in = fopen(old,"r");
    FILE *out = fopen(new,"w");
    if(in==NULL || out==NULL)
	return qfalse;
    while(!feof(in)){
	fputc(fgetc(in),out);
    }
    if(ferror(in)!=0 || ferror(out)!=0)
	return qfalse;
    
    fclose(in);
    fclose(out);

}
qboolean UPDT_MoveFile(char *old, char *new)
{
    if(UPDT_CopyFile(old,new) && remove(old)){
	return qtrue;
    }
    return qfalse;
}
void UPDT_AbortUpdate()
{
    updt_vars.currAction = UPDT_ACTION_IDLE;
    if(updt_vars.buff!=NULL){
	//__asm__("int $3");
	free(updt_vars.buff);
	updt_vars.buff=NULL;
    }
    NET_TcpCloseConnection(&updt_vars.socket);
    //system("rm -rf tmp");
    return;
}

char *UPDT_GetFileName(char *url){
    int n;
    for(n = strlen(url)-1;n>=0;--n){
	if(url[n]=='/' || url[n]==':' || url[n]=='\\'){
	    return url + n + 1;
	}
    }
    return url;
}
qboolean UPDT_FileOK(){
    char buffer[128];
    sprintf(buffer,"tmp/%s",UPDT_GetFileName(updt_vars.files.file[updt_vars.filen]));
    FILE *f = fopen(buffer,"rb");
    if(f==NULL){
	return qfalse;
    }
    fseek(f,0,SEEK_END);
    if(ftell(f)==updt_vars.overalSize){
	fclose(f);
	return qtrue;
    }
    Com_DPrintf("File size: %d, declared size: %d.\n",ftell(f),updt_vars.overalSize);
    fclose(f);
    return qfalse;

}
int UPDT_ParseFilesList(char *data){

    char count[8];
    char *ptr,*ptr2;
    int n,i;

    if(updt_vars.files.file != NULL){
	for(i=0;i<updt_vars.files.n;++i){
	    if(updt_vars.files.file[i]==NULL){
		free(updt_vars.files.file[i]);
		updt_vars.files.file[i] = NULL;
	    }
	}
	free(updt_vars.files.file);
	updt_vars.files.file=NULL;
    }
    ptr = strchr(data,'\n');
    strncpy(count,data,ptr-data);
    count[ptr-data]=0;
    n = atoi(count);
    updt_vars.files.file = (char **)malloc(sizeof(char *)*n);
    memset(updt_vars.files.file,0x00,sizeof(char *)*n);
    updt_vars.files.n = n;
    Com_Printf("Parsing %d file names...\n",n);
    for(i=0;i<n;++i){
	ptr2 = ptr+1;
	ptr = strchr(ptr2,'\n');
	if(ptr==NULL)
	 break;
	updt_vars.files.file[i] = (char *)malloc(ptr-ptr2);
	strncpy(updt_vars.files.file[i],ptr2,ptr-ptr2);
	updt_vars.files.file[i][ptr-ptr2]=0;
    }
    return n;
}
char *UPDT_GetHeaderField(httpHeader_t *header,char *name)
{
    int i;
    for(i=0;i<header->nFields;++i){
	if(strcmp(header->fields[i].name,name)==0){
	    return header->fields[i].value;
	}
    }
    return NULL;

}
char *UPDT_StripHttpHeader(char *data,httpHeader_t *header)
{
    char *end = strstr(data,"\r\n\r\n")+4;
    char *line = data;
    char *lineEnd;
    char *tmp,*tmp2;
    char status[16]={0};
    int lines=0,i=0;
    Com_Printf("Debug:\n\n\n-----\n%s\n-----\n\n\n",data);
    while(line<end){
	line = strstr(line,"\r\n")+2;
	++lines;
    }
    if(header->fields!=NULL){
	//for(i=0;i<header->nFields;++i){
	//    free(header->fields+i);
	//}
	free(header->fields);
	header->fields = NULL;
    }
    lines -= 2;
    header->fields = (httpHeaderField_t *)malloc(sizeof(httpHeaderField_t)*lines);
    header->nFields = lines;
    
    line = data;
    
    
    lineEnd = strstr(line,"\r\n");
    tmp = strchr(line,' ');
    if(tmp==NULL || lineEnd == NULL){
        header->status = -1;
        return data;
        
    }
    tmp2 = strchr(tmp+1,' ')+1;
    if(tmp2-tmp > sizeof(status))
	strncpy(status,tmp,sizeof(status));
    else
	strncpy(status,tmp,tmp2-tmp);
    header->status = atoi(status);
    //Com_Printf("Debug: status: \"%s\", %d.\n",status,header->status);
    i=0;
    line = lineEnd + 2;
    while(line<end){
	lineEnd = strstr(line,"\r\n");
	//Com_Printf("DEBUG 1\n");
	if(line>=lineEnd)
	    break;
	tmp = strchr(line, ' ');
	//Com_Printf("DEBUG 1.5\n");
	if(tmp==NULL || lineEnd == NULL)
	    break;
	//Com_Printf("DEBUG 2\n");
	strncpy(header->fields[i].name,line,tmp-line);
	header->fields[i].name[tmp-line]=0;
	strncpy(header->fields[i].value,tmp+1,lineEnd-tmp-1);
	header->fields[i].value[lineEnd-tmp-1]=0;
	++i;
	*lineEnd = 0;
	Com_Printf("DEBUG: %d\n",line);
	line = lineEnd+2;
	
    }
    Com_Printf("Debug: Done parsing http header.\n");
    *(end-1)=0;
    //Com_Printf("\n\n---DEBUG---\n\n%s\n\n--- END ---\n\n",data);
    return end;
}
void UPDT_Update_f()
{
    if(updt_vars.currAction != UPDT_ACTION_IDLE){
	Com_Printf("^3An update is already in progress, please wait!\n");
	return;
    }
    int i;
    if(Cmd_Argc() > 1){
	int cmds = Cmd_Argc();
	for(i=1;i<cmds;++i){
	    if(Cmd_Argv(i)[0] != '-'){
		Com_Printf("Usage: %s.\n",UPDT_USAGE);
	    }
	    else{
		updt_vars.flags = 0;
		switch(Cmd_Argv(i)[1]){
		    case 'v':
			updt_vars.flags |= UPDT_FLAG_VERSION;
			break;
		    case 'u':
			updt_vars.flags |= UPDT_FLAG_UPDATE;
			break;
		}
	    }
	}
    }
    Com_Printf("Starting a system update...\n");
    Com_DPrintf("Allocating memory...\n");
    if(updt_vars.buff!=NULL){
	free(updt_vars.buff);
	updt_vars.buff = NULL;
    }
    updt_vars.buff = (char *)malloc(updt_vars.buffsize);
    if(updt_vars.buff==NULL){
	Com_PrintError("Allocating memory failed. Update aborted.\n");
	UPDT_AbortUpdate();
	return;
    }
    //Plugin_BoldPrintf(-1,"^2The server is updating...\n");
    if((updt_vars.socket = NET_TcpConnect("iceops.in:80\0"))<1){
	Com_PrintError("Failed to connect to the update server!");
	UPDT_AbortUpdate();
	return;
    }
    updt_vars.currAction = UPDT_ACTION_VR;
    sprintf(updt_vars.buff,UPDT_HTML_REQUEST,"/?mode=getversion");
    //Com_Printf("Request sent:\n\n\n%s\n\n\n",buff);
    NET_TcpSendData(&updt_vars.socket,updt_vars.buff,strlen(updt_vars.buff));
    Com_Printf("Waiting for version response...\n");
}
void UPDT_DoUpdate()
{
    Com_Printf("Starting server update...\n");
    //Plugin_BoldPrintf(-1,"^2The server is being updated, it will restart soon.\n");
    //Plugin_BoldPrintf(-1,"^2We are sorry for any inconvenience.\n");
    Com_DPrintf("Creating temporary directory...\n");
    mkdir("tmp",S_IRWXU | S_IRWXG | S_IRWXO);
    Com_DPrintf("Allocating memory...\n");
    if(updt_vars.buff==NULL)
	updt_vars.buff = (char *)malloc(updt_vars.buffsize);
    if(updt_vars.buff==NULL){
	Com_PrintError("Allocating memory failed. Update aborted.\n");
	UPDT_AbortUpdate();
	return;
    }
    Com_DPrintf("Allocated %gMB of memory for download buffer.\n",(double)updt_vars.buffsize/(double)(1024*1024));
    Com_DPrintf("Starting file download...\n");
    updt_vars.currAction = UPDT_ACTION_AR;
    //sprintf(buff,DOWNLOAD,REQUEST,file);
    //Plugin_TcpSendData(0,buff,strlen(buff));
    sprintf(updt_vars.buff,UPDT_HTML_REQUEST,"/?mode=getfiles");
    NET_TcpSendData(&updt_vars.socket,updt_vars.buff,strlen(updt_vars.buff));
}

void UPDT_ApplyUpdate()
{
    //char buffer[204800];
    //char buff2[256];
    int i;
    char file[256];
    //FILE *f = fopen("tmp_update.sh","w");
    if(updt_vars.buff!=NULL){
	free(updt_vars.buff);
	updt_vars.buff = NULL;
    }
    NET_TcpCloseConnection(&updt_vars.socket);
    //if(f==NULL)
    //	return;
    for(i=0;i<updt_vars.files.n;++i){
	sprintf(file,"%s.old",UPDT_GetFileName(updt_vars.files.file[i]));
	
	FS_CopyFile(UPDT_GetFileName(updt_vars.files.file[i]),file); // backup failed? that means this file is new ^^
	sprintf(file,"tmp/%s",UPDT_GetFileName(updt_vars.files.file[i]));
	FS_CopyFile(file,UPDT_GetFileName(updt_vars.files.file[i]));
	
    }
    updt_vars.update_restart = qtrue;
    Sys_Quit();
    //return;
    
    /*fprintf(f,"#!/bin/bash\nsleep 10;\n");
    for(i=0;i<updt_vars.files.n;++i){
	//if(files.file[i][0]=='/')
	fprintf(f,"cp -f tmp/%s %s;\n",UPDT_GetFileName(updt_vars.files.file[i]),UPDT_GetFileName(updt_vars.files.file[i]));
	//else
	//    sprintf(buff2,"cp -f tmp/%s %s;",files.file[i],files.file[i]);
	//strcat(buffer,buff2);
    }
    fprintf(f,"rm -rf tmp;\nchmod +x cod4_lnxded\n./cod4_lnxded");
    *//*for(int i=1;i<CommandLineC;++i){
	fprintf(f," %s",CommandLineV[i]);
	//strcat(buffer,buff2);
    }
    fprintf(f,"\nrm $0");
    //Com_Printf("Debug: system command: \"%s\"\n",buffer);
    fclose(f);
    system("chmod +x tmp_update.sh");
    system("./tmp_update.sh &");
    *(int *)(0x00) = 5;
    //system("sleep 10;");
    */
}
void UPDT_StartDownload()
{
    if(updt_vars.filen>=updt_vars.files.n){
	Com_Printf("Download complete.\n");
	UPDT_ApplyUpdate();
	updt_vars.currAction = UPDT_ACTION_IDLE;
	return;
    }
    updt_vars.currAction = UPDT_ACTION_DL;
    updt_vars.overalSize = 0;
    updt_vars.progress = 0;
    updt_vars.firstPacket = qtrue;
    sprintf(updt_vars.buff,"tmp/%s",UPDT_GetFileName(updt_vars.files.file[updt_vars.filen]));
    updt_vars.fp = fopen(updt_vars.buff,"wb");
    if(updt_vars.fp==NULL){
	Com_Printf("Failed to create a file \"%s\". Aborting...\n",updt_vars.buff);
	UPDT_AbortUpdate();
	return;
    }
    Com_Printf("Starting download of file %s...\n",UPDT_GetFileName(updt_vars.files.file[updt_vars.filen]));
    sprintf(updt_vars.buff,UPDT_HTML_REQUEST,updt_vars.files.file[updt_vars.filen]);
    NET_TcpSendData(&updt_vars.socket,updt_vars.buff,strlen(updt_vars.buff));
}
int UPDT_Init(){
	//	Load function pointers, the following 2 lines of code should not be changed.
	//if(GetFunctions(mainFunctions)==-1)
	//	return -1;	// -1 => Initialization failed, unload the plugin.
	//	Function pointers loaded, add your plugin's initialization content here.
	//Plugin_AddCommand("sys_update",(void *)Update_f,100);
	Cmd_AddCommand("sys_update",UPDT_Update_f);
	updt_vars.flags = 0;
	updt_vars.filen = 0;
	updt_vars.retries = 0;
	updt_vars.files.file = NULL;
	updt_vars.currAction = UPDT_ACTION_IDLE;
	updt_vars.buff = NULL;
	updt_vars.buffsize = 24 * 1024 * 1024;
	updt_vars.update_restart = qfalse;
	Com_DPrintf("^3Updater initialized soccessfully.\n");
	return 0;	// 0 => Initialization successfull.
}
qboolean UPDT_DoFinalize(char *cmdline)
{
    if(!UPDT_UpdateRestart())
	return qfalse;
    int size = &UPDT_CODE_END - &UPDT_CODE_START;
    long pageSize = sysconf(_SC_PAGESIZE);
    void *ptr;
    char **names;
    //char *cmdline;
    char name[128];
    int i,j;
    if(size<=0 || pageSize <= 0)
	return qfalse;

    posix_memalign(&ptr,pageSize,size);

    if(ptr==NULL)
	return qfalse;

    memcpy(&UPDT_CODE_START,ptr,size);

    if(mprotect(ptr,size,PROT_READ | PROT_EXEC) != 0)
	return qfalse;

    // the code is on the heap, memory is executable. Prepare the filenames array
    names = (char **)malloc(sizeof(char **) * updt_vars.files.n * 4);

    for(i=0,j=0;i<updt_vars.files.n;++i){
	sprintf(name,"%s",UPDT_GetFileName(updt_vars.files.file[i]));
	names[j++] = (char *)malloc(sizeof(char)*strlen(name));

	sprintf(name,"%s.old",UPDT_GetFileName(updt_vars.files.file[i]));
	names[j++] = (char *)malloc(sizeof(char)*strlen(name));

	// ---

	sprintf(name,"tmp/%s",UPDT_GetFileName(updt_vars.files.file[i]));
	names[j++] = (char *)malloc(sizeof(char)*strlen(name));
	
	sprintf(name,"%s",UPDT_GetFileName(updt_vars.files.file[i]));
	names[j++] = (char *)malloc(sizeof(char)*strlen(name));
    }
    // filenames array ready
    // all ready for the big moment! ;P time to call our assembly function
    UPDT_SwapAndExec("cod4_lnxded",cmdline,updt_vars.files.n * 4,names);
}
void UPDT_Finalize()
{
    char *cmdline;
    int i,len;
    // prepare the commandline
    len = 1;
    for(i=1;i<SYSTEM_ARGC;++i){
	len += strlen(SYSTEM_ARGV[i]) + 1; // +1 for ' '
    }

    cmdline = (char *)malloc(sizeof(char)*len);
    *cmdline = 0;
    for(i=1;i<SYSTEM_ARGC;++i){
	if(i > 1)
	    strcat(cmdline, " ");
	strcat(cmdline, SYSTEM_ARGV[i]);
    }
    
    if(!UPDT_DoFinalize(cmdline))
	execvp("cod4_lnxded",cmdline);

}
void UPDT_OnFrame()
{
    if(updt_vars.currAction==UPDT_ACTION_IDLE){
	return;
    }
    int bytes;
    int tmp,i,j;
    char sbuff[32];
    char *data;
    char *cptr,*cptr2;
    httpHeader_t header;
    header.nFields=0;
    header.fields=NULL;
    bytes = NET_TcpGetData(&updt_vars.socket,updt_vars.buff,updt_vars.buffsize);
    if(bytes==0){
	return;
    }
    else if(bytes<0){
	updt_vars.currAction = UPDT_ACTION_IDLE;
	Com_Printf("Updater: lost connection. Update aborted.\n");
	UPDT_AbortUpdate();
	return;
    }
    Com_Printf("Updater: a packet has arrived.\n");
    data = UPDT_StripHttpHeader(updt_vars.buff,&header);
    Com_Printf("Debug: Status: %d.\nDebug: Header fileds parsed: %d.\n",header.status,header.nFields);
    for(j=0;j<header.nFields;++j){
	Com_Printf("Field #%d: Name: \"%s\", Value: \"%s\".\n",j,header.fields[j].name,header.fields[j].value);
	
    }
    switch(updt_vars.currAction){
	case UPDT_ACTION_VR:
	    //Plugin_TcpGetData(0,buff,buffsize);
	    tmp = atoi(data);
	    Com_Printf("Server build: %d, newest build: %d.\n",BUILD_NUMBER,tmp);
	    if(tmp>BUILD_NUMBER){
		Com_Printf("A new version is available! Update process will stop the server and restart it after the process.\n");
		if(updt_vars.flags && UPDT_FLAG_UPDATE){
		    UPDT_DoUpdate();
		    break;
		}
		else{
		    Com_Printf("If you want to continue type in \"sys_update -u\".\n");
		    UPDT_AbortUpdate();
		    break;;
		}
		//Plugin_AddCommand("yes",(void *)DoUpdate,100);
		//Plugin_AddCommand("no",(void *)DoUpdate,100);
	    }
	    else{
		Com_Printf("Server is already running the newest version.\n");
		UPDT_AbortUpdate();
	    }
	    break;
	case UPDT_ACTION_AR:
	    //Com_Printf("Debug: Files to download:\n%s\n",data);
	    UPDT_ParseFilesList(data);
	    Com_Printf("Files to download:\n");
	    for(i=0;i<updt_vars.files.n;++i){
		Com_Printf("%d: %s\n",i+1,updt_vars.files.file[i]);
	    }
	    updt_vars.currAction = UPDT_ACTION_DL;
	    UPDT_StartDownload();
	    break;
	case UPDT_ACTION_DL:
	    if(updt_vars.firstPacket){
		updt_vars.firstPacket = qfalse;
		//cptr = strstr(updt_vars.buff,"Content-Length: ");
		cptr = UPDT_GetHeaderField(&header,"Content-Length:");
		if(cptr==NULL){
		    Com_Printf("Download failed :(.\n");
		    if(updt_vars.retries++<UPDT_MAX_RETRIES){
		        Com_Printf(" Retrying...\n");
		        UPDT_StartDownload();
		
		    }
		    else{
		        Com_Printf(" Retry limit reached. Aborting...\n");
		        UPDT_AbortUpdate();
		        break;
		    }
		    break;
		}
		//cptr+=16;
		//cptr2 = strchr(cptr,'\r');
		/*if(cptr2 <= cptr || cptr2-cptr>sizeof(sbuff)){
		    Com_Printf("^1ERROR: Downloaded corrupt data! Aborting update...\n");
		    NET_TcpCloseConnection(&updt_vars.socket);
		    UPDT_AbortUpdate();
		    break;
		}*/
		//strncpy(sbuff,cptr,cptr2-cptr);
		//sbuff[cptr2-cptr]=0;
		updt_vars.overalSize = atoi(cptr);
		Com_Printf("Size of %s: %gkB\n",UPDT_GetFileName(updt_vars.files.file[updt_vars.filen]),(double)updt_vars.overalSize/1024);
	    }
		if(data-updt_vars.buff>=bytes){
		    Com_Printf("^2Warning: Recieved corrupt data!\n");
		    if(updt_vars.retries++<UPDT_MAX_RETRIES){
		        Com_Printf(" Retrying...\n");
		        UPDT_StartDownload();
		
		    }
		    else{
		        Com_Printf(" Retry limit reached. Aborting...\n");
		        UPDT_AbortUpdate();
		    }
		    break;
		}
		updt_vars.progress += bytes - (data-updt_vars.buff);
		if(fwrite(data,bytes - (data-updt_vars.buff),1,updt_vars.fp)!=1){
		    Com_Printf("^1ERROR: Error writing to file. Error code: %d. Aborting update...\n",ferror(updt_vars.fp));
		    UPDT_AbortUpdate();
		    break;
		}
		if(updt_vars.progress >= updt_vars.overalSize){
		    fclose(updt_vars.fp);
		    if(UPDT_FileOK()){
			Com_Printf("File %s downloaded successfully.\n",UPDT_GetFileName(updt_vars.files.file[updt_vars.filen]));
			++updt_vars.filen;
			updt_vars.retries = 0;
			UPDT_StartDownload();
		    }
		    else
		    {
			Com_Printf("File %s is corrupted.",UPDT_GetFileName(updt_vars.files.file[updt_vars.filen]));
			if(updt_vars.retries++<UPDT_MAX_RETRIES){
			    Com_Printf(" Retrying...\n");
			    UPDT_StartDownload();
			    
			}
			else{
			    Com_Printf(" Retry limit reached. Aborting...\n");
			    UPDT_AbortUpdate();
			    break;
			}
		    }
		}
	    break;
	default:
		Com_PrintError("Updater: unknown status. The heap is corrupted!\n");
	    break;
    }
    if(header.fields!=NULL){
	//for(i=0;i<header.nFields;++i){
	//    free(&(header.fields[i]));
	//}
	free(header.fields);
	header.fields = NULL;
    }
}
