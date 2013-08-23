#include "../pinc.h"
#include "censor.h"

PCL int OnInit(){	// Funciton called on server initiation

	G_SayCensor_Init();
	
	return 0;
}
PCL void OnMessageSent(char *message){
	G_SayCensor(message);
}
PCL void OnInfoRequest(pluginInfo_t *info){	// Function used to obtain information about the plugin
    // Memory pointed by info is allocated by the server binary, just fill in the fields
    
    // =====  MANDATORY FIELDS  =====
    info->handlerVersion.major = PLUGIN_HANDLER_VERSION_MAJOR;
    info->handlerVersion.minor = PLUGIN_HANDLER_VERSION_MINOR;	// Requested handler version
    
    // =====  OPTIONAL  FIELDS  =====
    info->pluginVersion.major = 2;
    info->pluginVersion.minor = 0;	// Plugin version
    strncpy(info->fullName,"IceOps message censoring plugin by TheKelm",sizeof(info->fullName)); //Full plugin name
    strncpy(info->shortDescription,"This plugin is used to censor the ingame chat from swears.",sizeof(info->shortDescription)); // Short plugin description
    strncpy(info->longDescription,"This plugin is used to censor the ingame chat from swears. The words to be censored should be put in 'badwords.txt' in the server directory.\n\nCopyright (c) 2013 IceOps.\n\nVisit us at www.iceops.in",sizeof(info->longDescription));
}
