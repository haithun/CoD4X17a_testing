/*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

//#define BUILD_NUMBER "1"
int _BUILD_NUMBER[1] = {
#include "version.h"
};
#define BUILD_NUMBER _BUILD_NUMBER[0]

#pragma pack(1)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <netinet/in.h>
#include <dlfcn.h>
#include <sys/mman.h>
#include <setjmp.h>
#include <sys/time.h>
#include <unistd.h>
#include <termios.h>
#include <errno.h>
#include <assert.h>
#include <fpu_control.h>
#include <time.h>
#include <wait.h>
#include <math.h>
#include <pthread.h>

//__asm__(".symver __isoc99_sscanf,__isoc99_sscanf@GLIBC_2.3");

#include "cod4_extfunc.h"

#include "cmds.h"

#include "q_platform.h"
#include "sys_local.h"
#include "filesystem.h"
#include "g_hud.h"
#include "server.h"
#include "math_vector.h"
#include "extfunc.h"
#include "hl2rcon.h"

#include "scr_vm.h"
#include "mac_authorize.h"
#include "plugin_events.h"

#include "qshared.c"
#include "cmd.c"
#include "cvar.c"
#include "huffman.c"
#include "filesystem.c"
#include "plugin_handler.h"     // Requires filesystem.c definitions!

#include "banlist.c"
#include "hashgen.c"

#include "assetspatch.c"
#include "common.c"
#include "hl2rcon.c"

#include "nvconfig.c"
#include "punkbusterfunc.c"

#include "remotecmd.c"
//#include "cmdsquery.c"

#include "msg.c"
#include "net_chan.c"
#include "g_cmds.c"
#include "g_main.c"
#include "g_team.c"
#include "g_messages.c"
#include "g_client.c"
#include "g_hud.c"
#include "movement.c"

#include "geoip.c"

#include "sv_main.c"
#include "sv_net_chan.c"
#include "sv_demo.c"
#include "sv_snapshot.c"
#include "sv_client.c"
#include "sv_world.c"
#include "sv_game.c"
#include "sv_cmds.c"
//#include "sv_uidexec.c"
//#include "sv_uidcomm.c"

#include "scr_vm_cmd.c"
#include "scr_vm_functions.c"
#include "scr_vm_classfunc.c"
#include "scr_vm_callbacks.c"
#include "scr_vm_main.c"
#include "plugin_handler.c"

#include "sys_con_tty.c"
#include "sys_patch.c"
#include "sys_main.c"
#include "sys_net.c"
#include "sys_unix.c"
#include "mac_authorize.c"


DLL_PUBLIC void Dummy(client_t *cl)
{

}