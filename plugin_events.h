//typedef char[32] plugin_event_t;

enum PluginEvents{
    // plugin handling events
    
    //PLUGINS_ONINIT,
    PLUGINS_ONINFOREQUEST,
    
    // runtime events:

    PLUGINS_ONPLAYERDC,
    PLUGINS_ONPLAYERCONNECT,
    PLUGINS_ONEXITLEVEL,
    PLUGINS_ONMESSAGESENT,
    PLUGINS_ONFRAME,		//new
    PLUGINS_ONONESECOND,	// new
    PLUGINS_ONTENSECONDS,
    PLUGINS_ONCLIENTAUTHORIZED,	// new
    PLUGINS_ONCLIENTSPAWN,
    PLUGINS_ONCLIENTENTERWORLD,
    PLUGINS_ONTCPSERVERPACKET,	// new
    PLUGINS_ONUDPNETEVENT,
    PLUGINS_ONUDPNETSEND,
    PLUGINS_ONSPAWNSERVER,
    PLUGINS_ONPREFASTRESTART,
    PLUGINS_ONPOSTFASTRESTART,
    PLUGINS_ITEMCOUNT

};

char Plugin_Events[PLUGINS_ITEMCOUNT][32]={

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

void Plugin_Event(int, ...);