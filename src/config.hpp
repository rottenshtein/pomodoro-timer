#pragma once

#include <filesystem>
#include <ostream>
#include <string>


struct PomodoroConfig {
	std::string startText;
	std::string workNotification;
	std::string restNotification;

	int workMins;
	int workSeconds;

	int restMins;
	int restSeconds;

	int notification;      // Uses libnotify.
	int soundNotification; // Also uses libnotify. (for now)

	PomodoroConfig(
		std::string startText,
		std::string workNotification,
		std::string restNotification,

		int workMins,
		int workSeconds,

		int restMins,
		int restSeconds,

		int notification,
		int soundNotification
			) : startText(startText), workNotification(workNotification), restNotification(restNotification), workMins(workMins), workSeconds(workSeconds), restMins(restMins), restSeconds(restSeconds), notification(notification), soundNotification(soundNotification) {};
	explicit PomodoroConfig(bool empty)	{
		if (!empty) {
			startText="Pomodoro timer started!";
			workNotification="Rest is over!";
			restNotification="Rest time!";

			workMins=25;
			workSeconds=0;
			restMins=5;
			restSeconds=0;

			notification=0;
			soundNotification=0;
		} else {
			workMins=-1;
			workSeconds=-1;

			restMins=-1;
			restSeconds=-1;
			notification=0;
			soundNotification=0;
		}
	};
	
	PomodoroConfig operator^(const PomodoroConfig& other) {
		return PomodoroConfig(
					other.startText.empty() ? this->startText : other.startText,
					other.workNotification.empty() ? this->workNotification : other.workNotification,
					other.restNotification.empty() ? this->restNotification : other.restNotification,
					other.workMins==-1 ? this->workMins : other.workMins,
					other.workSeconds==-1 ? this->workSeconds : other.workSeconds,
					
					other.restMins==-1 ? this->restMins : other.restMins,
					other.restSeconds==-1 ? this->restSeconds : other.restSeconds,

					other.notification,
					other.soundNotification
				);
	};
	
	friend std::ostream& operator<<(std::ostream& os, const PomodoroConfig& config) {
		os << 
			"          startText: " << config.startText << "\n" <<
			"          workNotification: " << config.workNotification << "\n" <<
			"          restNotification: " << config.restNotification << "\n" <<
			"\n" <<
			"          workMins: " << config.workMins << "\n" <<
			"          workSeconds: " << config.workSeconds << "\n" <<
			"\n" <<
			"          restMins: " << config.restMins << "\n" <<
			"          restSeconds: " << config.restSeconds << "\n" <<
			"\n" <<
			"          notification: " << (config.notification ? "true" : "false") << "\n" <<
			"          soundNotification: " << (config.soundNotification ? "true" : "false") << "\n";
		return os;
	}

};

PomodoroConfig read_pomodoro_config(std::filesystem::path path, int* err=nullptr);

