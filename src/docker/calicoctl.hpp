
#ifndef __CALICOCTL_HPP__
#define __CALICOCTL_HPP__

#include <process/future.hpp>
#include <process/subprocess.hpp>

#include <stout/nothing.hpp>

using namespace process;

using std::string;

namespace calicoctl {

Future<Nothing> statusCheck(const Subprocess& s,
                            const string& cmd);

Future<Nothing> runProcess(const string& params);

Future<Nothing> container(const string& containerName);

Future<Nothing> container(const string& containerName,
                          const string& actions);

Future<Nothing> container(const string& containerName,
                          const string& action,
                          const string& profileName);

Future<Nothing> containerIP(const string& containerName,
                            const string& ipAddress,
                            const string& action);

Future<Nothing> profile(const string& profileName,
                        const string& action);
}

#endif //__CALICOCTL_HPP__
