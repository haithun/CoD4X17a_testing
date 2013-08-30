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



#include "q_shared.h"
#include "entity.h"
#include "scr_vm.h"

//Only CoD4 gamescript callback functions here


qboolean Scr_PlayerSay(gentity_t* from, int mode, const char* text){

    int callback;
    int threadId;

    callback = script_CallBacks_new[SCR_CB_NEW_SAY];
    if(!callback){
        return qfalse;
    }
    if(!say_forwardAll)
    {
        if(*text != '/' && *text != '.' && *text != '&')
            return qfalse;

        Scr_AddString(&text[1]);

    }else{
        Scr_AddString(text);
    }
    if(mode == 0)
        Scr_AddBool( qfalse );
    else
        Scr_AddBool( qtrue );

    threadId = Scr_ExecEntThread(from, callback, 1);

    Scr_FreeThread(threadId);

    return qtrue;

}
