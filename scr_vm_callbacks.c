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
