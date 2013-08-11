qboolean Plugin_ExportFunction_p(char *name, void *(*function)());
void *Plugin_ImportFunction_p(char *pluginName, char *name);
qboolean Plugin_IsLoaded_p(char *name);