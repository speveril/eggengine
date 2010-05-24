#include "core.h"
#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>

// Config file parsing for Egg Core. Called at launch time.

// There should be default.cfg somewhere in the bin directory that
// lists and explains all of the options, or you can look near the
// bottom of this function to see what they do. If you're adding a
// configuration option, please update the master default.cfg so
// we know about it!

void EggCore::loadConfiguration(std::string filename) {
	// set defaults first
	config.xRes = 320;
	config.yRes = 240;
	config.noWindow = false;
	config.windowTitle = std::string("Egg Engine");

	// then read the config file
	Log::debug("Reading configuration file %s.", filename.c_str());
	
	std::string line;
	std::fstream configfile;
	std::string key, val, restofline;
	int keyend, valstart, valend, lineend;

	configfile.open(filename.c_str(), std::ios::in);
	if (!configfile) {
		Log::debug("No config file to read.");
		return;
	}

	while (!configfile.eof()) {
		std::getline(configfile, line);

		// strip out comments
		lineend = line.find_first_of("#");
		if (lineend != std::string::npos)
			line = line.substr(0, lineend);

		// figure out where the key name ends
		keyend = line.find_first_of(" \t\n\r");
		if (keyend == std::string::npos) {
			// if there's nothing to end the key, then we'll just assume the key is the whole line
			key = line;
			val = "";
		} else {
			// otherwise just get the key...
			key = line.substr(0, keyend);

			// ... and also the value
			valstart = line.find_first_not_of(" \t", keyend + 1);

			// we might have a situation where there's just a key, no value, so check for it and handle
			if (valstart != std::string::npos) {
				valend = line.find_last_not_of(" \t\n\r");
				val = line.substr(valstart, valend + 1 - valstart);
			} else {
				val = "";
			}
		}

		// convert key to all lowercase
		std::transform(key.begin(), key.end(), key.begin(), ::tolower);

		// figure out what key we've got and update config appropriately
		if (!key.compare("xres"))              { config.xRes = atoi(val.c_str()); Log::debug(" >> xRes -> %d", config.xRes); }
		else if (!key.compare("yres"))         { config.yRes = atoi(val.c_str()); Log::debug(" >> yRes -> %d", config.yRes); }
		else if (!key.compare("module"))       { scriptEngine->loadFile(val.c_str()); Log::debug(" >> module -> %s", val.c_str()); }
		else if (!key.compare("nowindow"))     { config.noWindow = true; Log::debug(" >> noWindow"); }
		else if (!key.compare("windowtitle"))  { config.windowTitle = val; Log::debug(" >> windowTitle -> \"%s\"", config.windowTitle.c_str()); }
	}

	Log::debug("Done reading config file.");

	configfile.close();
}
