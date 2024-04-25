#include "ArgParser.h"

using namespace ArgumentParser;

ArgParser::ArgParser(const std::string& str)
    : parser_name_{str} {}

bool ArgParser::Parse(const std::vector<std::string>& arr) {
  if (arr.size() == 0) {
    return true;
  }
  if (arr.size() == 1) {
    if (string_arguments_.empty() && int_arguments_.empty() && flag_arguments_.empty()) {
      return true;
    }
    for (int i = 0; i < string_arguments_.size(); i++) {
      if (string_arguments_[i]->GetDefaultValue()== false) {
        return false;
      }
    }
    for (int i = 0; i < int_arguments_.size(); i++) {
      if (int_arguments_[i]->GetDefaultValue() == false) {
        return false;
      }
    }
    for (int i = 0; i < flag_arguments_.size(); i++) {
      if (flag_arguments_[i]->GetDefaultValue() == false) {
        return false;
      }
    }
    return true;
  }

  for (int i = 1; i < arr.size(); i++) {
    if (arr[i][0] == '-') {
      if (arr[i].length() < 2) {
        return false;
      }
      if (arr[i][1] == '-') {
        size_t allocation_index = arr[i].find('=');
        if (allocation_index == std::string::npos) {
          std::string current_argument = arr[i].substr(2);
          //help check
          if (help_) {
            if (current_argument == long_name_help_) {
              help_used_ = true;
              return true;
            }
          }

          //if it is not help -> check flag
          bool flag = false;
          for (int k = 0; k < flag_arguments_.size(); k++) {
            if (flag_arguments_[k]->GetLongName() == current_argument) {
              flag_arguments_[k]->SetKey(true);
              flag = true;
              break;
            }
          }

          if (flag) {
            continue;
          } else {
            //it is not flag or help and there is no =
            return false;
          }
        } else {
//          std::cout << allocation_index<< " " << arr[i]->length()<< std::endl;
          //there is =, so writing values of int or string long_name argument
          std::string current_argument = arr[i].substr(2, allocation_index - 2);
          std::string current_key = arr[i].substr(allocation_index + 1);
//          std::cout << current_argument << " " << current_key << std::endl;
          for (int k = 0; k < string_arguments_.size(); k++) {
            if (string_arguments_[k]->GetLongName() == current_argument) {
              string_arguments_[k]->SetKey(current_key);
              break;
            }
          }

          for (int k = 0; k < int_arguments_.size(); k++) {
            if (int_arguments_[k]->GetLongName() == current_argument) {
              if (int_arguments_[k]->IsMultiValue()) {
                int_arguments_[k]->AddToKeys(current_key);
              } else {
                int_arguments_[k]->SetKey(current_key);
              }
              break;
            }
          }
        }

      } else {

        if (help_) {
          if (arr[i][1] == short_name_help_) {
            help_used_ = true;
            return true;
          }
        }
        // GetShortName() flag check
//        std::cout << flag_arguments_[0]->GetShortName() << flag_arguments_[1]->GetShortName() << flag_arguments_[2]->GetShortName();
        bool flag = false;
        for (int k = 0; k < flag_arguments_.size(); k++) {
          if (flag_arguments_[k]->GetShortName() == arr[i][1]) {
            flag_arguments_[k]->SetKey(true);
            flag = true;
            break;
          }
        }

        if (flag) {
          for (int k = 2; k < arr[i].length(); k++) {
            for (int j = 0; j < flag_arguments_.size(); j++) {
              if (flag_arguments_[j]->GetShortName() == arr[i][k]) {
                flag_arguments_[j]->SetKey(true);
                break;
              }
            }
          }
          continue;
        } else {
          //flag was not found

          //error check
          if (arr[i].length() == 2) {
            return false;
          }
          if (arr[i][2] != '=') {
            return false;
          }
          if (arr[i].length() < 4) {
            return false;
          }

          std::string current_key = arr[i].substr(3);
          char current_argument = arr[i][1];

          for (int k = 0; k < string_arguments_.size(); k++) {
            if (string_arguments_[k]->GetShortName() == current_argument) {
              string_arguments_[k]->SetKey(current_key);
              break;
            }
          }

          for (int k = 0; k < int_arguments_.size(); k++) {
            if (int_arguments_[k]->GetShortName() == current_argument) {
              if (int_arguments_[k]->IsMultiValue()) {
                int_arguments_[k]->AddToKeys(current_key);
              } else {
                int_arguments_[k]->SetKey(current_key);
              }
              break;
            }
          }
        }
      }
    } else {
      for (int k = 0; k < int_arguments_.size(); k++) {
        if (int_arguments_[k]->IsPositional()) {
          if (int_arguments_[k]->IsMultiValue()) {
            int_arguments_[k]->AddToKeys(arr[i]);
          } else {
            int_arguments_[k]->SetKey(arr[i]);
          }
        }
        break;
      }
    }
  }
  for (int i = 0; i < int_arguments_.size(); i++) {
    if (int_arguments_[i]->IsMultiValue() == true) {
      if (int_arguments_[i]->GetKeys().size() < int_arguments_[i]->GetMinKeys()) {
        return false;
      }
    }
  }

  for (int i = 0; i < string_arguments_.size(); i++) {
    if (string_arguments_[i]->GetStoreValue()) {
      string_arguments_[i]->GetStore() = string_arguments_[i]->GetKey();
    }
  }

  for (int i = 0; i < flag_arguments_.size(); i++) {
    if (flag_arguments_[i]->GetStoreValue()) {
      flag_arguments_[i]->GetStore() = flag_arguments_[i]->GetKey();
    }
  }

  for (int i = 0; i < int_arguments_.size(); i++) {
    if (int_arguments_[i]->GetStoreValue()) {
      for (int j = 0; j < int_arguments_[i]->GetKeys().size(); j++) {
        int_arguments_[i]->GetStore().push_back(int_arguments_[i]->GetKeys()[i]);
      }
    }
  }
  return true;
}

IntArgument& ArgParser::AddIntArgument(const char short_name,
                                       const std::string& long_name,
                                       const std::string& description) {
  IntArgument* a = new IntArgument(short_name, long_name, description);
  int_arguments_.push_back(a);
  return *a;
}

IntArgument& ArgParser::AddIntArgument(const std::string& long_name, const std::string& description) {
  IntArgument* a = new IntArgument(' ', long_name, description);
  int_arguments_.push_back(a);
  return *a;
}

StringArgument& ArgParser::AddStringArgument(const char short_name,
                                             const std::string& long_name,
                                             const std::string& description) {

  StringArgument* a = new StringArgument(short_name, long_name, description);
  string_arguments_.push_back(a);
  return *a;
}

StringArgument& ArgParser::AddStringArgument(const std::string& long_name, const std::string& description) {
  StringArgument* a = new StringArgument(' ', long_name, description);
  string_arguments_.push_back(a);
  return *a;
}

FlagArgument& ArgParser::AddFlag(char short_name,
                                            const std::string& long_name,
                                            const std::string& description) {
  FlagArgument* a = new FlagArgument(short_name, long_name, description);
  flag_arguments_.push_back(a);
  return *a;
}

FlagArgument& ArgParser::AddFlag(const std::string& long_name, const std::string& description) {
  FlagArgument* a = new FlagArgument(' ', long_name, description);
  flag_arguments_.push_back(a);
  return *a;
}

int ArgParser::GetIntValue(const std::string& argument, int number) {
  for (int i = 0; i < int_arguments_.size(); i++) {
    if (int_arguments_[i]->GetLongName() == argument) {
      if (int_arguments_[i]->IsMultiValue() == true) {
        return int_arguments_[i]->GetKeys()[number];
      } else {
        return int_arguments_[i]->GetKey();
      }
    }
  }
  return 0;
}

const std::string& ArgParser::GetStringValue(const std::string& argument, int number) {
  for (int i = 0; i < string_arguments_.size(); i++) {
    if (string_arguments_[i]->GetLongName() == argument) {
      return string_arguments_[i]->GetKey();
    }
  }
  return "";
}

bool ArgParser::GetFlag(const std::string& argument) {
  for (int i = 0; i < flag_arguments_.size(); i++) {
    if (flag_arguments_[i]->GetLongName() == argument) {
      return flag_arguments_[i]->GetKey();
    }
  }
  return false;
}

void ArgParser::AddHelp(char short_name, const std::string& long_name, const std::string& description) {
  help_ = true;
  long_name_help_ = long_name;
  short_name_help_ = short_name;
  description_help_ = description;
  return;
}

const std::string ArgParser::HelpDescription() {
  std::string ans = parser_name_ + '\n' + description_help_ + '\n';

  for (int i = 0; i < string_arguments_.size(); i++) {
    ans += '-' + string_arguments_[i]->GetShortName() + ", --" + string_arguments_[i]->GetLongName() + ' '
        + string_arguments_[i]->GetDescription();
    if (string_arguments_[i]->GetMinKeys() != 0) {
      ans += " [min arguments = " + std::to_string(string_arguments_[i]->GetMinKeys()) + "]\n";
    }
    if (string_arguments_[i]->GetDefaultValue() == true) {
      ans += "[default = " + string_arguments_[i]->GetKey() + "]\n";
    }
  }

  for (int i = 0; i < int_arguments_.size(); i++) {
    ans +=
        '-' + int_arguments_[i]->GetShortName() + ", --" + int_arguments_[i]->GetLongName() + ' ' + int_arguments_[i]->GetDescription();
    if (int_arguments_[i]->GetMinKeys() != 0) {
      ans += " [min arguments = " + std::to_string(int_arguments_[i]->GetMinKeys()) + "]\n";
    }
    if (int_arguments_[i]->GetDefaultValue() == true) {
      ans += "[default = " + std::to_string(int_arguments_[i]->GetKey()) + "]\n";
    }
  }

  for (int i = 0; i < flag_arguments_.size(); i++) {
    ans += '-' + flag_arguments_[i]->GetShortName() + ", --" + string_arguments_[i]->GetLongName() + ' '
        + string_arguments_[i]->GetDescription();
    if (flag_arguments_[i]->GetDefaultValue() == true) {
      if (flag_arguments_[i]->GetKey()) {
        ans += "[default = true]\n";
      } else {
        ans += "[default = false]\n";
      };
    }
  }
  return ans;
}

bool ArgParser::Help() {
  return help_used_;
}

