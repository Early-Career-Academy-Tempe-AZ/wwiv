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

#include "bbs/input.h"
#include "bbs/subxtr.h"
#include "bbs/bbs.h"
#include "bbs/email.h"
#include "bbs/fcns.h"
#include "bbs/vars.h"
#include "core/stl.h"
#include "core/strings.h"
#include "core/textfile.h"
#include "sdk/filenames.h"

using namespace wwiv::stl;
using namespace wwiv::strings;

bool display_sub_categories();
int find_hostfor(char *type, short *ui, char *description, short *opt);


static void maybe_netmail(xtrasubsnetrec * ni, bool bAdd) {
  bout << "|#5Send email request to the host now? ";
  if (yesno()) {
    strcpy(irt, "Sub type ");
    strcat(irt, ni->stype);
    if (bAdd) {
      strcat(irt, " - ADD request");
    } else {
      strcat(irt, " - DROP request");
    }
    set_net_num(ni->net_num);
    email(irt, 1, ni->host, false, 0);
  }
}

static void sub_req(uint16_t main_type, uint16_t minor_type, int tosys, char *extra) {
  net_header_rec nh;

  nh.tosys = static_cast<uint16_t>(tosys);
  nh.touser = 1;
  nh.fromsys = net_sysnum;
  nh.fromuser = 1;
  nh.main_type = main_type;
  nh.minor_type = minor_type;
  nh.list_len = 0;
  nh.daten = static_cast<uint32_t>(time(nullptr));
  nh.method = 0;
  if (minor_type == 0) {
    // This is an alphanumeric sub type.
    nh.length = strlen(extra) + 1;
    send_net(&nh, nullptr, extra, nullptr);
  } else {
    nh.length = 1;
    send_net(&nh, nullptr, "", nullptr);
  }

  bout.nl();
  if (main_type == main_type_sub_add_req) {
    bout <<  "Automated add request sent to @" << tosys << wwiv::endl;
  } else {
    bout << "Automated drop request sent to @" << tosys << wwiv::endl;
  }
  pausescr();
}


#define OPTION_AUTO   0x0001
#define OPTION_NO_TAG 0x0002
#define OPTION_GATED  0x0004
#define OPTION_NETVAL 0x0008
#define OPTION_ANSI   0x0010


int find_hostfor(char *type, short *ui, char *description, short *opt) {
  char s[255], *ss;
  int rc = 0;

  if (description) {
    *description = 0;
  }
  *opt = 0;

  bool done = false;
  for (int i = 0; i < 256 && !done; i++) {
    if (i) {
      sprintf(s, "%s%s.%d", session()->network_directory().c_str(), SUBS_NOEXT, i);
    } else {
      sprintf(s, "%s%s", session()->network_directory().c_str(), SUBS_LST);
    }
    TextFile file(s, "r");
    if (file.IsOpen()) {
      while (!done && file.ReadLine(s, 160)) {
        if (s[0] > ' ') {
          ss = strtok(s, " \r\n\t");
          if (ss) {
            if (IsEqualsIgnoreCase(ss, type)) {
              ss = strtok(nullptr, " \r\n\t");
              if (ss) {
                short h = static_cast<short>(atol(ss));
                short o = 0;
                ss = strtok(nullptr, "\r\n");
                if (ss) {
                  int i1 = 0;
                  while (*ss && ((*ss == ' ') || (*ss == '\t'))) {
                    ++ss;
                    ++i1;
                  }
                  if (i1 < 4) {
                    while (*ss && (*ss != ' ') && (*ss != '\t')) {
                      switch (*ss) {
                      case 'T':
                        o |= OPTION_NO_TAG;
                        break;
                      case 'R':
                        o |= OPTION_AUTO;
                        break;
                      case 'G':
                        o |= OPTION_GATED;
                        break;
                      case 'N':
                        o |= OPTION_NETVAL;
                        break;
                      case 'A':
                        o |= OPTION_ANSI;
                        break;
                      }
                      ++ss;
                    }
                    while (*ss && ((*ss == ' ') || (*ss == '\t'))) {
                      ++ss;
                    }
                  }
                  if (*ui) {
                    if (*ui == h) {
                      done = true;
                      *opt = o;
                      rc = h;
                      if (description) {
                        strcpy(description, ss);
                      }
                    }
                  } else {
                    bout.nl();
                    bout << "Type: " << type << wwiv::endl;
                    bout << "Host: " << h << wwiv::endl;
                    bout << "Sub : " << ss << wwiv::endl;
                    bout.nl();
                    bout << "|#5Is this the sub you want? ";
                    if (yesno()) {
                      done = true;
                      *ui = h;
                      *opt = o;
                      rc = h;
                      if (description) {
                        strcpy(description, ss);
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }
      file.Close();
    } else {
      done = true;
    }
  }

  return rc;
}


void sub_xtr_del(int n, int nn, int f) {
  // make a copy of the old network info.
  xtrasubsnetrec xn = session()->xsubs[n].nets[nn];

  if (f) {
    auto it = session()->xsubs[n].nets.begin();
    if (nn > 0) {
      std::advance(it, nn);
    }
    session()->xsubs[n].nets.erase(it);
  }
  set_net_num(xn.net_num);

  if (xn.host != 0 && valid_system(xn.host)) {
    short opt;
    int ok = find_hostfor(xn.stype, &xn.host, nullptr, &opt);
    if (ok) {
      if (opt & OPTION_AUTO) {
        bout << "|#5Attempt automated drop request? ";
        if (yesno()) {
          sub_req(main_type_sub_drop_req, xn.type, xn.host, xn.stype);
        }
      } else {
        maybe_netmail(&xn, false);
      }
    } else {
      bout << "|#5Attempt automated drop request? ";
      if (yesno()) {
        sub_req(main_type_sub_drop_req, xn.type, xn.host, xn.stype);
      } else {
        maybe_netmail(&xn, false);
      }
    }
  }
}

void sub_xtr_add(int n, int nn) {
  unsigned short i;
  short opt;
  char szDescription[100], s[100], onx[20], *mmk, ch;
  int onxi, odci, ii, gc;

  // nn may be -1
  while (nn >= size_int(session()->xsubs[n].nets)) {
    xtrasubsnetrec xnp;
    memset(&xnp, 0, sizeof(xtrasubsnetrec));
    session()->xsubs[n].nets.push_back(xnp);
  }
  xtrasubsnetrec xnp;
  memset(&xnp, 0, sizeof(xtrasubsnetrec));

  if (session()->max_net_num() > 1) {
    odc[0] = 0;
    odci = 0;
    onx[0] = 'Q';
    onx[1] = 0;
    onxi = 1;
    bout.nl();
    for (ii = 0; ii < session()->max_net_num(); ii++) {
      if (ii < 9) {
        onx[onxi++] = static_cast<char>(ii + '1');
        onx[onxi] = 0;
      } else {
        odci = (ii + 1) / 10;
        odc[odci - 1] = static_cast<char>(odci + '0');
        odc[odci] = 0;
      }
      bout << "(" << ii + 1 << ") " << session()->net_networks[ii].name << wwiv::endl;
    }
    bout << "Q. Quit\r\n\n";
    bout << "|#2Which network (number): ";
    if (session()->max_net_num() < 9) {
      ch = onek(onx);
      if (ch == 'Q') {
        ii = -1;
      } else {
        ii = ch - '1';
      }
    } else {
      mmk = mmkey(2);
      if (*mmk == 'Q') {
        ii = -1;
      } else {
        ii = atoi(mmk) - 1;
      }
    }
    if (ii >= 0 && ii < session()->max_net_num()) {
      set_net_num(ii);
    } else {
      return;
    }
  }
  xnp.net_num = static_cast<short>(session()->net_num());

  bout.nl();
  bout << "|#2What sub type? ";
  input(xnp.stype, 7);
  if (xnp.stype[0] == 0) {
    return;
  }

  xnp.type = StringToUnsignedShort(xnp.stype);

  if (xnp.type) {
    sprintf(xnp.stype, "%u", xnp.type);
  }

  bout << "|#5Will you be hosting the sub? ";
  if (yesno()) {
    char file_name[MAX_PATH];
    sprintf(file_name, "%sn%s.net", session()->network_directory().c_str(), xnp.stype);
    File file(file_name);
    if (file.Open(File::modeBinary | File::modeCreateFile | File::modeReadWrite)) {
      file.Close();
    }

    bout << "|#5Allow auto add/drop requests? ";
    if (noyes()) {
      xnp.flags |= XTRA_NET_AUTO_ADDDROP;
    }

    bout << "|#5Make this sub public (in subs.lst)?";
    if (noyes()) {
      xnp.flags |= XTRA_NET_AUTO_INFO;
      if (display_sub_categories()) {
        gc = 0;
        while (!gc) {
          bout.nl();
          bout << "|#2Which category is this sub in (0 for unknown/misc)? ";
          input(s, 3);
          i = StringToUnsignedShort(s);
          if (i || IsEquals(s, "0")) {
            TextFile ff(session()->network_directory(), CATEG_NET, "rt");
            while (ff.ReadLine(s, 100)) {
              int i1 = StringToUnsignedShort(s);
              if (i1 == i) {
                gc = 1;
                xnp.category = i;
                break;
              }
            }
            file.Close();
            if (IsEquals(s, "0")) {
              gc = 1;
            } else if (!xnp.category) {
              bout << "Illegal/invalid category.\r\n\n";
            }
          } else {
            if (strlen(s) == 1 && s[0] == '?') {
              display_sub_categories();
              continue;
            }
          }
        }
      }
    }
  } else {
    int ok = find_hostfor(xnp.stype, &(xnp.host), szDescription, &opt);

    if (!ok) {
      bout.nl();
      bout << "|#2Which system (number) is the host? ";
      input(szDescription, 6);
      xnp.host = static_cast<uint16_t>(atol(szDescription));
      szDescription[0] = '\0';
    }
    if (!session()->xsubs[n].desc[0]) {
      strcpy(session()->xsubs[n].desc, szDescription);
    }

    if (xnp.host == net_sysnum) {
      xnp.host = 0;
    }

    if (xnp.host) {
      if (valid_system(xnp.host)) {
        if (ok) {
          if (opt & OPTION_NO_TAG) {
            session()->subboards[n].anony |= anony_no_tag;
          }
          bout.nl();
          if (opt & OPTION_AUTO) {
            bout << "|#5Attempt automated add request? ";
            if (yesno()) {
              sub_req(main_type_sub_add_req, xnp.type, xnp.host, xnp.stype);
            }
          } else {
            maybe_netmail(&xnp, true);
          }
        } else {
          bout.nl();
          bout << "|#5Attempt automated add request? ";
          bool bTryAutoAddReq = yesno();
          if (bTryAutoAddReq) {
            sub_req(main_type_sub_add_req, xnp.type, xnp.host, xnp.stype);
          } else {
            maybe_netmail(&xnp, true);
          }
        }
      } else {
        bout.nl();
        bout << "The host is not listed in the network.\r\n";
        pausescr();
      }
    }
  }
  if (nn == -1 || nn >= size_int(session()->xsubs[n].nets)) {
    // nn will be -1 when adding a new sub.
    session()->xsubs[n].nets.push_back(xnp);
  } else {
    session()->xsubs[n].nets[nn] = xnp;
  }
}

bool display_sub_categories() {
  if (!net_sysnum) {
    return false;
  }

  TextFile ff(session()->network_directory(), CATEG_NET, "rt");
  if (ff.IsOpen()) {
    bout.nl();
    bout << "Available sub categories are:\r\n";
    bool abort = false;
    char szLine[255];
    while (!abort && ff.ReadLine(szLine, 100)) {
      char* ss = strchr(szLine, '\n');
      if (ss) {
        *ss = 0;
      }
      pla(szLine, &abort);
    }
    ff.Close();
    return true;
  }
  return false;
}

int amount_of_subscribers(const char *pszNetworkFileName) {
  int numnodes = 0;

  File file(pszNetworkFileName);
  if (!file.Open(File::modeReadOnly | File::modeBinary)) {
    return 0;
  } else {
    long len1 = file.GetLength();
    if (len1 == 0) {
      return 0;
    }
    char *b = static_cast<char *>(BbsAllocA(len1));
    file.Seek(0L, File::seekBegin);
    file.Read(b, len1);
    b[len1] = 0;
    file.Close();
    long len2 = 0;
    while (len2 < len1) {
      while ((len2 < len1) && ((b[len2] < '0') || (b[len2] > '9'))) {
        ++len2;
      }
      if ((b[len2] >= '0') && (b[len2] <= '9') && (len2 < len1)) {
        numnodes++;
        while ((len2 < len1) && (b[len2] >= '0') && (b[len2] <= '9')) {
          ++len2;
        }
      }
    }
    free(b);
  }
  return numnodes;
}
