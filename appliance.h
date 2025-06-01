#ifndef APPLIANCE_H
#define APPLIANCE_H

#include "rbk/misc/intTypes.h"
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
};

class Thermo1 : public Appliance {
};

class Appliance {
      public:
	static void loadAll();
	Appliance();
};

#endif // APPLIANCE_H
