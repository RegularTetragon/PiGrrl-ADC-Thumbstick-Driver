#pragma once
#include "../plugin.h"

extern device_plugin pigrrl_dev;

extern const event_decl pigrrl_events[];


class pigrrl_device {
public:
  pigrrl_device();
  ~pigrrl_device();
  int init(input_source* ref);

  input_source* ref;
  //let our init func call private methods by marking it as a friend.
  friend PLUGIN_INIT_FUNC(pigrrl)(plugin_api api);
  static device_methods methods;

  constexpr static const char* name_stem = "ex";
protected:
  void process(void*);
  int process_option(const char* opname, const MGField value);
private:
  pigrrl2_controller_state currentState;
  pigrrl2_controller_config config;
};

class pigrrl_manager {
public:

  void init_profile();

  int init(device_manager* ref);

  int start();

  int accept_device(struct udev* udev, struct udev_device* dev);

  int process_option(const char* name, const MGField value);

  pigrrl_manager();

  ~pigrrl_manager();

  friend PLUGIN_INIT_FUNC(pigrrl)(plugin_api api);
  static manager_methods methods;
  constexpr static const char* name = "pigrrl";
private:
  device_manager* ref;
};

