#ifndef APPLIANCE_H
#define APPLIANCE_H

#include "rbk/misc/intTypes.h"
#include <memory>
#include <pqxx/row.hxx>
#include <string>
class QWebSocket;
class Appliance;

struct State {
	//single thread is perfectly fine for now, no need for atomic stuff
	u64 msgRec  = 0;
	u64 msgSent = 0;
	//so we know how to handle different devices
	Appliance* handler;
	//will be the primary key to identify things
	std::string mac;

	//used in the index to sort and process the queue
	uint lastPoll = 0;
	//if we think is connected, but we are not receiving data
	uint lastUpdate = 0;
	//indexed and needed to operate
	QWebSocket* socket = nullptr;
};

class Appliance {
      public:
	static void loadAll();
	virtual void fromRow(pqxx::row& row) = 0;

	static std::shared_ptr<Appliance> factory(pqxx::row& row);
};

class Thermo1 : public Appliance {
      public:
	void fromRow(pqxx::row& row) override;
};

#endif // APPLIANCE_H
