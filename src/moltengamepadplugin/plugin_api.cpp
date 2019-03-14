#include "pigrrl_plugin.h"
#include <cstring>

//This file provides the entry point that
//fills out the needed function pointers.

//A plugin should change the manager .name
//and device .name_stem fields, as well
//as the device get_description().

//Not many other changes should be needed here, other
//than changing variable names as needed.

device_plugin pigrrl_dev;

PLUGIN_INIT(pigrrl)(plugin_api api) {
  if (!API_EXACT(api))
    return -1;
  //set static vars
  pigrrl_manager::methods = *(api.head.manager);
  pigrrl_device::methods = *(api.head.device);
  pigrrl_manager* manager = new pigrrl_manager();

  //set manager call backs
  manager_plugin pigrrl_man;
  memset(&pigrrl_man, 0, sizeof(pigrrl_man));
  pigrrl_man.size = sizeof(pigrrl_man);
  pigrrl_man.name = pigrrl_manager::name;
  pigrrl_man.subscribe_to_gamepad_profile = true;
  pigrrl_man.init = [] (void* plug_data, device_manager* ref) -> int {
    return ((pigrrl_manager*)plug_data)->init(ref);
  };
  pigrrl_man.destroy = [] (void* data) -> int {
    delete (pigrrl_manager*) data;
    return 0;
  };
  pigrrl_man.start = [] (void* data) { 
    return ((pigrrl_manager*)data)->start();
  };
  pigrrl_man.process_manager_option = [] (void* ref, const char* opname, MGField opvalue) {
    return ((pigrrl_manager*)ref)->process_option(opname, opvalue);
  };
  pigrrl_man.process_udev_event = [] (void* data, struct udev* udev, struct udev_device* dev) {
    return ((pigrrl_manager*)data)->accept_device(udev, dev);
  };

  //set device call backs
  memset(&pigrrl_dev, 0, sizeof(pigrrl_dev));
  pigrrl_dev.size = sizeof(pigrrl_dev);
  pigrrl_dev.name_stem = pigrrl_device::name_stem;
  pigrrl_dev.uniq = "";
  pigrrl_dev.phys = "";
  pigrrl_dev.init = [] (void* data, input_source* ref) -> int {
    return ((pigrrl_device*)data)->init(ref);
  };
  pigrrl_dev.destroy = [] (void* data) -> int {
    delete (pigrrl_device*) data;
    return 0;
  };
  pigrrl_dev.get_description = [] (const void* data) {
    return "Example Plugin Device";
  };
  pigrrl_dev.get_type = [] (const void* data) {
    return "gamepad";
  };
  pigrrl_dev.process_event = [] (void* data, void* tag) -> int {
    ((pigrrl_device*)data)->process(tag);
    return 0;
  };
  pigrrl_dev.process_option = [] (void* data, const char* opname, MGField opvalue) {
    return ((pigrrl_device*)data)->process_option(opname, opvalue);
  };
  pigrrl_dev.upload_ff = nullptr;
  pigrrl_dev.erase_ff = nullptr;
  pigrrl_dev.play_ff = nullptr;

  api.mg.add_manager(pigrrl_man,  manager);
  return 0;
}
