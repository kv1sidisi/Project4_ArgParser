#pragma once
#include <string>
#include <iostream>
#include <vector>

namespace ArgumentParser {

class Argument {
 public:
  Argument(const char short_name, const std::string& long_name, const std::string& description = "")
      : short_name_(short_name), long_name_(long_name), description_(description) {}

  char GetShortName() const { return short_name_; }
  std::string GetLongName() const { return long_name_; }
  std::string GetDescription() const { return description_; }
  bool GetDefaultValue() const { return default_value_; }
  bool GetStoreValue() const { return store_value_; }

 protected:
  char short_name_;
  std::string long_name_;
  std::string description_;
  bool default_value_ = false;
  bool store_value_ = false;
};

class IntArgument : public Argument {
 public:
  IntArgument(const char short_name, const std::string& long_name, const std::string& description = "")
      : Argument(short_name, long_name, description) {}

  void SetDefault(int value) {
    default_value_ = true;
    key_ = value;
  }
  void SetStoreValues(std::vector<int>& int_values) {
    store_ = int_values;
    store_value_ = true;
  }
  void AddToKeys(std::string current_key){
    keys_.push_back(std::stoi(current_key));
  }
  void SetKey(std::string current_key) {
    key_ = std::stoi(current_key);
  };
  bool IsMultiValue() const { return multi_value_; }
  size_t GetMinKeys() const { return min_keys_; }
  bool IsPositional() const { return positional_; }

  std::vector<int>& GetKeys(){
    return keys_;
  }

  std::vector<int>& GetStore(){
    return store_;
  };

  void SetMultiValue() {
    multi_value_ = true;
  }
  void SetMultiValue(size_t min_count) {
    multi_value_ = true;
    min_keys_ = min_count;
  }
  void SetPositional() {
    positional_ = true;
  }
  int GetKey(){
    return key_;
  }

 private:
  bool positional_ = false;
  bool multi_value_ = false;
  size_t min_keys_ = 0;
  std::vector<int> keys_;
  int key_;
  std::vector<int> store_;
};

class StringArgument : public Argument {
 public:
  StringArgument(const char short_name, const std::string& long_name, const std::string& description = "")
      : Argument(short_name, long_name, description) {}

  void SetDefault(const std::string& value) {
    key_ = value;
    default_value_ = true;
  }
  void SetStoreValue(std::string& value) {
    store_ = value;
    store_value_ = true;
  }
  void SetKey(std::string key){
    this->key_ = key;
  }
  std::string& GetKey(){
    return key_;
  }
  bool IsMultiValue() const { return multi_value_; }
  size_t GetMinKeys() const { return min_keys_; }

  void SetMultiValue(size_t min_keys) {
    multi_value_ = true;
    min_keys_ = min_keys;
  }

  std::string& GetStore(){
    return store_;
  }

 private:
  std::string key_;
  std::string store_;
  size_t min_keys_;
  bool multi_value_ = false;
};

class FlagArgument : public Argument {
 public:
  FlagArgument(const char short_name, const std::string& long_name, const std::string& description = "")
      : Argument(short_name, long_name, description) {}

  void SetDefault(bool value) {
    default_value_ = true;
    key_ = value;
  }
  void SetStoreValue(bool& value) {
    store_ = value;
    store_value_ = true;
  }
  void SetKey(bool key){
    this->key_ = key;
  };
  bool& GetKey(){
    return key_;
  };

  bool& GetStore(){
    return store_;
  };

 private:
  bool key_ = false;
  bool store_;
};
 
class ArgParser {
 private:
  std::vector<StringArgument*> string_arguments_;
  std::vector<IntArgument*> int_arguments_;
  std::vector<FlagArgument*> flag_arguments_;

  std::string long_name_help_;
  char short_name_help_;
  std::string description_help_;
  bool help_ = false;
  bool help_used_ = false;

 public:
  std::string parser_name_;

  ArgParser(const std::string& parser_name);

  bool Parse(const std::vector<std::string>& arr);

  IntArgument& AddIntArgument(const char short_name, const std::string& long_name, const std::string& description = "");
  IntArgument& AddIntArgument(const std::string& long_name, const std::string& description = "");

  StringArgument& AddStringArgument(const char short_name, const std::string& long_name, const std::string& description = "");
  StringArgument& AddStringArgument(const std::string& long_name, const std::string& description = "");

  FlagArgument& AddFlag(const char short_name, const std::string& long_name, const std::string& description = "");
  FlagArgument& AddFlag(const std::string& long_name, const std::string& description = "");

  int GetIntValue(const std::string& argument, int number = 0);
  const std::string& GetStringValue(const std::string& argument, int number = 0);

  bool GetFlag(const std::string& argument);

  void AddHelp(char short_name, const std::string& long_name, const std::string& description);

  const std::string HelpDescription();

  bool Help();
};

}