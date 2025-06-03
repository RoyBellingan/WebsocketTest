#ifndef CONFIG_H
#define CONFIG_H

#include "rbk/number/intTypes.h"
#include <string>
#include <string_view>

struct DBConf {
    DBConf(){
        //for reason the inline init is not working
        port = 5432;
    };

	std::string user;
	std::string pass;
	std::string host;
	u16         port = 5432;
	std::string schema;
	std::string db;
};

class Config {
      public:
	static Config newConfig();
	static void   dumpExample();

	Config() = default;

	DBConf dbConf;
};

const Config& conf();
Config&       setConf();
void          initConfig(std::string_view configFile);

#endif // CONFIG_H
