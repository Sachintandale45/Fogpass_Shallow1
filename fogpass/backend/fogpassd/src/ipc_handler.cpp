#include "ipc_handler.h"

#include <fstream>
#include <iostream>
#include <sstream>

namespace fogpass {

namespace {
LandmarkEngine::Mode parseMode(const std::string &value) {
    if (value == "manual") {
        return LandmarkEngine::Mode::Manual;
    }
    return LandmarkEngine::Mode::Automatic;
}

std::string modeToString(LandmarkEngine::Mode mode) {
    return mode == LandmarkEngine::Mode::Automatic ? "automatic" : "manual";
}
}  // namespace

IPCHandler::IPCHandler(const std::string &config_path) : config_path_(config_path) {
    loadConfig();
}

void IPCHandler::emitImuFirstFix(const GnssSample &fix) {
    std::cout << "[DBus] Notifying IMU service: first fix lat=" << fix.latitude_deg
              << " lon=" << fix.longitude_deg << std::endl;
}

void IPCHandler::publishUiUpcomingLandmarks(const std::vector<LandmarkDistance> &upcoming,
                                            bool changed) {
    std::cout << "[DBus] UI update (changed=" << std::boolalpha << changed << ")" << std::endl;
    for (const auto &entry : upcoming) {
        std::cout << "  - " << entry.landmark.name << ": " << entry.distance_m << " m" << std::endl;
    }
}

void IPCHandler::publishDistanceTick(const std::vector<LandmarkDistance> &upcoming) {
    std::cout << "[DBus] Distance tick @1Hz" << std::endl;
    if (!upcoming.empty()) {
        std::cout << "    Next landmark: " << upcoming.front().landmark.name
                  << " in " << upcoming.front().distance_m << " m" << std::endl;
    }
}

void IPCHandler::sendAudioAlert(const LandmarkDistance &landmark) {
    std::cout << "[UDS] Audio alert: approaching " << landmark.landmark.name << std::endl;
}

void IPCHandler::sendAudioReminder(const std::vector<LandmarkDistance> &landmarks) {
    std::cout << "[UDS] Audio reminder: still heading towards "
              << (landmarks.empty() ? "N/A" : landmarks.front().landmark.name) << std::endl;
}

void IPCHandler::registerModeChangedCallback(ModeCallback callback) {
    mode_callback_ = std::move(callback);
}

void IPCHandler::onUiModeChange(LandmarkEngine::Mode mode) {
    ui_mode_ = mode;
    std::cout << "[DBus] UI requested mode change to " << modeToString(mode) << std::endl;
    if (mode_callback_) {
        mode_callback_(mode);
    }
}

void IPCHandler::loadConfig() {
    std::ifstream file(config_path_);
    if (!file.is_open()) {
        std::cerr << "Failed to open config: " << config_path_ << std::endl;
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') {
            continue;
        }

        const auto pos = line.find('=');
        if (pos == std::string::npos) {
            continue;
        }

        const std::string key = line.substr(0, pos);
        const std::string value = line.substr(pos + 1);

        if (key == "mode") {
            ui_mode_ = parseMode(value);
        } else if (key == "alert_distance_m") {
            alert_distance_m_ = std::stod(value);
        }
    }
}

}  // namespace fogpass
