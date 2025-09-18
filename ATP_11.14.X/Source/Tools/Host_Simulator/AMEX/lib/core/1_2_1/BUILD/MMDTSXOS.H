#ifndef mmdtsxos_H
#define mmdtsxos_H

#define DTS_PATH_SEPARATOR "\\"

#define DISDIRPATH "c:\\dts\\sfiles"
#define PDBDIRPATH "c:\\dts\\pdb"

#define DTSDEBUGDIR "c:\\dts"

#ifndef MAXPATHLEN
#define MAXPATHLEN 1024
#endif  MAXPATHLEN 


#define DTS_DEFAULT_ARCHNAME	"MICROSOFT WINNT"
#define DTS_DEFAULT_ARCHID		DTS_ARCHID_MICROSOFT_NT
#define DTS_DEFAULT_LANGID		DTS_LANGID_C

#define strcasecmp	strcmpi

#define DTS_COMMAND	"CL /P"

typedef unsigned char *DTSBUF ;

#endif /* mmdtsxos_H */
