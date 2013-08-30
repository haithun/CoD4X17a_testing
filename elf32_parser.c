/*
===========================================================================
    Copyright (C) 2010-2013  Ninja and TheKelm of the IceOps-Team

    This file is part of CoD4X17a-Server source code.

    CoD4X17a-Server source code is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as
    published by the Free Software Foundation, either version 3 of the
    License, or (at your option) any later version.

    CoD4X17a-Server source code is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>
===========================================================================
*/



#include <string.h>
#include <elf.h>
#include <stdlib.h>
#include "qcommon_io.h" // Com_Printf
#include "g_shared.h"   // qboolean
#include "elf32_parser.h"

int ELF32_GetStrTable(char *fname, char **output,elf_data_t *text)
{
    Elf32_Ehdr *hdr;
    Elf32_Shdr *shdr;
    char *strtable;
    char *strings;
    void *buff;
    qboolean textfound = qfalse;
    qboolean dynstrfound = qfalse;
    int j,nstrings = 0;
    long len;
    
    FILE *fp = fopen(fname,"rb");
    
    if(!fp){
        return qfalse;
    }
    
    fseek(fp,0,SEEK_END);
    len = ftell(fp);
    rewind(fp);
    
    if(len<=0)
        return qfalse;
    buff = malloc(len);
    
    if(buff == NULL)
        return qfalse;
        
    if(fread(buff,len,1,fp)!=1){
        if(buff)
            free(buff);
        return qfalse;
    }
    hdr = buff;
    if(hdr->e_ident[0] != ELFMAG0 || hdr->e_ident[1] != ELFMAG1 || hdr->e_ident[2] != ELFMAG2 || hdr->e_ident[3] != ELFMAG3){
        if(buff)
            free(buff);
        return qfalse;
    }
    if(hdr->e_type != ET_DYN){
        if(buff)
            free(buff);
        return qfalse;
    }
    if((shdr = (Elf32_Shdr *)(buff + hdr->e_shoff))>=(Elf32_Shdr *)(buff + len-sizeof(Elf32_Shdr))){
        if(buff)
            free(buff);
        return qfalse;
    }
    if(hdr->e_shstrndx!=SHN_UNDEF){
        if(hdr->e_shstrndx!=SHN_XINDEX){     // Typical case, standard elf addressing
            if(hdr->e_shstrndx<=hdr->e_shnum)
                strtable = buff + shdr[hdr->e_shstrndx].sh_offset;
            else{
                Com_Printf("Error: the string table index is too big! String table index: %d, section headers: %d.\n",hdr->e_shstrndx,hdr->e_shnum);
            if(buff)
                free(buff);
            return qfalse;
            }
        }else{        // So called 'elf extended addressing'. Because 'simple' is too mainstream.
            if(shdr[0].sh_link<=hdr->e_shnum)
                strtable = buff + shdr[shdr[0].sh_link].sh_offset;
            else{
                Com_Printf("Error: the string table index is too big! String table index: %d, section headers: %d.\n",shdr[0].sh_link,hdr->e_shnum);
            if(buff)
                free(buff);
            return qfalse;
            }
        }
    }
    else{
        Com_Printf("Could not find the string table.\n");
        if(buff)
            free(buff);
        return qfalse;
    }
    //Com_Printf("Debug2\n");
    for(j=0;j<hdr->e_shnum;++j){
        if(strcmp(&strtable[shdr[j].sh_name],".text")==0){
            textfound = qtrue;
            text->size = shdr[j].sh_size;
            text->offset = shdr[j].sh_addr;
            //Com_Printf("Debug: .text section found! Size: %d, address: %d.\n",shdr[j].sh_size,shdr[j].sh_addr);
            if(dynstrfound)
                break;
        }
        else if(strcmp(&strtable[shdr[j].sh_name],".dynstr")==0){
            dynstrfound = qtrue;
            nstrings = shdr[j].sh_size;
            strings = (char *)malloc(sizeof(char)*nstrings);
            strings = malloc(shdr[j].sh_size);
            memcpy(strings,shdr[j].sh_offset + buff,shdr[j].sh_size);
            *output = strings;
            if(textfound)
                break;
        }
    }
    if(textfound && dynstrfound){
        if(buff)
            free(buff);
        return nstrings;
    }else{
        if(buff)
            free(buff);
        return qfalse;
    }
}
