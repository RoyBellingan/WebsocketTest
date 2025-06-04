#ifndef APPLIANCEMAP_H
#define APPLIANCEMAP_H

#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index_container.hpp>

#include "appliance.h"

struct ByNextUpdate {};
struct ByQWebsocket {};
struct ByMac {};

struct ClientIndex : boost::multi_index::indexed_by<
                         boost::multi_index::hashed_unique<boost::multi_index::tag<ByQWebsocket>, BOOST_MULTI_INDEX_MEMBER(Client, QWebSocket*, qWSocket)>,
                         boost::multi_index::hashed_unique<boost::multi_index::tag<ByMac>, BOOST_MULTI_INDEX_MEMBER(Client, std::string, mac)>,
                         boost::multi_index::ordered_non_unique<boost::multi_index::tag<ByNextUpdate>, BOOST_MULTI_INDEX_MEMBER(Client, i64, nextUpdate)>> {};

using ClientMap = boost::multi_index_container<ClientSP, ClientIndex>;

#endif // APPLIANCEMAP_H
