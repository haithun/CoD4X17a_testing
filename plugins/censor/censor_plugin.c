//#include "../declarations.h"
//#include "../functions.c"
#include "../pinc.h"
#include "censor.h"

DLL_PUBLIC int OnInit(mainFunctions_t mainFunctions){	// Funciton called on server initiation
	//	Load function pointers
	if(GetFunctions(mainFunctions)==-1)
		return -1;
	//	Function pointers loaded, add your plugin's content here
	G_SayCensor_Init();
	
	return 0;
}
DLL_PUBLIC void OnMessageSent(char *message){
	G_SayCensor(message);
}
DLL_PUBLIC void OnInfoRequest(pluginInfo_t *info){	// Function used to obtain information about the plugin
    // Memory pointed by info is allocated by the server binary, just fill in the fields
    
    // =====  MANDATORY FIELDS  =====
    info->handlerVersion = 1.01;	// Requested handler version
    
    // =====  OPTIONAL  FIELDS  =====
    info->pluginVersion = 1.1;	// Plugin version
    strncpy(info->fullName,"IceOps message censoring plugin by TheKelm",sizeof(info->fullName)); //Full plugin name
    strncpy(info->shortDescription,"This plugin is used to censor the ingame chat from swears.",sizeof(info->shortDescription)); // Short plugin description
    strncpy(info->longDescription,"This plugin is used to censor the ingame chat from swears. The words to be censored should be put in 'badwords.txt' in the server directory.\n\nCopyright (c) 2013 IceOps.\n\nVisit us at www.iceops.in",sizeof(info->longDescription));
}