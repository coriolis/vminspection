/* Get OS details 
    using reglookup 
    or linux files
*/

#include <stdio.h>
#include <string.h>
/*
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
*/

#include "osinfo.h"
//#include "reglookuplib.h"
#include "hivex.h"

static void *rghandle = NULL;

reg_key_lookup_st win_keys[OS_INFO_END] = {
        { "/Microsoft/Windows NT/CurrentVersion", REG_STRING, 0 },
        { "/Microsoft/Windows/CurrentVersion/Uninstall", REG_STRING, 1 },
        { "/Microsoft/Windows/CurrentVersion/Setup/OC Manager/Subcomponents", REG_DWORD, 0},
        { "/Microsoft/Internet Explorer", REG_STRING, 0 },
        { "/Microsoft/InetMgr/Parameters", REG_STRING, 0 }
    };

char *win_os_info_path[]= { "Microsoft", "Windows NT", "CurrentVersion", NULL };

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
    char **info=NULL, **ret=NULL;
    char *key = NULL, *value = NULL;
    int i=0;

    //allocate buffer to return;
    //TODO: max key-value pair 128
    ret = (char **) calloc(256, sizeof(char *)); 
   
#ifdef REGLOOKUP
    info = (char **)rll_get_value_strings(rghandle, win_keys[0].key, win_keys[0].recursive);
    
    while(info && info[i])
    {
        key = get_base_key(info[i]);
        value = get_value(info[i]);
        ret[i*2]=key;
        ret[i*2+1]=value;
        //fprintf(stderr, "\t\t%s : %s \n", key, value);
        i++;
    }

#else
    {
    hive_node_h hn;
    hive_value_h *vals;

    hn = hivex_root(rghandle);
    while(win_os_info_path[i])
    {
        hn = hivex_node_get_child(rghandle, hn, win_os_info_path[i]);
        if(!hn) 
        {
            printf("Node not found %s \n", win_os_info_path[i]);
            break;
        }
        i++;
    }
    if(hn)
    {
        vals = hivex_node_values(rghandle, hn);
        while(vals && vals[i])
        {
            key = hivex_value_key(rghandle, vals[i]);
            value = hivex_value_string(rghandle, vals[i]);
            ret[i*2]=key;
            ret[i*2+1]=value;
            fprintf(stderr, "\t\t%s : %s \n", key, value);
            i++;
        }
    }
    }        
    
#endif
    *osinfo = ret;
    return 0;
}
   

/* get os information */
int osi_get_os_details(void *op, void *rd, void *sz, char ***osinfo)
{
    int status =0;
    int i=0;
    char **info = NULL;

    if(!osinfo) 
    {
        fprintf(stderr, "Invalid parameters\n");
        return 0;
    }
#ifdef REGLOOKUP
    rghandle = (void *)rll_open_file_clbks(op, rd, sz);

#else

    rghandle = (void *)hivex_open_clbks(0, op, rd, sz);
    
#endif

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

#ifdef REGLOOKUP
    rll_close(rghandle);
#else
    hivex_close(rghandle);
#endif
    return 0;
}
