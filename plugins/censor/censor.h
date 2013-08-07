void G_SayCensor_Init(void);
char* G_SayCensor(char *msg);


#define DLL_PUBLIC __attribute__ ((visibility ("default"))) __attribute__ ((cdecl))
#define DLL_LOCAL __attribute__ ((visibility ("hidden")))
