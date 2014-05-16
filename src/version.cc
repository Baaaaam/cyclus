
#include <sstream>

#include "version.h"

#include "CbcConfig.h"
#include "boost/version.hpp"
#include "hdf5.h"
#include "libxml/xmlversion.h"
#include "sqlite3.h"

// required for coin-cbc v < 2.5
#ifndef CBC_VERSION
#define CBC_VERSION CBCVERSION
#endif

namespace cyclus {
namespace version {
const char* describe() {
  return "0.4.4-200-g50aa2c9";
}

const char* core() {
  return "0.4.4";
}

const char* boost() {
  return BOOST_LIB_VERSION;
}

const char* sqlite3() {
  return SQLITE_VERSION;
}

const char* hdf5() {
  std::stringstream ss;
  ss << H5_VERS_MAJOR << ".";
  ss << H5_VERS_MINOR << ".";
  ss << H5_VERS_RELEASE << "-";
  ss << H5_VERS_SUBRELEASE;
  return ss.str().c_str();
}

const char* xml2() {
  return LIBXML_DOTTED_VERSION;
}

const char* coincbc() {
  return CBC_VERSION;
}
}  // namespace version
}  // namespace cyclus

