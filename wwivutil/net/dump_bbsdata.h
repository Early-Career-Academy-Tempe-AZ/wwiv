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
/**************************************************************************/
#ifndef __INCLUDED_WWIVUTIL_DUMP_BBSDATA_H__
#define __INCLUDED_WWIVUTIL_DUMP_BBSDATA_H__

#include <map>
#include <string>

#include "core/command_line.h"
#include "sdk/callout.h"
#include "wwivutil/command.h"

namespace wwiv {
namespace wwivutil {

class DumpBbsDataCommand final: public UtilCommand {
public:
  DumpBbsDataCommand()
    : UtilCommand("dump_bbsdata", "Dumps parsed representation of BBSDATA.*") {}
  int Execute() override final;
  std::string GetUsage() const override final;
  bool AddSubCommands() override final;
};


}  // namespace wwivutil
}  // namespace wwiv

#endif  // __INCLUDED_WWIVUTIL_DUMP_BBSDATA_H__
