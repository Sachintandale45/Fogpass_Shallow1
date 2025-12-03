#include "alert_manager.h"
#include "gnss_reader.h"
#include "ipc_handler.h"
#include "landmark_engine.h"
#include "locator.h"

#include <chrono>
#include <iostream>
#include <thread>

int main() {
    fogpass::IPCHandler ipc_handler{"configs/fogpass.conf"};

    fogpass::LandmarkEngine landmark_engine;
    if (!landmark_engine.loadFromCsv("configs/landmarks.csv")) {
        std::cerr << "Failed to load landmark data" << std::endl;
        return 1;
    }

    landmark_engine.setMode(ipc_handler.uiMode());

    fogpass::GnssReader gnss_reader;
    if (!gnss_reader.initialize("/dev/ttyS0")) {
        std::cerr << "Failed to initialize GNSS reader" << std::endl;
        return 1;
    }

    fogpass::Locator locator{gnss_reader, ipc_handler};
    fogpass::AlertManager alert_manager{landmark_engine, ipc_handler};
    alert_manager.setReminderInterval(std::chrono::minutes(3));

    ipc_handler.registerModeChangedCallback([&](fogpass::LandmarkEngine::Mode mode) {
        landmark_engine.setMode(mode);
    });

    for (int iteration = 0; iteration < 10; ++iteration) {
        if (auto fix = locator.update()) {
            alert_manager.processFix(*fix);
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    return 0;
}
