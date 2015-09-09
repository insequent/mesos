/*
 * A library for interacting with calicoctl using Mesos. This could
 * easily be turned into a isolator/hook when Docker's containerizer
 * is more compatible with the new mesos modules.
 */

#include <stout/lambda.hpp>
#include <stout/os.hpp>
#include <stout/result.hpp>
#include <stout/strings.hpp>

#include <stout/os/read.hpp>

#include <process/check.hpp>
#include <process/collect.hpp>
#include <process/io.hpp>

#include "common/status_utils.hpp"

#include "logging/logging.hpp"

#include "docker/calicoctl.hpp"

using namespace process;

using std::cout;
using std::endl;
using std::string;

namespace calicoctl {


Future<Nothing> statusCheck(const Subprocess& s, const string& cmd) {
  const Option<int>& status = s.status().get();

  if (status.isNone() || status.get() != 0) {
    string msg = "Failed to execute '" + cmd + "': ";
    if (status.isSome()) {
      msg += WSTRINGIFY(status.get());
    } else {
      msg += "unknown exit status";
    }
    return Failure(msg);
  }

  CHECK_SOME(s.out());

  // TODO: Should branch from here for processing based on command
  // Sidenote: Why do we use lambda_bind? Future requirement?
  // return io::read(s.out().get())
  //   .then(lambda::bind(&Docker::__version, lambda::_1));
  return Nothing();
}


// Generic Subprocess call that returns a string
Future<Nothing> runProcess(const string& params) {
  const string cmd = "/usr/bin/calicoctl " + params;

  // TODO: Switch this to use glog (aka LOG("INFO"))
  cout << "CALICOCTL: Executing " << cmd << endl;

  Try<Subprocess> s = subprocess(
      cmd,
      Subprocess::PATH("/dev/null"), // STDIN?
      Subprocess::PIPE(),            // STDOUT?
      Subprocess::PIPE());           // STDERR?

  if (s.isError()) {
    return Failure(s.error());
  }

  return s.get().status()
    .then(lambda::bind(&statusCheck, s.get(), cmd));
}


// Simple container endpoint show
Future<Nothing> container(const string& containerName) {
  string params = "container " + containerName + " endpoint show";

  return runProcess(params);
}

// Create/delete container references in Calico, as well as assigning initial IP
Future<Nothing> container(const string& containerName, const string& action) {
  string params = "container " + action + " " + containerName;

  if (action == "add") {
    params += " any";
  }

  return runProcess(params);
}

// Add/Remove profiles from containers
Future<Nothing> container(const string& containerName,
                          const string& action,
                          const string& profileName) {
  string params = "container " + containerName + " profile " +
                  action + " " + profileName;

  return runProcess(params);
}

// Add/Remove IP from a container
Future<Nothing> containerIp(const string& containerName,
                            const string& ipAddress,
                            const string& action) {
  string params = "container " + containerName + " ip " +
                  action + " " + ipAddress;

  return runProcess(params);
}

// Create/delete profiles
Future<Nothing> profile(const string& profileName, const string& action) {
  string params = "profile " + action + " " + profileName;

  return runProcess(params);
}

} // namespace calicoctl
