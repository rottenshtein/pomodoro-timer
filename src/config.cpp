#include "config.hpp"
#include "include/mini/src/mini/ini.h"
#include "utils.hpp"
#include <filesystem>

PomodoroConfig read_pomodoro_config(std::filesystem::path path, int* err) {
	PomodoroConfig config(true);
	if (!std::filesystem::exists(path)) {
		if (err!=nullptr) *err = 1;
		return config;
	}
	mINI::INIFile configFile(path);
	mINI::INIStructure configStruct;
	configFile.read(configStruct);
	
	auto workMins = configStruct.get("timers").get("workMins");
	auto workSeconds = configStruct.get("timers").get("workSeconds");
          
	auto restMins = configStruct.get("timers").get("restMins");
	auto restSeconds = configStruct.get("timers").get("restSeconds");

	config.workMins = is_valid_num(workMins) ? std::stoi(workMins) : -1;
	config.workSeconds = is_valid_num(workSeconds) ? std::stoi(workSeconds) : -1;

	config.restMins = is_valid_num(restMins) ? std::stoi(restMins) : -1;
	config.restSeconds = is_valid_num(restSeconds) ? std::stoi(restSeconds) : -1;

	config.startText = configStruct.get("text").get("startText");
	config.workNotification = configStruct.get("text").get("workNotification");
	config.restNotification = configStruct.get("text").get("restNotification");

	auto notification = configStruct.get("notification").get("enabled"); // does not work for some reason. Will be defaulted to true for now.
	// config.notification = is_valid_num(notification) ? std::stoi(notification) : 0; 
	config.notification = 1;

	return config;

}
