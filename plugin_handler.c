/*
===========================================================================
    Copyright (C) 2010-2013  Ninja and TheKelm of the IceOps-Team

    This file is part of CoD4X17a-Server source code.

    CoD4X17a-Server source code is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as
    published by the Free Software Foundation, either version 3 of the
    License, or (at your option) any later version.

    CoD4X17a-Server source code is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>
===========================================================================
*/



#include "plugin_handler.h"
#include "elf32_parser.h"

/*=========================================*
 *                                         *
 *       Plugin Handler's main file        *
 *                                         *
 *   Functions in this file are the most   *
 *     important for Plugin Handler.       *
 *    Those functions are safe for use     *
 *  anywhere in the server's source code.  *
 *                                         *
 *=========================================*/


pluginWrapper_t pluginFunctions;

char PHandler_Events[PLUGINS_ITEMCOUNT][32]={

    "OnInfoRequest",
    
    "OnPlayerDC",
    "OnPlayerConnect",
    "OnExitLevel",
    "OnMessageSent",
    "OnFrame",
    "OnOneSecond",
    "OnTenSeconds",
    "OnClientAuthorized",
    "OnClientSpawn",
    "OnClientEnterWorld",
    "OnTcpServerPacket",
    "OnUdpNetEvent",
    "OnUdpNetSend",
    "OnSpawnServer",
    "OnPreFastRestart",
    "OnPostFastRestart"
};

void PHandler_Init() // Initialize the Plugin Handler's data structures and add commands
{
    pluginFunctions.loadedPlugins=0;
    pluginFunctions.enabled=qfalse;

    memset(&pluginFunctions,0x00,sizeof(pluginFunctions));    // 0 all data
    pluginFunctions.enabled=qtrue;

    Cmd_AddCommand("loadPlugin", PHandler_LoadPlugin_f);
    Cmd_AddCommand("unloadPlugin", PHandler_UnLoadPlugin_f);
    Cmd_AddCommand("plugins", PHandler_PluginList_f);
    Cmd_AddCommand("pluginInfo", PHandler_PluginInfo_f);
    
    Com_Printf("PHandler_Init: Plugins initialization successfull.\n");
}
void PHandler_Load(char* name, size_t size) // Load a plugin, safe for use
{
    int i,j,nstrings;
    char dll[256],*strings;
    char* realpath;
    void *lib_handle;
    char *error;
    elf_data_t text;
    pluginInfo_t info;

        if(!pluginFunctions.enabled){
            Com_Printf("Plugin handler is not initialized!\n");
            return;

        }
    if(pluginFunctions.loadedPlugins>=MAX_PLUGINS-1){
        Com_Printf("Too many plugins loaded.");
        return;
    }

    if(size>128){
        Com_Printf("File name too long.");
        return;
    }
    Com_DPrintf("Checking if the plugin is not already loaded...\n");
    //    Check if the plugin is not already loaded...
    for(i=0;i<MAX_PLUGINS;i++){
        if(strcmp(name,pluginFunctions.plugins[i].name)==0){
            Com_Printf("This plugin is already loaded!\n");
            return;
        }
    }
    Com_DPrintf("Checking if the plugin file exists and is of correct format...\n");
    sprintf(dll,"plugins/%s.so",name);
    //Additional test if a file is there
    realpath = FS_SV_GetFilepath( dll );
    if(realpath == NULL)
    {
        Com_Printf("No such file found: %s. Can not load this plugin.\n", dll);
        return;
    }
    //Parse the pluginfile and extract function names string table
    nstrings = ELF32_GetStrTable(realpath,&strings,&text);
    if(!nstrings){
        Com_Printf("%s is not a plugin file or is corrupt.\n",dll);
        return;
    }
    Com_DPrintf("Parsing plugin function names...\n");
    --nstrings;
    for(i = 0,j=0 ;i<nstrings;++i){
        if(strings[i]==0){
            if(strcmp(strings+i+1,"malloc")==0 || strcmp(strings+i+1,"calloc")==0 || strcmp(strings+i+1,"realloc")==0 || strcmp(strings+i+1,"free")==0 || strcmp(strings+i+1,"printf")==0 || strcmp(strings+i+1,"scanf")==0 ||  strcmp(strings+i+1,"free")==0){ // malloc, calloc, realloc, free, printf, scanf
                Com_Printf("The plugin file contains one of the disallowed functions! Disallowed function name: \"%s\".\nPlease refer to the documentation for details.\nPlugin load failed.\n",strings+i+1);
                
                free(strings);
                return;
            }
            if(strncmp(strings+i+1,"_Znaj",5)==0 || strncmp(strings+i+1,"_Znwj",5)==0){ // new and new[]
                Com_Printf("The plugin file contains C++'s new operator which is forbidden.\nPlease refer to the documentation for details.\nPlugin load failed.\n");
                free(strings);
                return;
            }
        }
        else
        i+=strlen(strings+i+1);
    }
    free(strings);
    Com_DPrintf("Done parsing plugin function names.\n");
    dlerror(); // Clear errors (if any) before loading the .so
    Com_DPrintf("Loading the plugin .so...\n");
    lib_handle = dlopen(realpath, RTLD_NOW);
    error = dlerror();
    if (!lib_handle || error != NULL){
        Com_PrintError("Failed to load the plugin! Error string: '%s'.\n",dll,error);
        return;
    }
    Com_DPrintf("Plugin OK! Loading...\n");
    // find first free plugin slot
    for(i=0;i<MAX_PLUGINS;i++){
        if(!(pluginFunctions.plugins[i].loaded))
            break;
    }
    pluginFunctions.plugins[i].OnInit = dlsym(lib_handle, "OnInit");
    for(j=0;j<PLUGINS_ITEMCOUNT;++j){
        pluginFunctions.plugins[i].OnEvent[j] = dlsym(lib_handle,PHandler_Events[j]);
    
    }
    pluginFunctions.plugins[i].OnInfoRequest = pluginFunctions.plugins[i].OnEvent[PLUGINS_ONINFOREQUEST];
    
    pluginFunctions.plugins[i].OnUnload = dlsym(lib_handle, "OnUnload");
    
    dlerror();    //    Just clear the errors, if the function was not found then we have a NULL pointer - thats what we want

    pluginFunctions.plugins[i].loaded = qtrue;
    pluginFunctions.plugins[i].enabled = qtrue;
    strcpy(pluginFunctions.plugins[i].name,name);
    pluginFunctions.initializing_plugin = qtrue;
    
    pluginFunctions.plugins[i].lib_start = LIBRARY_ADDRESS_BY_HANDLE(lib_handle) + text.offset;;
            pluginFunctions.plugins[i].lib_size = text.size;
    
    if(pluginFunctions.plugins[i].OnInit==NULL){
        Com_Printf("Error loading plugin's OnInit function.\nPlugin load failed.\n");
        return;
    }
    Com_DPrintf("Executing plugin's OnInit...\n");
    if((*pluginFunctions.plugins[i].OnInit)(/*mainFunctions*/)<0){
        Com_Printf("Error in plugin's OnInit function!\nPlugin load failed.\n");
        pluginFunctions.plugins[i].loaded = qfalse;
        pluginFunctions.initializing_plugin = qfalse;
        memset(pluginFunctions.plugins + i,0x00,sizeof(plugin_t));    // We need to remove all references so we can dlclose.
        dlclose(lib_handle);
        return;
    }
    else{
        Com_Printf("Plugin's OnInit executed successfully!\n");
        //    Save info about the loaded plugin
        pluginFunctions.initializing_plugin = qfalse;
        pluginFunctions.loadedPlugins++;
        pluginFunctions.plugins[i].lib_handle = lib_handle;

        if(pluginFunctions.plugins[i].OnInfoRequest){
            Com_DPrintf("Fetching plugin information...\n");
            (*pluginFunctions.plugins[i].OnInfoRequest)(&info);
            if(info.handlerVersion.major != PLUGIN_HANDLER_VERSION_MAJOR || info.handlerVersion.minor > PLUGIN_HANDLER_VERSION_MINOR || (info.handlerVersion.minor - PLUGIN_HANDLER_VERSION_MINOR) > 100){
                Com_PrintError("^1ERROR:^7 This plugin might not be compatible with this server version! Requested plugin handler version: %d.%d, server's plugin handler version: %d.%d. Unloading the plugin...\n",info.handlerVersion.major,info.handlerVersion.minor, PLUGIN_HANDLER_VERSION_MAJOR,PLUGIN_HANDLER_VERSION_MINOR);
                PHandler_Unload(i);
            return;
            }
            
            
            Com_Printf("Plugin %s loaded successfully. Server is currently running %d plugins.\n",pluginFunctions.plugins[i].name,pluginFunctions.loadedPlugins);
        }
        else{
            Com_Printf("^1ERROR:^7 function OnInfoRequest not found in the plugin file. Unloading...\n");
            PHandler_Unload(i);
            return;

        }
        Com_Printf("Plugin %s loaded successfully. Server is currently running %d plugins.\n",pluginFunctions.plugins[i].name,pluginFunctions.loadedPlugins);
        return;
    }


}
void PHandler_Unload(int id) // Unload a plugin, safe for use.
{
    void *lib_handle;
    int i;
    if(pluginFunctions.plugins[id].loaded){
        if(pluginFunctions.plugins[id].exports != 0){ // Library-plugins cannot be unloaded, see plugins/readme.txt
            Com_PrintError("PHandler_Unload: Cannot unload a library plugin!\n");
            return;
        }
        if(pluginFunctions.plugins[id].scriptfunctions != 0 || pluginFunctions.plugins[id].scriptmethods != 0){
            // Script-library plugins cannot be unloaded, see plugins/readme.txt
            Com_PrintError("PHandler_Unload: Cannot unload a script-library plugin!\n");
            return;
        }
        if(pluginFunctions.plugins[id].OnUnload != NULL)
            (*pluginFunctions.plugins[id].OnUnload)();
        // Remove all server commands of the plugin
        for(i=0;i<pluginFunctions.plugins[id].cmds;i++){
            if(pluginFunctions.plugins[id].cmd[i].xcommand!=NULL){
                Com_DPrintf("Removing command \"%s\"...\n",pluginFunctions.plugins[id].cmd[i].name);
                Cmd_RemoveCommand(pluginFunctions.plugins[id].cmd[i].name);
            }

        }
        lib_handle = pluginFunctions.plugins[id].lib_handle;                // Save the lib handle
        memset(&(pluginFunctions.plugins[id]), 0x00, sizeof(plugin_t));     // Wipe out all the data
        dlclose(lib_handle);                                                // Close the dll as there are no more references to it
        --pluginFunctions.loadedPlugins;
    }else{
        Com_Printf("Tried unloading a not loaded plugin!\nPlugin ID: %d.",id);
    }
}
int PHandler_GetID(char *name, size_t size) // Get ID of a plugin by name, safe for use
{
    int i;
    for(i=0;i<MAX_PLUGINS;i++){
        if(strcmp(name,pluginFunctions.plugins[i].name)==0){
            return i;
        }
    }
    return PLUGIN_UNKNOWN;
}


void PHandler_UnloadByName(char *name, size_t size) // Unload a plugin, safe for use
{
    int id = PHandler_GetID(name,size);
    if(id<0)
        Com_Printf("Cannot unload plugin: plugin %s is not loaded!\n",name);
    else{
        Com_Printf("Unloading plugin %s, plugin id: %d.\n",name,id);
        PHandler_Unload(id);

    }
}


void PHandler_Event(int eventID,...) // Fire a plugin event, safe for use
{
    int i=0;

    if(!pluginFunctions.enabled)
            return;

    if(eventID < 0 || eventID >= PLUGINS_ITEMCOUNT){
        Com_DPrintf("Plugins: unknown event occured! Event ID: %d.\n",eventID);
        return;
    }

    va_list argptr;

    va_start(argptr, eventID);
    void *arg_0 = va_arg(argptr, void*);
    void *arg_1 = va_arg(argptr, void*);
    void *arg_2 = va_arg(argptr, void*);
    void *arg_3 = va_arg(argptr, void*);
    void *arg_4 = va_arg(argptr, void*);
    void *arg_5 = va_arg(argptr, void*);

    va_end(argptr);

    for(i=0;i < pluginFunctions.loadedPlugins; i++){
        if(pluginFunctions.plugins[i].OnEvent[eventID]!= NULL)
            (*pluginFunctions.plugins[i].OnEvent[eventID])(arg_0, arg_1, arg_2, arg_3, arg_4, arg_5);
    }
}


/*

P_P_F void PHandler_ScrAddFunction(char *name,xcommand_t xcommand)
{
    volatile int pID;
    pID = PHandler_CallerID();
    if(pID>=MAX_PLUGINS){
        Com_Printf("Error: tried adding a script command for a plugin with non existent pID. pID supplied: %d.\n",pID);
        return;
    }else if(pID<0){
        Com_Printf("Plugin Handler error: Plugin_ScrAddFunction called from not within a plugin or from a disabled plugin!\n");
        return;
    }
    if(!pluginFunctions.plugins[pID].loaded){
        Com_Printf("Error: Tried adding a command for not loaded plugin! PID: %d.\n",pID);
    }
    Com_Printf("Adding a plugin script function for plugin %d, command name: %s.\n",pID,name);
    Cmd_AddCommand(name, xcommand);
    pluginFunctions.plugins[pID].cmd[pluginFunctions.functions[pID].cmds].xcommand = xcommand;
    strcpy(pluginFunctions.plugins[pID].cmd[pluginFunctions.plugins[pID].cmds++].name,name);
    Com_Printf("Command added.\n");
   // pluginFunctions.plugins[pID].


}
void PHandler_ScrRemoveFunction(int pID,char *name)
{
    int i,j,k;
    j=pluginFunctions.plugins[pID].cmds;
    for(i=0;i<j;i++){
        if(strcmp(name,pluginFunctions.plugins[pID].cmd[i].name)==0){
            Cmd_RemoveCommand(name);
            memset(pluginFunctions.plugins[pID].cmd,0x00,sizeof(pluginCmd_t));
            // Now we need to rearrrange the array...
            for(k=i;k<j-1;k++){
                pluginFunctions.plugins[pID].cmd[k] = pluginFunctions.plugins[pID].cmd[k+1];

            }
            Com_Printf("Command '%s' removed for plugin %s.\n",name,pluginFunctions.plugins[pID].name);
            return;
        }

    }
    Com_Printf("Warning: tried removing command '%s', which was not found for plugin %s.\n",name,pluginFunctions.plugins[pID].name);

}
*/




