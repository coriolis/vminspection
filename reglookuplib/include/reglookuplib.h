

void *rll_open_file(char *regfile);
void *rll_open_file_clbks(void *open, void *read, void *lseek);
void rll_close(void *f);
char **rll_get_value_strings(void *p, char *key, int subtree);
char **rll_get_value_dwords(void *p, char *key, int subtree);

char **rll_get_subtree_value_strings(void *p, char *key);
