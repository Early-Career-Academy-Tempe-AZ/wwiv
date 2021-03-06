/**************************************************************************/
/*                                                                        */
/*                              WWIV Version 5.x                          */
/*             Copyright (C)1998-2016, WWIV Software Services             */
/*                                                                        */
/*    Licensed  under the  Apache License, Version  2.0 (the "License");  */
/*    you may not use this  file  except in compliance with the License.  */
/*    You may obtain a copy of the License at                             */
/*                                                                        */
/*                http://www.apache.org/licenses/LICENSE-2.0              */
/*                                                                        */
/*    Unless  required  by  applicable  law  or agreed to  in  writing,   */
/*    software  distributed  under  the  License  is  distributed on an   */
/*    "AS IS"  BASIS, WITHOUT  WARRANTIES  OR  CONDITIONS OF ANY  KIND,   */
/*    either  express  or implied.  See  the  License for  the specific   */
/*    language governing permissions and limitations under the License.   */
/*                                                                        */
/**************************************************************************/
#ifndef __INCLUDED_VARS_H__
#define __INCLUDED_VARS_H__

#include <ctime>

#include "bbs/conf.h"
#include "core/version.h"
#include "sdk/vardec.h"
#include "sdk/net.h"

///////////////////////////////////////////////////////////////////////////////

#ifdef _DEFINE_GLOBALS_
#define __EXTRN__
#else
#define __EXTRN__ extern
#endif


///////////////////////////////////////////////////////////////////////////////

static constexpr int MAX_FILENAME_LENGTH = 260;
__EXTRN__ char  ansistr[81],
          charbuffer[255],
          dc[81],
          dcd[81],
          g_szDownloadFileName[MAX_FILENAME_LENGTH],
          g_szDSZLogFileName[MAX_FILENAME_LENGTH],
          dtc[81],
          g_szExtDescrFileName[MAX_FILENAME_LENGTH],
          endofline[81],
          irt[81],
          irt_name[205],
          irt_sub[81],
          net_email_name[205],
          odc[81],
          *quotes_ind,
          tc[81];

__EXTRN__ unsigned char  checksum;

__EXTRN__ int ansiptr,
          bquote,
          change_color,
          charbufferpointer,
          chatting,
          defscreenbottom,
          do_event,
          equote,
          fsenttoday,
          fwaiting,
          nsp;

__EXTRN__ unsigned int
lines_listed;

__EXTRN__ bool  bChatLine,
          newline,
          global_xx,
          forcescansub,
          local_echo,
          mailcheck,
          guest_user,
          no_hangup,
          smwcheck,
          chatcall,
          chat_file,
          express,
          emchg,
          expressabort,
          hangup,
          incom,
          outcom,
          okskey,
          okmacro,
          ok_modem_stuff,
          returning;

// Chatroom additions
__EXTRN__ bool  in_chatroom;
__EXTRN__ unsigned short
*csn_index,
net_sysnum;

__EXTRN__ int modem_speed;

__EXTRN__ int curatr;
__EXTRN__ unsigned int
          dirconfnum,
          subconfnum;

__EXTRN__ int g_num_listed,
          timelastchar1;

__EXTRN__ time_t nscandate;


__EXTRN__ int
g_flags,
com_speed;

__EXTRN__ uint32_t
*qsc,
*qsc_n,
*qsc_q,
*qsc_p;

__EXTRN__ float batchtime;

__EXTRN__ double
extratimecall,
timeon,
time_event;

__EXTRN__ small_configrec syscfg;

__EXTRN__ configoverrec syscfgovr;
__EXTRN__ colorrec rescolor;
__EXTRN__ usersubrec *usub, *udir;
__EXTRN__ userconfrec *uconfsub, *uconfdir;
__EXTRN__ tagrec *filelist;
__EXTRN__ net_system_list_rec *csn;
__EXTRN__ confrec *subconfs, *dirconfs;
__EXTRN__ int iia;

#endif // __INCLUDED_VARS_H__

