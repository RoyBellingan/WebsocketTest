#include "appliance.h"
#include <boost/json.hpp>
#include <iostream>
#include <pqxx/pqxx>
#include <string>

using namespace std;

extern pqxx::connection* db;

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

void Thermo1::fromRow(const pqxx::row& row) {
	row["id"].to(dbId);
	row["mac_address"].to(mac);
}

bool Thermo1::decodeMac(const boost::json::value& json) {
	/**
	 * We should receive something like this
	 *
	 * {
	 ...
	"params": {
	    "sys": {
	        "mac": "CC8DA2652404",
	        ...
	 *
	 * We will not try to extract from the name, as can be changed!
	 */

	if (auto ptr = json.try_at_pointer("/params/sys/mac"); ptr) {
		auto s = ptr->try_as_string();
		if (s.has_value()) {
			mac = s.value();
			return true;
		}
	}

	return true;
}

void ApplianceDummy::fromRow(const pqxx::row&) {
}

bool ApplianceDummy::decodeMac(const boost::json::value&) {
	return false;
}

Client::Client() {
	appliance = make_shared<ApplianceDummy>();
}
