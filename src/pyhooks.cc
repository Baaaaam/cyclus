#include "pyhooks.h"

#include  <iostream>
#ifdef CYCLUS_WITH_PYTHON
#include <stdlib.h>

#include "Python.h"

extern "C" {
#include "eventhooks.h"
#include "pyinfile.h"
#include "pymodule.h"
}

namespace cyclus {
int PY_INTERP_COUNT = 0;
bool PY_INTERP_INIT = false;


void PyAppendInitTab(void) {
#if PY_MAJOR_VERSION < 3
  // Not used before Python 3
#else
  PyImport_AppendInittab("_cyclus_eventhooks", PyInit_eventhooks);
  PyImport_AppendInittab("_cyclus_pyinfile", PyInit_pyinfile);
  PyImport_AppendInittab("_cyclus_pymodule", PyInit_pymodule);
#endif
}

void PyImportInit(void) {
#if PY_MAJOR_VERSION < 3
  initeventhooks();
  initpyinfile();
  initpymodule();
#else
  PyImport_ImportModule("_cyclus_eventhooks");
  PyImport_ImportModule("_cyclus_pyinfile");
  PyImport_ImportModule("_cyclus_pymodule");
#endif
};


void PyImportCallInit(void) {
#if PY_MAJOR_VERSION < 3
  initeventhooks();
  initpyinfile();
  initpymodule();
#else
  PyInit_eventhooks();
  PyInit_pyinfile();
  PyInit_pymodule();
#endif
};


void PyStart(void) {
  if (!PY_INTERP_INIT) {
    PyAppendInitTab();
    Py_Initialize();
    PyImportInit();
    atexit(PyStop);
    PY_INTERP_INIT = true;
  };
  PY_INTERP_COUNT++;
};

void PyStop(void) {
  PY_INTERP_COUNT--;

  // PY_INTERP_COUNT should only be negative when called atexit()
  if (PY_INTERP_INIT && PY_INTERP_COUNT < 0) {
    Py_Finalize();
  };
};

void EventLoop(void) { CyclusEventLoopHook(); };

std::string PyFindModule(std::string lib) { return CyclusPyFindModule(lib); };

Agent* MakePyAgent(std::string lib, std::string agent, void* ctx) {
  return CyclusMakePyAgent(lib, agent, ctx);
};

void InitFromPyAgent(Agent* src, Agent* dst, void* ctx) {
  CyclusInitFromPyAgent(src, dst, ctx);
};

void ClearPyAgentRefs(void) { CyclusClearPyAgentRefs(); };

void PyDelAgent(int i) { 
  std::cout << __FILE__ << ":" << __LINE__ << std::endl; 
  CyclusPyDelAgent(i); 
  std::cout << __FILE__ << ":" << __LINE__ << std::endl; 
};

namespace toolkit {
std::string PyToJson(std::string infile) { return CyclusPyToJson(infile); };

std::string JsonToPy(std::string infile) { return CyclusJsonToPy(infile); };

void PyCallListeners(std::string tstype, Agent* agent, void* cpp_ctx, int time, boost::spirit::hold_any value){
    CyclusPyCallListeners(tstype, agent, cpp_ctx, time, value);
};

}  // namespace toolkit
}  // namespace cyclus
#else   // else CYCLUS_WITH_PYTHON
#include "error.h"

namespace cyclus {
int PY_INTERP_COUNT = 0;
bool PY_INTERP_INIT = false;

void PyAppendInitTab(void) {};

void PyImportInit(void) {};

void PyImportCallInit(void) {};

void PyStart(void) {};

void PyStop(void) {};

void EventLoop(void) {};

std::string PyFindModule(std::string lib) { return std::string(""); };

Agent* MakePyAgent(std::string lib, std::string agent, void* ctx) { return NULL; };

void InitFromPyAgent(Agent* src, Agent* dst, void* ctx) {};

void ClearPyAgentRefs(void) {};

void PyDelAgent(int i) {};

namespace toolkit {
std::string PyToJson(std::string infile) {
  throw cyclus::ValidationError("Cannot convert from Python input files since "
                                "Cyclus was not built with Python bindings.");
  return "";
};

std::string JsonToPy(std::string infile) {
  throw cyclus::ValidationError("Cannot convert to Python input files since "
                                "Cyclus was not built with Python bindings.");
  return "";
};

void PyCallListeners(std::string tsname, Agent* agent, void* cpp_ctx, int time, boost::spirit::hold_any value) {};

} // namespace toolkit
} // namespace cyclus
#endif  // ends CYCLUS_WITH_PYTHON
