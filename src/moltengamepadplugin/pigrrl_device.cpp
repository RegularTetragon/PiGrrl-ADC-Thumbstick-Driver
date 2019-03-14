#include "pigrrl_plugin.h"
#include "../pigrrlread.h"
//a little brevity...
#define BTN DEV_KEY
#define ABS DEV_AXIS
#define OPT DEV_OPTION

device_methods pigrrl_device::methods;

//static array of event declarations, loaded by pigrrl_manager init_profile();
//{"name","description",type,"default mapping"}
const event_decl pigrrl_events[] = {
  {"one","First Button",BTN,"first"},
  {"two","Second Button",BTN,"second"},
  {"three","Third Button",BTN,"third"},
  {"fourth","Fourth Button",BTN,"fourth"},
  {"up","Up Button",BTN,"up"},
  {"down","Down Button",BTN,"down"},
  {"left","Left Button",BTN,"left"},
  {"right","Right Button",BTN,"right"},
  {nullptr, nullptr, NO_ENTRY, nullptr}
};

pigrrl_device::pigrrl_device() {
  //basic initialization
  //it might be wise to provide a file path to a device node
  //via the constructor.
  wiringPiInit();
  pigrrl2_controller_init(&this->currentState);
  pigrrl2_config_load(&this->config);
}
pigrrl_device::~pigrrl_device() {
  //close any file descriptors...
}

int pigrrl_device::init(input_source* ref) {
  //main purpose is to provide the ref used to call methods.
  this->ref = ref;

  //now would be a good time tell MoltenGamepad to start watching a file...
  //methods.watch_file(ref, fd, tag);

  //The tag is some arbitrary value to distinguish file descriptors.
  //It CANNOT be equal to ref.
  return 0;
}

void pigrrl_device::process(void* tag) {
  //read the file identified by the tag
  //and compute values to be sent for translation.

  //  methods.send_value(ref,ev_id,ev_value);

  //ev_id is a 0-index of the events, in the order they were registered.
  //For this pigrrl, event "up" has an id of 4.
  //So if reading this file indicated the "up" button was pressed:
  //  methods.send_value(ref,4,1);
  

  //send a SYN_REPORT as appropriate. Without it, events may be ignored while
  //software waits for the SYN_REPORT.
  methods.send_syn_report(ref);
}

int pigrrl_device::process_option(const char* opname, const MGField value) {
  //an option was changed! Use the name and value to do what needs to be done.
  return 0; //should be possible in the future to signal an error...
}
