/*
============================
    Plugin INClude file
  This contains plugin lib
 This file must be included
============================
*/

#include "../pinc.h"

/*
==================
  Other includes
==================
*/
#include <cstring>
#include <string>

/*
===========================
     OnInit callback
 This is a function called
  right after the plugin
        is loaded.
===========================
*/
PCL int OnInit(mainFunctions_t mainFunctions){
	//	Load function pointers, the following 2 lines of code should not be changed.
	if(GetFunctions(mainFunctions)==-1)
		return -1;	// -1 => Initialization failed, unload the plugin.
	//	Function pointers loaded, add your plugin's initialization content here.
	std::string *s = new std::string();
	return 0;	// 0 => Initialization successfull.
}
/*
=========================
    Event callbacks
 Those are not mandatory
=========================
*/
PCL void OnPlayerConnect(int slotNum);
PCL void OnPlayerDC(int slotNum);
PCL void OnExitLevel();
PCL void OnMessageSent(char *message);
PCL void OnTenSeconds();
PCL void OnFrame();
PCL void OnClientAuthorized();
PCL void OnClientSpawn();
PCL void OnClietnEnterWorld();
PCL void TCPServerPacket();

/*
============================
   OnInfoRequest callback
 This function is mandatory
============================
*/

PCL void OnInfoRequest(pluginInfo_t *info){	// Function used to obtain information about the plugin
    // Memory pointed by info is allocated by the server binary, just fill in the fields
    
    // =====  MANDATORY FIELDS  =====
    info->handlerVersion = PLUGIN_HANDLER_VERSION;	// Requested handler version, we request the version compatible with this plugin lib
    
    // =====  OPTIONAL  FIELDS  =====
    info->pluginVersion = 1.0;	// Plugin version
    strncpy(info->fullName,"An example C++ plugin.",sizeof(info->fullName)); //Full plugin name
    strncpy(info->shortDescription,"This is the plugin's short description.",sizeof(info->shortDescription)); // Short plugin description
    strncpy(info->longDescription,"This is the plugin's long description.",sizeof(info->longDescription));
}