#ifndef APPLIANCEMAP_H
#define APPLIANCEMAP_H

#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index_container.hpp>

#include "appliance.h"

struct ByNext {};
struct ByQWebsocket {};

struct ClientIndex : boost::multi_index::indexed_by<
                         boost::multi_index::hashed_unique<boost::multi_index::tag<ByQWebsocket>, BOOST_MULTI_INDEX_MEMBER(Client, QWebSocket*, qWSocket)>> {};

using ClientMap = boost::multi_index_container<ClientSP, ClientIndex>;

#endif // APPLIANCEMAP_H
