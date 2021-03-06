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
#include <cstdarg>
#include <string>
#include "core//strings.h"
#include "bbs/bbs.h"
#include "bbs/fcns.h"
#include "bbs/vars.h"
#include "sdk/filenames.h"

using std::string;
using namespace wwiv::sdk;
using namespace wwiv::strings;

// Local function prototypes
void SendLocalShortMessage(unsigned int nUserNum, unsigned int nSystemNum, char *messageText);
void SendRemoteShortMessage(unsigned int nUserNum, unsigned int nSystemNum, char *messageText);

/*
 * Handles reading short messages. This is also where PackScan file requests
 * plug in, if such are used.
 */
void rsm(int nUserNum, User *pUser, bool bAskToSaveMsgs) {
  bool bShownAnyMessage = false;
  int bShownAllMessages = true;
  if (pUser->HasShortMessage()) {
    File file(session()->config()->datadir(), SMW_DAT);
    if (!file.Open(File::modeReadWrite | File::modeBinary | File::modeCreateFile)) {
      return;
    }
    int nTotalMsgsInFile = static_cast<int>(file.GetLength() / sizeof(shortmsgrec));
    for (int nCurrentMsg = 0; nCurrentMsg < nTotalMsgsInFile; nCurrentMsg++) {
      shortmsgrec sm;
      file.Seek(nCurrentMsg * sizeof(shortmsgrec), File::seekBegin);
      file.Read(&sm, sizeof(shortmsgrec));
      if (sm.touser == nUserNum && sm.tosys == 0) {
        bout.Color(9);
        bout << sm.message;
        bout.nl();
        bool bHandledMessage = false;
        bShownAnyMessage = true;
        if (!so() || !bAskToSaveMsgs) {
          bHandledMessage = true;
        } else {
          if (session()->HasConfigFlag(OP_FLAGS_CAN_SAVE_SSM)) {
            if (!bHandledMessage && bAskToSaveMsgs) {
              bout << "|#5Would you like to save this notification? ";
              bHandledMessage = !yesno();
            }
          } else {
            bHandledMessage = true;
          }

        }
        if (bHandledMessage) {
          sm.touser = 0;
          sm.tosys = 0;
          sm.message[0] = 0;
          file.Seek(nCurrentMsg * sizeof(shortmsgrec), File::seekBegin);
          file.Write(&sm, sizeof(shortmsgrec));
        } else {
          bShownAllMessages = false;
        }
      }
    }
    file.Close();
    smwcheck = true;
  }
  if (bShownAnyMessage) {
    bout.nl();
  }
  if (bShownAllMessages) {
    pUser->SetStatusFlag(User::SMW);
  }
}

void SendLocalShortMessage(unsigned int nUserNum, unsigned int nSystemNum, char *messageText) {
  User user;
  session()->users()->ReadUser(&user, nUserNum);
  if (!user.IsUserDeleted()) {
    File file(session()->config()->datadir(), SMW_DAT);
    if (!file.Open(File::modeReadWrite | File::modeBinary | File::modeCreateFile)) {
      return;
    }
    int nTotalMsgsInFile = static_cast<int>(file.GetLength() / sizeof(shortmsgrec));
    int nNewMsgPos = nTotalMsgsInFile - 1;
    shortmsgrec sm;
    if (nNewMsgPos >= 0) {
      file.Seek(nNewMsgPos * sizeof(shortmsgrec), File::seekBegin);
      file.Read(&sm, sizeof(shortmsgrec));
      while (sm.tosys == 0 && sm.touser == 0 && nNewMsgPos > 0) {
        --nNewMsgPos;
        file.Seek(nNewMsgPos * sizeof(shortmsgrec), File::seekBegin);
        file.Read(&sm, sizeof(shortmsgrec));
      }
      if (sm.tosys != 0 || sm.touser != 0) {
        nNewMsgPos++;
      }
    } else {
      nNewMsgPos = 0;
    }
    sm.tosys = static_cast<uint16_t>(nSystemNum);
    sm.touser = static_cast<uint16_t>(nUserNum);
    strncpy(sm.message, messageText, 80);
    sm.message[80] = '\0';
    file.Seek(nNewMsgPos * sizeof(shortmsgrec), File::seekBegin);
    file.Write(&sm, sizeof(shortmsgrec));
    file.Close();
    user.SetStatusFlag(User::SMW);
    session()->users()->WriteUser(&user, nUserNum);
  }
}

void SendRemoteShortMessage(int nUserNum, int nSystemNum, char *messageText) {
  net_header_rec nh;
  nh.tosys = static_cast<uint16_t>(nSystemNum);
  nh.touser = static_cast<uint16_t>(nUserNum);
  nh.fromsys = net_sysnum;
  nh.fromuser = static_cast<uint16_t>(session()->usernum);
  nh.main_type = main_type_ssm;
  nh.minor_type = 0;
  nh.list_len = 0;
  nh.daten = static_cast<uint32_t>(time(nullptr));
  if (strlen(messageText) > 80) {
    messageText[80] = '\0';
  }
  nh.length = strlen(messageText);
  nh.method = 0;
  const string packet_filename = StringPrintf("%sp0%s", 
    session()->network_directory().c_str(),
    session()->network_extension().c_str());
  File file(packet_filename);
  file.Open(File::modeReadWrite | File::modeBinary | File::modeCreateFile);
  file.Seek(0L, File::seekEnd);
  file.Write(&nh, sizeof(net_header_rec));
  file.Write(messageText, nh.length);
  file.Close();
}

/*
 * Sends a short message to a user. Takes user number and system number
 * and short message as params. Network number should be set before calling
 * this function.
 */
void ssm(int nUserNum, int nSystemNum, const char *format, ...) {
  if (nUserNum == 65535 || nUserNum == 0 || nSystemNum == 32767) {
    return;
  }

  va_list ap;
  char szMessageText[2048];

  va_start(ap, format);
  vsnprintf(szMessageText, sizeof(szMessageText), format, ap);
  va_end(ap);

  if (nSystemNum == 0) {
    SendLocalShortMessage(nUserNum, nSystemNum, szMessageText);
  } else if (net_sysnum && valid_system(nSystemNum)) {
    SendRemoteShortMessage(nUserNum, nSystemNum, szMessageText);
  }
}
