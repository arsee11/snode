#ifndef SN_THREADING_H
#define SN_THREADING_H

#include <arseeulib/threading/exescope.h>
#include <easynet/event_queue_epoll.h>

namespace snode
{

using ThreadScope = arsee::ExeScope;

using EventQueueEpoll = arsee::net::EventQueueEpoll;
using ThreadScopePolling = arsee::ExeScope_p<EventQueueEpoll>;
using CurThreadingScope = arsee::CurExeScope<EventQueueEpoll>;

}//namesapce snode

#endif /*SN_THREADING_H*/
