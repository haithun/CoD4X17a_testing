typedef struct{
    long size;
    long offset;
}elf_data_t;

int ELF32_GetStrTable(char *fname, char **output,elf_data_t *text);
