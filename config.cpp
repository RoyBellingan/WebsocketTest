#include "config.h"

#include <fmt/format.h>
#include <pqxx/pqxx>

pqxx::connection* db = nullptr;

static Config config = Config::newConfig();

const Config& conf() {
	return config;
}

Config Config::newConfig() {
	return {};
}

void initConfig(std::string_view configFile) {

	auto& dbConf  = config.dbConf;
	dbConf.user   = "postgres";
	dbConf.pass   = "powerpal";
	dbConf.host   = "127.0.0.1";
	dbConf.schema = "powerpal";
	dbConf.port   = 54320;
    
	try {
		std::string conn_string = fmt::format("postgresql://{}:{}@{}:{}/{}",
		                                      config.dbConf.user, config.dbConf.pass, config.dbConf.host, config.dbConf.port, config.dbConf.schema);
		db                      = new pqxx::connection(conn_string);

		if (db->is_open()) {
			fmt::print("Connected to database successfully!\n");
		} else {
			fmt::print("Failed to connect to database\n");
		}
	} catch (const std::exception& e) {
		fmt::print("Database connection error: {}\n", e.what());
	}
}

Config& setConf() {
	return config;
}
