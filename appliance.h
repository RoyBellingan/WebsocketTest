#ifndef APPLIANCE_H
#define APPLIANCE_H

#include "rbk/BoostJson/fwd.h"
#include "rbk/misc/intTypes.h"
#include <memory>
#include <pqxx/row.hxx>
#include <string>

class QWebSocket;
class Appliance;

class Appliance {
      public:
	static void  loadAll();
	virtual void fromRow(const pqxx::row& row)    = 0;
	virtual bool decodeMac(const bj::value& json) = 0;

	static std::shared_ptr<Appliance> factory(pqxx::row& row);

	u64         dbId = 0;
	std::string mac;
};
using ApplianceSP = std::shared_ptr<Appliance>;

class ApplianceDummy : public Appliance {
	void fromRow(const pqxx::row&) override;
	bool decodeMac(const bj::value&) override;
};

class Client {
      public:
	Client();
	//single thread is perfectly fine for now, no need for atomic stuff
	u64 msgRec  = 0;
	u64 msgSent = 0;

	//used in the index to sort and process the queue
	uint lastPoll = 0;

	//if we think is connected, but we are not receiving data
	uint lastUpdate = 0;

	//indexed and needed to operate
	QWebSocket* qWSocket = nullptr;

	ApplianceSP appliance;
};

using ClientSP = std::shared_ptr<Client>;

class Thermo1 : public Appliance {
      public:
	void fromRow(const pqxx::row& row) override;
	bool decodeMac(const boost::json::value& json) override;
};

#endif // APPLIANCE_H
