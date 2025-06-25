#ifndef APPLIANCE_H
#define APPLIANCE_H

#include "rbk/BoostJson/fwd.h"
#include "rbk/number/intTypes.h"
#include <QObject>
#include <QString>
#include <memory>
#include <pqxx/row.hxx>
#include <string>

class QWebSocket;
class Appliance;
class Client;

class Appliance {
      public:
	virtual ~Appliance() = default;
	static void                        loadAll();
	virtual void                       fromRow(const pqxx::row& row)    = 0;
	virtual bool                       decodeMac(const bj::value& json);
	virtual bool                       decodePacket(const QString& pk)  = 0;
	virtual void                       pollState()                      = 0;
	virtual std::string_view           getModel() const                 = 0;
	virtual std::shared_ptr<Appliance> clone() const                    = 0;

	virtual bool identify(const bj::value& json) const;

	static std::shared_ptr<Appliance> factory(pqxx::row& row);

	u64     dbId   = 0;
	Client* client = nullptr;
};
using ApplianceSP = std::shared_ptr<Appliance>;

class ApplianceDummy : public Appliance {
	void fromRow(const pqxx::row&) override;
	bool decodeMac(const bj::value&) override;
	bool decodePacket(const QString& pk) override;
	void pollState() override;

	std::shared_ptr<Appliance> clone() const override;
	std::string_view           getModel() const override;
};

class Client : public QObject {
	Q_OBJECT

      public:
	Client();
	~Client();
	//single thread is perfectly fine for now, no need for atomic stuff
	u64 msgRec  = 0;
	u64 msgSent = 0;

	//used in the index to sort and process the queue
	i64 lastPoll = 0;

	//if we think is connected, but we are not receiving data
	i64 lastUpdate = 0;

	//This is used for the index to have a list of the device to contact
	i64 nextUpdate = 0;

	//indexed and needed to operate
	QWebSocket* qWSocket = nullptr;

	ApplianceSP appliance;

	//This is here and not in appliace for now, as is just easier to use the multi index
	std::string mac;

	QString getInitialPacket() const;
	void    sendInitialPacket() const;

      signals:
	void sendMessage(const QString& message, Client* pClient);
};

using ClientSP = std::shared_ptr<Client>;

class Thermo1 : public Appliance {
      public:
	std::string_view           getModel() const override;
	void                       fromRow(const pqxx::row& row) override;
	bool                       decodePacket(const QString& pk) override;
	void                       pollState() override;
	std::shared_ptr<Appliance> clone() const override;
};

class ProEM50 : public Appliance {
	std::string_view           getModel() const override;
	void                       fromRow(const pqxx::row& row) override;
	bool                       decodePacket(const QString& pk) override;
	void                       pollState() override;
	std::shared_ptr<Appliance> clone() const override;
};

#endif // APPLIANCE_H
