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
#include <stdlib.h>

#include "osinfo.h"
#ifdef REGLOOKUP
#include "reglookuplib.h"
#else
#include "hivex.h"
#endif

static void *g_rghandle = NULL;

clbk_open g_open = NULL;
clbk_close g_close = NULL;
clbk_pread g_pread = NULL;
clbk_read g_read = NULL;
clbk_getsize g_size = NULL;
clbk_lseek g_lseek = NULL;

reg_key_lookup_st win_keys[OS_INFO_END] = {
        { "/Microsoft/Windows NT/CurrentVersion", REG_STRING, 0 },
        { "/Microsoft/Windows/CurrentVersion/Uninstall", REG_STRING, 1 },
        { "/Microsoft/Windows/CurrentVersion/Setup/OC Manager/Subcomponents", REG_DWORD, 0},
        { "/Microsoft/Internet Explorer", REG_STRING, 0 },
        { "/Microsoft/InetMgr/Parameters", REG_STRING, 0 }
    };


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
    
/*get OS name, version etc.
  Returns array of char*, treated as key-value pair 
  osinfo[0]= key
  osinfo[1]= value
*/

int osi_get_os_info(char ***osinfo)
{
<<<<<<< Updated upstream
    char **info=NULL, **ret=NULL;
=======
    char **ret=NULL, **info;
>>>>>>> Stashed changes
    char *key = NULL, *value = NULL;
    int i=0;
    info = (char **)rll_get_value_strings(rghandle, win_keys[0].key, win_keys[0].recursive);
    
    //allocate buffer to return;
    //TODO: max key-value pair 128
    ret = (char **) calloc(256, sizeof(char *)); 
   
<<<<<<< Updated upstream
=======
#ifdef REGLOOKUP
    fprintf(stdout, "Finding regkeys %s \n", win_keys[0].key);
    g_rghandle = (void *)rll_open_file_clbks(g_open, g_read, g_lseek);
    info = (char **)rll_get_value_strings(g_rghandle, win_keys[0].key, win_keys[0].recursive);
    
>>>>>>> Stashed changes
    while(info && info[i])
    {
        key = get_base_key(info[i]);
        value = get_value(info[i]);
        ret[i*2]=key;
        ret[i*2+1]=value;
        fprintf(stderr, "\t\t%s : %s \n", key, value);
        i++;
    }

<<<<<<< Updated upstream
=======
    rll_close(g_rghandle);
    print_info(0, ret);
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
>>>>>>> Stashed changes
    *osinfo = ret;
    return 0;
}
   

/* get os information */
int osi_get_os_details(void *open, void *read, void *lseek, char ***osinfo)
{
    int status =0;
    int i=0;
    char **info = NULL;
    

    if(!osinfo) 
    {
        fprintf(stderr, "Invalid parameters\n");
        return 0;
    }

<<<<<<< Updated upstream
    rghandle = (void *)rll_open_file_clbks(open, read, lseek);
=======
    g_open = op;
    g_close = cl;
    g_pread = rd;
    g_size = sz;
#ifdef REGLOOKUP
    g_lseek = sz;
    g_read = rd;
#endif
    if(os && strcmp(os, "windows")==0)
        //get the os info
        osi_get_os_info_windows(&info);
    else
        osi_get_os_info_linux(&info);
>>>>>>> Stashed changes

    if(!rghandle)
    {
        fprintf(stderr, "Failed to open registry file \n");
        return 0;
    }

    //get the os info
    osi_get_os_info(&info);
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

    rll_close(rghandle);
    return 0;
}
