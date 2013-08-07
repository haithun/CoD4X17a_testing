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
#define PATH_SEP '/'
//#define MAX_OSPATH 256
#define MAX_FILE_HANDLES 48

//typedef int	fileHandle_t;
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
	int				checksum;					// regular checksum
	int				pure_checksum;				// checksum for pure
	int				numfiles;					// number of files in pk3
	int				referenced;					// referenced file flags
	int				hashSize;					// hash table size (power of 2)		+0x318
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

