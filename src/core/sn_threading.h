#ifndef SN_THREADING_H
#define SN_THREADING_H

#include <arseeulib/threading/exescope.h>

namespace snode
{

using ThreadScope = arsee::ExeScope;

template<class Poller>
using ThreadScopePolling = arsee::ExeScope_p<Poller>;

}//namesapce snode

#endif /*SN_THREADING_H*/
