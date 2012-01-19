/* Get OS details 
    using reglookup 
    or linux files
*/

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "osinfo.h"
//#include "reglookuplib.h"
#include "hivex.h"

static void *g_rghandle = NULL;

clbk_open g_open = NULL;
clbk_close g_close = NULL;
clbk_read g_pread = NULL;
clbk_getsize g_size = NULL;
clbk_lseek g_lseek = NULL;

reg_key_lookup_st win_keys[OS_INFO_END] = {
        { "/Microsoft/Windows NT/CurrentVersion", REG_STRING, 0 },
        { "/Microsoft/Windows/CurrentVersion/Uninstall", REG_STRING, 1 },
        { "/Microsoft/Windows/CurrentVersion/Setup/OC Manager/Subcomponents", REG_DWORD, 0},
        { "/Microsoft/Internet Explorer", REG_STRING, 0 },
        { "/Microsoft/InetMgr/Parameters", REG_STRING, 0 }
    };

char *win_os_info_path[]= { "Microsoft", "Windows NT", "CurrentVersion", NULL };

typedef enum OSI_LINUX_DISTRO_NAME_t {
        OSI_LINUX_REDHAT,
        OSI_LINUX_DEBIAN
}OSI_LINUX_DISTRO_NAME;

typedef struct linux_distro_info_st {
    char *path;
    OSI_LINUX_DISTRO_NAME name;
}linux_distro_info_t;
linux_distro_info_t  linux_dist_files[] =  {
            { "/etc/redhat-release", OSI_LINUX_REDHAT },
            {"/etc/lsb-release" , OSI_LINUX_DEBIAN },
            { NULL, 0}
        };
char *linux_kern_files_path[] = { "/var/log/dmesg", NULL};

static void print_info(int idx, char **info)
{
    int i=0;

    fprintf(stderr, "Info for %s \n", win_keys[idx].key);
    while (info[i] != NULL)
        fprintf(stderr, "\t%s\n", info[i++]);

}

/* Parse key string and return base key
    e.g. /Microsoft/Windows NT/CurrentVersion/ProductName,SZ,Microsoft Windows XP,
    returns ProductName
    Returned ptr must be freed by caller
*/
static char * get_base_key(char *fullkey)
{
    char *key = NULL;
    char *tmp = NULL;

    key = strdup(fullkey);
    tmp=strchr(key, ',');
    if(tmp)
        *tmp = '\0';
    tmp = strrchr(key, '/');
    if(tmp && tmp != key)
        memcpy(key, tmp+1, strlen(tmp)+1);

    return key;
}
/* Parse key string and return value at the right most field
    e.g. /Microsoft/Windows NT/CurrentVersion/ProductName,SZ,Microsoft Windows XP,
    returns Microsoft Windows XP
    Returned ptr must be freed by caller
*/
static char * get_value(char *fullkey)
{
    char *val = NULL;
    char *tmp = NULL;
    int len =0;

    len = strlen(fullkey);
    if(fullkey[len-1] == ',')
        fullkey[len-1] = '\0';

    tmp = strrchr(fullkey, ',');
    if(tmp)
        tmp++;
    else 
        tmp = fullkey;

    val = strdup(tmp);

    return val;
}

char **get_linux_kern_info()
{
    char buf[1024];
    int len = 0, i=0;
    int done = 0;
    char *line = NULL, *next = NULL, *data = NULL;
    char **ret= NULL;
    
    int handle = -1;
    while(linux_kern_files_path[i]) {
        handle = g_open(linux_kern_files_path[i], O_RDONLY);

        i++;
        if(handle == -1)
            continue;
        else
            break;
    }
    if(handle == -1)
    {
        fprintf(stderr, "Failed to find kern info files \n");
        return NULL;
    }

    
    //read 1st 1K we should get something within that
    len = g_pread(handle, buf, sizeof(buf)-1, 0);
    buf[len]='\0';
    line = strtok_r(buf, "\n", &next);
    while(line && !done) {
        if((data=strstr(line, "Linux version")) != NULL) {
            if(strstr(data, "[    0.000000]"))
                data = data + strlen("[    0.000000]");
            done = 1;
        }
        line = strtok_r(NULL, "\n", &next);
    }

    //found something
    if(data) {
        ret = (char **) malloc(sizeof(char *) * 4);
        ret[0] = strdup("Kernel Info");
        ret[1] = strdup(data);
        ret[2] = NULL;
        ret[3] = NULL;
    }

    if(handle != -1)
        g_close(handle);

    return ret;
        
}
            
char **get_linux_dist_info()
{
    char buf[1024];
    int len = 0, i=0;
    char *line = NULL, *next = NULL, *data = NULL;
    char **ret= NULL;
    linux_distro_info_t dinfo;
    
    int handle = -1;
    while(linux_dist_files[i].path) {
        handle = g_open(linux_dist_files[i].path, O_RDONLY);
        dinfo = linux_dist_files[i];

        i++;
        if(handle == -1)
            continue;
        else
            break;
    }
    if(handle == -1)
    {
        fprintf(stderr, "Failed to find dist info files \n");
        return NULL;
    }

    
    //read 1st 1K we should get something within that
    len = g_pread(handle, buf, sizeof(buf)-1, 0);
    buf[len]='\0';
    line = strtok_r(buf, "\n", &next);
    //MAX 10 key-value pairs
    ret = (char **) calloc(20, sizeof(char *));
    i=0;
    while(line) {
        if(dinfo.name == OSI_LINUX_DEBIAN) {
            if((data=strstr(line, "=")) != NULL) {
                *data = '\0';
                ret[i]= strdup(line);
                ret[i+1] = strdup(data+1);
                printf("%s : %s\n", ret[i], ret[i+1]);
                i += 2;
            }
            line = strtok_r(NULL, "\n", &next);
            continue;
        }
        else {
            ret[i] = strdup("ProductName");
            ret[i+1] = strdup(line);
            break;
        }
    }

    if(handle != -1)
        g_close(handle);

    return ret;
        
}

    
/* get os details for linux 
  Returns array of char*, treated as key-value pair 
  osinfo[0]= key
  osinfo[1]= value
*/

int osi_get_os_info_linux(char ***osinfo)
{
    char **info=NULL, **ret=NULL;
    int cnt =0;

    info = get_linux_kern_info();

    ret = get_linux_dist_info();
    if(info) {
        while(ret && ret[cnt]) cnt++;

        ret = realloc(ret, (cnt + 2) * sizeof(char *));
        ret[cnt] = info[0];
        ret[cnt+1] = info[1];
    }
    
    *osinfo = ret;

    return 0;
}

/*get OS name, version etc.
  Returns array of char*, treated as key-value pair 
  osinfo[0]= key
  osinfo[1]= value
*/

int osi_get_os_info_windows(char ***osinfo)
{
    char **ret=NULL;
    char *key = NULL, *value = NULL;
    int i=0;

    //allocate buffer to return;
    //TODO: max key-value pair 128
    ret = (char **) calloc(256, sizeof(char *)); 
   
#ifdef REGLOOKUP
    g_rghandle = (void *)rll_open_file_clbks(g_open, g_pread, g_size);
    info = (char **)rll_get_value_strings(g_rghandle, win_keys[0].key, win_keys[0].recursive);
    
    while(info && info[i])
    {
        key = get_base_key(info[i]);
        value = get_value(info[i]);
        ret[i*2]=key;
        ret[i*2+1]=value;
        //fprintf(stderr, "\t\t%s : %s \n", key, value);
        i++;
    }

    rll_close(g_rghandle);
#else
    {
    hive_node_h hn;
    hive_value_h *vals;


    g_rghandle = (void *)hivex_open_clbks(0, g_open, g_close, g_pread, g_size);
        

    if(!g_rghandle)
    {
        fprintf(stderr, "Failed to open registry file \n");
        return 0;
    }

    hn = hivex_root(g_rghandle);
    while(win_os_info_path[i])
    {
        hn = hivex_node_get_child(g_rghandle, hn, win_os_info_path[i]);
        if(!hn) 
        {
            printf("Node not found %s \n", win_os_info_path[i]);
            break;
        }
        i++;
    }
    i=0;
    if(hn)
    {
        vals = hivex_node_values(g_rghandle, hn);
        while(vals && vals[i])
        {
            key = hivex_value_key(g_rghandle, vals[i]);
            value = hivex_value_string(g_rghandle, vals[i]);
            ret[i*2] = key ? key : strdup("");
            ret[i*2+1] = value ? value : strdup("");
            //fprintf(stderr, "\t\t%s : %s \n", ret[i*2], ret[i*2+1]);
            i++;
        }
    }
    }        
    
    hivex_close(g_rghandle);
#endif
    *osinfo = ret;
    return 0;
}
   

/* get os information */
int osi_get_os_details(char *os, void *op, void *cl, void *rd, void *sz, char ***osinfo)
{
    char **info = NULL;

    if(!osinfo) 
    {
        fprintf(stderr, "Invalid parameters\n");
        return 0;
    }

    g_open = op;
    g_close = cl;
    g_pread = rd;
    g_size = sz;
    if(os && strcmp(os, "windows")==0)
        //get the os info
        osi_get_os_info_windows(&info);
    else
        osi_get_os_info_linux(&info);

    *osinfo = info;


    /*
    for(i=0; i<OS_INFO_END; i++)
    {
        switch(win_keys[i].type)
        {
        case REG_STRING:
            info = rll_get_value_strings(rghandle, win_keys[i].key, win_keys[i].recursive);
            break;
        case REG_DWORD:
            info = rll_get_value_dwords(rghandle, win_keys[i].key, win_keys[i].recursive);
            break;
        case REG_ALL:
            info = rll_get_subtree_value_strings(rghandle, win_keys[i].key);
            break;
        default:
            fprintf(stderr, "Invalid key type %d \n", win_keys[i].type);
        }
    
        if(info)
        {
            print_info(i, info);
        }
        else
        {
            fprintf(stderr, "No info found for %s \n", win_keys[i].key);
        }
    }
    */

    return 0;
}
