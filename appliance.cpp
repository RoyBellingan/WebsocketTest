#include "appliance.h"
#include <iostream>
#include <pqxx/pqxx>
#include <string>

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

			std::cout << row["id"].as<int>() << " " << row["mac_address"].as<std::string>() << std::endl;
		}

	} catch (const std::exception& e) {
		std::cerr << "Query error: " << e.what() << std::endl;
	}
}

void Thermo1::fromRow(pqxx::row& row) {
}
