// Env.cpp
#include "env.h"

#include <cstring>
#include <string>
#include <sys/stat.h>
#include <utility>
#include <vector>

#include "boost/algorithm/string.hpp"
#include "boost/filesystem.hpp"

#include "logger.h"
#include "suffix.h"

namespace fs = boost::filesystem;

namespace cyclus {

fs::path Env::cwd_ = fs::current_path();

std::string Env::PathBase(std::string path) {
  std::string base;
  int index;

  index = path.rfind("/");
  base = path.substr(0, index);
  return base;
}

const std::string Env::GetInstallPath() {
  return "/home/r/cyc/install";
}

const std::string Env::GetBuildPath() {
  return "/home/r/cyc/cyclus/debug";
}

std::string Env::GetEnv(std::string varname) {
  char* pVar = getenv(varname.c_str());
  if (pVar == NULL) {
    return "";
  }
  return pVar;
}

const std::string Env::EnvDelimiter() {
#if _WIN32
  return ";";
#else
  return ":";
#endif
}

const std::string Env::PathDelimiter() {
#if _WIN32
  return "\\";
#else
  return "/";
#endif
}

const std::string Env::nuc_data() {
  std::string p = GetEnv("CYCLUS_NUC_DATA");
  if (p != "" && fs::exists(p)) {
    return p;
  }
  
  p = GetInstallPath() + "/share/cyclus_nuc_data.h5";
  if (fs::exists(p)) {
    return p;
  }

  p = GetBuildPath() + "/share/cyclus_nuc_data.h5";
  if (fs::exists(p)) {
    return p;
  }

  throw IOError("cyclus_nuc_data.h5 not found in "
                " environment variable CYCLUS_NUC_DATA or "
                + Env::GetInstallPath() + "/share or "
                + Env::GetBuildPath() + "/share");
}

const std::string Env::rng_schema(bool flat) {
  std::string p = GetEnv("CYCLUS_RNG_SCHEMA");
  if (p != "" && fs::exists(p)) {
    return p;
  }
  
  if (flat) {
    p = GetInstallPath() + "/share/cyclus-flat.rng.in";
    if (fs::exists(p)) {
      return p;
    }
    p = GetBuildPath() + "/share/cyclus-flat.rng.in";
    if (fs::exists(p)) {
      return p;
    }
    throw IOError("cyclus.rng.in not found in "
                  " environment variable CYCLUS_RNG_SCHEMA or "
                  + Env::GetInstallPath() + "/share or "
                  + Env::GetBuildPath() + "/share");
  } else {
    p = GetInstallPath() + "/share/cyclus.rng.in";
    if (fs::exists(p)) {
      return p;
    }
    p = GetBuildPath() + "/share/cyclus.rng.in";
    if (fs::exists(p)) {
      return p;
    }
    throw IOError("cyclus-flat.rng.in not found in "
                  " environment variable CYCLUS_RNG_SCHEMA or "
                  + Env::GetInstallPath() + "/share or "
                  + Env::GetBuildPath() + "/share");
  }
}

const std::vector<std::string> Env::cyclus_path() {
  std::string s = GetEnv("CYCLUS_PATH");
  std::vector<std::string> strs;
  boost::split(strs, s, boost::is_any_of(EnvDelimiter()));
  strs.push_back(GetInstallPath() + "/lib/cyclus");
  strs.push_back(GetBuildPath() + "/lib/cyclus");
  return strs;
}

#define SHOW(X) \
  std::cout << __FILE__ << ":" << __LINE__ << ": "#X" = " << X << "\n"

std::string Env::FindModule(std::string path) {
  std::vector<std::string> strs = cyclus_path();

  for (int i = 0; i < strs.size(); ++i) {
    fs::path full = fs::path(strs[i]) / path;
    if (fs::exists(full)) {
      return full.string();
    }
  }
  throw IOError("No module found for path " + path);
}

}  // namespace cyclus
