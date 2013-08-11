/*
	// logfile
	FS_PrintLogfile( msg );
	if ( com_logfile && com_logfile->integer ) {
        // TTimo: only open the qconsole.log if the filesystem is in an initialized state
        // also, avoid recursing in the qconsole.log opening (i.e. if fs_debug is on)
	    if ( !logfile && FS_Initialized()) {
			struct tm *newtime;
			time_t aclock;

			time( &aclock );
			newtime = localtime( &aclock );

			logfile = FS_FOpenFileWrite( "qconsole.log" );

			if ( com_logfile->integer > 1 && logfile ) {
				// force it to not buffer so we get valid
				// data even if we are crashing
				FS_ForceFlush(logfile);
			}
			if ( logfile ) FS_Write(va("\nLogfile opened on %s\n", asctime( newtime )), strlen(va("\nLogfile opened on %s\n", asctime( newtime ))), logfile);
	    }
	    if ( logfile && FS_Initialized()) {
	    	FS_Write(msg, msglen, logfile);
	    }
	}
*/