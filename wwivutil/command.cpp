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
#include "wwivutil/command.h"

#include <algorithm>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "core/command_line.h"
#include "core/file.h"
#include "core/strings.h"
#include "core/stl.h"
#include "sdk/config.h"

using std::map;
using std::string;
using std::vector;
using namespace wwiv::strings;
using namespace wwiv::sdk;

namespace wwiv {
namespace wwivutil {


// WWIVUTIL commands

UtilCommand::UtilCommand(const std::string& name, const std::string& description)
  : CommandLineCommand(name, description) {}
UtilCommand::~UtilCommand() {}

bool UtilCommand::add(std::unique_ptr<CommandLineCommand> cmd) {
  UtilCommand* util_command = dynamic_cast<UtilCommand*>(cmd.get());
  if (util_command != nullptr) {
    subcommands_.push_back(util_command);
    util_command->AddStandardArgs();
    util_command->AddSubCommands();
  }
  return CommandLineCommand::add(std::move(cmd));
}

bool UtilCommand::set_config(Configuration* config) { 
  for (const auto s : subcommands_) {
    s->set_config(config);
  }
  config_ = config;
  return true; 
}

}  // namespace wwivutil
}  // namespace wwiv

