#ifndef mmdtspub_H
#define mmdtspub_H


typedef enum _DTSSTATUS {

	/* Failure statuses */
	DTS_BAD_UNION_SWITCH	= -12,
	DTS_INTERNAL_ERROR		= -11,
	DTS_LANG_TYPE_MISMATCH	= -10,
	DTS_NO_UNION_IN_COBOL	= -9,
	DTS_LANG_NOT_SUPPORTED	= -8,
	DTS_ARCH_NOT_SUPPORTED	= -7,
	DTS_LOADSTRUCT_FAILED	= -6,
	DTS_PDBLOAD_FAILED		= -5,
	DTS_INVALID_ARGUMENTS	= -4,
	DTS_UNKNOWN_TYPE		= -3,
	DTS_NOT_IMPLEMENTED 	= -2,
	DTS_NOT_TRANSLATED		= -1,
	DTS_FAIL				= 0,

	/* Success */
	DTS_SUCCESS 			= 1

	/* Warning statuses */
} DTSSTATUS;

typedef struct PLATFORM {
	CHAR *Lang;
	CHAR *Arch;
} PLATFORM;

typedef struct PLATFORMID {
	unsigned char Lang;
	unsigned char Arch;
} PLATFORMID;


#define DTS_BAD_LANG_ID 	(255)
#define DTS_BAD_ARCH_CLASS	(255)


/*
 * External prototypes
 */
#ifdef C_PLUSPLUS
extern "C" {
#endif

XINT	MmDtsGetSize C_PROTOARGS((CHAR *, PLATFORM *, XINT *));
XINT	MmDtsGetSizeById C_PROTOARGS((CHAR *, PLATFORMID *, XINT *));
XINT	MmDtsTrans C_PROTOARGS((CHAR *, PLATFORM *, PLATFORM *, DTSBUF, DTSBUF, XINT));
XINT	MmDtsTransById C_PROTOARGS((CHAR *, PLATFORMID *, PLATFORMID *, DTSBUF, DTSBUF, XINT));

XINT	MmDtsGetArchName C_PROTOARGS((CHAR *));
VOID	MmDtsReload C_PROTOARGS((VOID));
VOID	MmDtsE2A C_PROTOARGS((DTSBUF InBuf, DTSBUF OutBuf, XINT Size));
VOID	MmDtsA2E C_PROTOARGS((DTSBUF InBuf, DTSBUF OutBuf, XINT Size));

VOID	DtsDebug C_PROTOARGS((CHAR *, ...));

#ifdef C_PLUSPLUS
}
#endif

#endif /* mmdtspub_H */
