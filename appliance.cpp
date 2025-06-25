#include "appliance.h"
#include "appliancemap.h"
#include "qwebsocket.h"
#include "rbk/BoostJson/extra.h"
#include <boost/json.hpp>
#include <iostream>
#include <pqxx/pqxx>
#include <string>

using namespace std;

extern pqxx::connection* db;

extern ClientMap clientMap;

void Appliance::loadAll() {
	//every now and then we will have to reload as new device might be added.
	//for the moment we trigger the reload when a request for something that does not exists is done
	//to avoid abuse is limited to 1 reload every 10 second

	try {
		pqxx::work txn(*db);
		// Example query
		pqxx::result r = txn.exec("SELECT * FROM public.devices");
		txn.commit();

		for (const auto& row : r) {
			auto type  = row["type"].as<std::string>();
			auto model = row["model"].as<std::string>();
			if (type == "Thermostat" && model == "changeMe") {
				auto a = make_shared<Thermo1>();
				a->fromRow(row);
			}
		}

	} catch (const std::exception& e) {
		std::cerr << "Query error: " << e.what() << std::endl;
	}
}

bool Appliance::decodeMac(const boost::json::value& json) {
	/**
	     * We should receive something like this
	     *
	     * {
	     ...
	    "result": {
	    "mac": "CC8DA2652404",
	            ...
	     *
	     * We will not try to extract from the name, as can be changed!
	     */

	if (auto ptr = json.try_at_pointer("/result/mac"); ptr) {
		auto s = ptr->try_as_string();
		if (s.has_value()) {
			//This is an indexed value, we have to notify the cache
			auto& cache = clientMap.get<ByQWebsocket>();
			if (auto iter = cache.find(client->qWSocket); iter != cache.end()) {
				cache.modify(iter, [&](ClientSP& c) {
					c->mac = s.value();
				});
			}

			//TODO also load from DB some info ...

			return true;
		}
	}

	return true;
}

string_view Thermo1::getModel() const {
	return "S3XT-0S"sv;
}

void Thermo1::fromRow(const pqxx::row& row) {
	row["id"].to(dbId);
	row["mac_address"].to(client->mac);
}

bool Thermo1::decodePacket(const QString& pk) {
	auto res = parseJson(pk, false);
	if (res.position) {
		return false;
	}
	return true;
}

void Thermo1::pollState() {
	/** The usage of the ID is critical, because they do not send back what you asked and so you can connect a request to a response in case of async, MQTT is alive -.-
{   "id": 5,   "jsonrpc": "2.0",   "method": "Number.GetStatus",   "params": { "id": 200 } } -> humidty
{"id":5,"src":"st1820-cc8da2652404","result":{"value":49,"source":"sys","last_update_ts":1749001693}}

{   "id": 5,   "jsonrpc": "2.0",   "method": "Number.GetStatus",   "params": { "id": 201 } } -> cur temp
{"id":5,"src":"st1820-cc8da2652404","result":{"value":21.4,"source":"sys","last_update_ts":1749001693}}

{   "id": 5,   "jsonrpc": "2.0",   "method": "Number.GetStatus",   "params": { "id": 202 } } -> target temp
{"id":5,"src":"st1820-cc8da2652404","result":{"value":19,"source":"sys","last_update_ts":1748915020}}
*/
	client->sendMessage(R"({"id": 200, "method": "Number.GetStatus",   "params": { "id": 200 } })", client);
	client->sendMessage(R"({"id": 201, "method": "Number.GetStatus",   "params": { "id": 201 } })", client);
	client->sendMessage(R"({"id": 202, "method": "Number.GetStatus",   "params": { "id": 202 } })", client);
}

bool Appliance::identify(const boost::json::value& json) const {
	if (auto ptr = json.try_at_pointer("/result/model"); ptr) {
		auto s = ptr->try_as_string();
		if (s.has_value()) {
			return s.value() == getModel();
		}
	}
	return false;
}

QString Client::getInitialPacket() const {
	return R"(
{
  "method": "Shelly.GetDeviceInfo",
  "id": 1,
  "params": {}
}
)";
}

void ApplianceDummy::fromRow(const pqxx::row&) {
}

bool ApplianceDummy::decodeMac(const boost::json::value&) {
	return false;
}

bool ApplianceDummy::decodePacket(const QString& pk) {
	auto res = parseJson(pk, false);
	if (res.position) {
		return false;
	}
	std::vector<Appliance*> apps{new Thermo1, new ProEM50};
	for (auto& app : apps) {
		if (app->identify(res.json)) {
			auto copy            = client;
			auto newAppliance    = app->clone();
			newAppliance->client = copy;
			copy->appliance      = newAppliance;
			copy->appliance->decodeMac(res.json);
			return true;
		}
	}
	return false;
}

void ApplianceDummy::pollState() {
	client->sendMessage("dummy poll", client);
}

Client::Client() {
	appliance         = make_shared<ApplianceDummy>();
	appliance->client = this;
	//we need an initial random value else we will have collision
	mac = to_string(QDateTime::currentMSecsSinceEpoch());
}

Client::~Client() {
}

void Client::sendInitialPacket() const {
	//TODO for the moment we only have 1 type to manage, in the future we will have to iterate over the N types we support and send the initial message
	qWSocket->sendTextMessage(Client::getInitialPacket());
}

std::shared_ptr<Appliance> ApplianceDummy::clone() const {
	return make_shared<ApplianceDummy>();
}

std::shared_ptr<Appliance> Thermo1::clone() const {
	return make_shared<Thermo1>();
}

std::string_view ApplianceDummy::getModel() const {
	return "dummy"sv;
}

string_view ProEM50::getModel() const {
	return "SPEM-002CEBEU50"sv;
}

void ProEM50::fromRow(const pqxx::row& row) {
}

bool ProEM50::decodePacket(const QString& pk) {
	return true;
}

void ProEM50::pollState() {
}

std::shared_ptr<Appliance> ProEM50::clone() const {
	return make_shared<ProEM50>();
}
