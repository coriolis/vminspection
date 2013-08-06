/* 
 * Copyright Coriolis Technologies Pvt Ltd
 */

enum OS {
    // Linux Constants
    OS_TYPE_LINUX_REDHAT    =   0x000001,
    OS_TYPE_LINUX_DEBIAN,

    // Windows Constants
    OS_TYPE_WINDOWS_NT,
    OS_TYPE_WINDOWS_2K,
    OS_TYPE_WINDOWS_XP,
    OS_TYPE_WINDOWS_2K3,
    OS_TYPE_WINDOWS_VISTA,
    OS_TYPE_WINDOWS_2K8
};

/* file path that can identify OS */
char *OS_PATH_MAP[] = {
    "etc/redhat-release",
    "etc/debian_version"
    "pagefile.sys",
    "pagefile.sys",
    "pagefile.sys",
    "pagefile.sys",
    "pagefile.sys",
    "pagefile.sys"
    };

char *WIN_REGISTRY_FILE_PATHS[] = {
    "Winnt/system32/config/software",
    "Winnt/system32/config/software.sav",
    "WINNT/system32/config/software",
    "WINNT/system32/config/software.sav",
    "WINDOWS/system32/config/software",
    "WINDOWS/system32/config/software.sav",
    "Windows/System32/config/SOFTWARE",
    "Windows/System32/config/SOFTWARE.sav"
    };

/*
//reg info type
#define REG_DWORD       "dword" 
#define REG_STRING      "string" 
#define REG_ALL         "all" 
*/
enum REG_INFO_DATA_TYPE {
        REG_DWORD,
        REG_STRING,
        REG_ALL
    };

//reg info
enum OS_INFO {
    OS_VERSION,
    OS_PROGRAMS,
    OS_COMPONENTS,
    OS_IE,
    OS_IIS,
    OS_INFO_END
    };


typedef struct reg_key_lookup_t {
    char *key;
    enum REG_INFO_DATA_TYPE type;
    unsigned short recursive;
} reg_key_lookup_st;



/* APIs */

/* From given regfile find OS information
   Returns information in a buffer
 */
int osi_get_os_details(char *os, void *open, void *close, void *read, void *lseek, char ***info);
typedef int (* clbk_open)(const char *filename, int flags);
typedef int (* clbk_close)(int fd);
typedef ssize_t (* clbk_pread)(int fd, void *buf, size_t off, size_t count);
typedef ssize_t (* clbk_read)(int fd, void *buf, size_t count);
typedef off_t (* clbk_getsize)(int fd);
typedef off_t (* clbk_lseek)(int fd, off_t offset, int whence);
