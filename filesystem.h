#ifndef __FILESYSTEM_H__
#define __FILESYSTEM_H__

#include "q_shared.h"


#define fs_game getcvaradr(0x13f9da18)
#define fs_debug getcvaradr(0x13f9da00)
#define fs_basepath getcvaradr(0x13f9da08)
#define fs_homepath getcvaradr(0x13f9da04)
#define getcvaradr(adr) ((cvar_t*)(*(int*)(adr)))


#define fsh_ADDR 0x13f9da40
#define fs_loadStack_ADDR 0x13f9d8e4
#define fs_gamedir_ADDR 0x13f9d900

#define fsh ((fileHandleData_t*)(fsh_ADDR))
#define fs_loadStack (*((int*)(fs_loadStack_ADDR)))
#define fs_gamedir ((char*)(fs_gamedir_ADDR))

#define fs_searchpaths (searchpath_t*)*((int*)(0x13f9da28))

#define	DEMOGAME			"demota"

// every time a new demo pk3 file is built, this checksum must be updated.
// the easiest way to get it is to just run the game and see what it spits out
#define	DEMO_PAK_CHECKSUM	437558517u

// if this is defined, the executable positively won't work with any paks other
// than the demo pak, even if productid is present.  This is only used for our
// last demo release to prevent the mac and linux users from using the demo
// executable with the production windows pak before the mac/linux products
// hit the shelves a little later
// NOW defined in build files
//#define PRE_RELEASE_TADEMO


// referenced flags
// these are in loop specific order so don't change the order
#define FS_GENERAL_REF	0x01
#define FS_UI_REF		0x02
#define FS_CGAME_REF	0x04
#define FS_QAGAME_REF	0x08

#define MAX_ZPATH	256
#define	MAX_SEARCH_PATHS	4096
#define MAX_FILEHASH_SIZE	1024
//#define MAX_OSPATH 256
#define MAX_FILE_HANDLES 48

typedef int	fileHandle_t;
typedef void*	unzFile;

// mode parm for FS_FOpenFile
typedef enum {
	FS_READ,
	FS_READ_LOCK,
	FS_WRITE,
	FS_WRITE_LOCK,
	FS_APPEND,
	FS_APPEND_LOCK,
	FS_APPEND_SYNC
} fsMode_t;

typedef enum {
	FS_SEEK_CUR,
	FS_SEEK_END,
	FS_SEEK_SET
} fsOrigin_t;


typedef struct fileInPack_s {
	char			*name;		// name of the file
	unsigned long		pos;		// file info position in zip
	struct	fileInPack_s*	next;		// next file in the hash
} fileInPack_t;

typedef struct {	//Verified
	char			pakFilename[MAX_OSPATH];	// c:\quake3\baseq3\pak0.pk3
	char			pakBasename[MAX_OSPATH];	// pak0
	char			pakGamename[MAX_OSPATH];	// baseq3
	unzFile			handle;						// handle to zip file +0x300
	int			checksum;					// regular checksum
	int			pure_checksum;				// checksum for pure
	int			numfiles;					// number of files in pk3
	int			referenced;					// referenced file flags
	int			hashSize;					// hash table size (power of 2)		+0x318
	fileInPack_t*	*hashTable;					// hash table	+0x31c
	fileInPack_t*	buildBuffer;				// buffer with the filenames etc. +0x320
} pack_t;

typedef struct {	//Verified
	char		path[MAX_OSPATH];		// c:\quake3
	char		gamedir[MAX_OSPATH];	// baseq3
} directory_t;

typedef struct searchpath_s {	//Verified
	struct searchpath_s *next;

	pack_t		*pack;		// only one of pack / dir will be non NULL
	directory_t	*dir;
} searchpath_t;

typedef union qfile_gus {
	FILE*		o;
	unzFile	z;
} qfile_gut;

typedef struct qfile_us {
	qfile_gut	file;
	qboolean	unique;
} qfile_ut;

//Added manual buffering as the stdio file buffering has corrupted the written files

typedef struct {
	qfile_ut	handleFiles;
	union{
		qboolean	handleSync;
		void*		writebuffer;
	};
	union{
		int	baseOffset;
		int	bufferSize;
	};
	union{
		int	zipFilePos;
		int	bufferPos; //For buffered writes
	};
	qboolean	zipFile;
	qboolean	streamed;
	char		name[MAX_ZPATH];
} fileHandleData_t; //0x11C (284) Size


// TTimo - https://zerowing.idsoftware.com/bugzilla/show_bug.cgi?id=540
// wether we did a reorder on the current search path when joining the server

// last valid game folder used
char lastValidBase[MAX_OSPATH];
char lastValidGame[MAX_OSPATH];

#ifdef FS_MISSING
FILE*		missingFiles = NULL;
#endif

void FS_CopyFile(char* FromOSPath,char* ToOSPath);
int FS_Read(void* data, int length, fileHandle_t);
int FS_FOpenFileRead(const char* filename,fileHandle_t* returnhandle);
fileHandle_t FS_FOpenFileWrite(const char* filename);
fileHandle_t FS_FOpenFileAppend(const char* filename);
qboolean FS_Initialized();
int FS_filelength( fileHandle_t f );

unzFile unzOpen(const char* path);
int unzOpenCurrentFile(unzFile file);
int unzSetOffset(unzFile file, unsigned long pos);
int unzReadCurrentFile(unzFile file, void *buf, unsigned len);
int unzClose( unzFile file );
int unzCloseCurrentFile( unzFile file );

char *FS_BuildOSPath( const char *base, const char *game, const char *qpath );
qboolean FS_HomeRemove( const char *path );
qboolean FS_SV_HomeRemove( const char *path );

qboolean FS_FileExists( const char *file );

char* FS_SV_GetFilepath( const char *file );
void FS_Rename( const char *from, const char *to );
void FS_SV_Rename( const char *from, const char *to );
qboolean FS_FCloseFile( fileHandle_t f );
qboolean FS_FilenameCompare( const char *s1, const char *s2 );
char *FS_ShiftedStrStr(const char *string, const char *substring, int shift);
long FS_fplength(FILE *h);

qboolean FS_IsExt(const char *filename, const char *ext, int namelen);

void FS_ConvertPath( char *s );

int FS_PathCmp( const char *s1, const char *s2 );
int	FS_FTell( fileHandle_t f );
void	FS_Flush( fileHandle_t f );
void FS_FreeFile( void *buffer );
int FS_ReadLine( void *buffer, int len, fileHandle_t f );
fileHandle_t FS_SV_FOpenFileWrite( const char *filename );
int FS_SV_FOpenFileRead( const char *filename, fileHandle_t *fp );
fileHandle_t FS_SV_FOpenFileAppend( const char *filename );
int FS_Write( const void *buffer, int len, fileHandle_t h );
int FS_ReadFile( const char *qpath, void **buffer );
void FS_WriteFile( const char *qpath, const void *buffer, int size );
void FS_SV_WriteFile( const char *qpath, const void *buffer, int size );
void QDECL FS_Printf( fileHandle_t h, const char *fmt, ... );
int FS_Seek( fileHandle_t f, long offset, int origin );
__cdecl const char* FS_GetBasepath();
qboolean FS_VerifyPak( const char *pak );
void	FS_ForceFlush( fileHandle_t f );
void __cdecl FS_InitFilesystem(void);
void __cdecl FS_Shutdown(qboolean);
void __cdecl FS_ShutdownIwdPureCheckReferences(void);
void __cdecl FS_ShutdownServerIwdNames(void);
void __cdecl FS_ShutdownServerReferencedIwds(void);
void __cdecl FS_ShutdownServerReferencedFFs(void);
const char* __cdecl FS_LoadedIwdPureChecksums(void);
char* __cdecl FS_GetMapBaseName( const char* mapname );
qboolean FS_CreatePath (char *OSPath);
void FS_SV_HomeCopyFile(char* from, char* to);

#endif
