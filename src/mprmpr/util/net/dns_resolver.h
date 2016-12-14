#ifndef KUDU_UTIL_NET_DNS_RESOLVER_H
#define KUDU_UTIL_NET_DNS_RESOLVER_H

#include <vector>

#include "mprmpr/base/gscoped_ptr.h"
#include "mprmpr/base/macros.h"
#include "mprmpr/util/async_util.h"
#include "mprmpr/util/status.h"

namespace mprmpr {

class HostPort;
class Sockaddr;
class ThreadPool;

// DNS Resolver which supports async address resolution.
class DnsResolver {
 public:
  DnsResolver();
  ~DnsResolver();

  // Resolve any addresses corresponding to this host:port pair.
  // Note that a host may resolve to more than one IP address.
  //
  // 'addresses' may be NULL, in which case this function simply checks that
  // the host/port pair can be resolved, without returning anything.
  //
  // When the result is available, or an error occurred, 'cb' is called
  // with the result Status.
  //
  // NOTE: the callback should be fast since it is called by the DNS
  // resolution thread.
  // NOTE: in some rare cases, the callback may also be called inline
  // from this function call, on the caller's thread.
  void ResolveAddresses(const HostPort& hostport,
                        std::vector<Sockaddr>* addresses,
                        const StatusCallback& cb);

 private:
  gscoped_ptr<ThreadPool> pool_;

  DISALLOW_COPY_AND_ASSIGN(DnsResolver);
};

} // namespace mprmpr
#endif /* KUDU_UTIL_NET_DNS_RESOLVER_H */
