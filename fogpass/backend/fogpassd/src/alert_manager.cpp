#include "alert_manager.h"

#include <algorithm>
#include <chrono>

#include "ipc_handler.h"

namespace fogpass {

AlertManager::AlertManager(LandmarkEngine &engine, IPCHandler &ipc_handler)
    : engine_(engine), ipc_handler_(ipc_handler), last_alert_ts_(std::chrono::steady_clock::now()) {}

void AlertManager::processFix(const GnssSample &fix) {
    const auto upcoming = engine_.upcomingLandmarks(fix);
    if (upcoming.empty()) {
        return;
    }

    publishUpdates(upcoming);
    maybeTriggerAudioAlert(upcoming);
    maybeReplayLastAlert();
}

void AlertManager::setReminderInterval(std::chrono::minutes interval) {
    reminder_interval_ = interval;
}

void AlertManager::publishUpdates(const std::vector<LandmarkDistance> &upcoming) {
    const bool changed = upcoming != last_published_;
    ipc_handler_.publishUiUpcomingLandmarks(upcoming, changed);
    ipc_handler_.publishDistanceTick(upcoming);

    if (changed) {
        last_published_ = upcoming;
        last_alert_ts_ = std::chrono::steady_clock::now();
    }
}

void AlertManager::maybeTriggerAudioAlert(const std::vector<LandmarkDistance> &upcoming) {
    if (upcoming.empty()) {
        return;
    }

    const auto &next = upcoming.front();
    const double threshold = next.landmark.pre_warning_distance_m;
    if (next.distance_m <= threshold) {
        ipc_handler_.sendAudioAlert(next);
        last_alert_ts_ = std::chrono::steady_clock::now();
    }
}

void AlertManager::maybeReplayLastAlert() {
    if (last_published_.empty()) {
        return;
    }

    const auto now = std::chrono::steady_clock::now();
    if (now - last_alert_ts_ >= reminder_interval_) {
        ipc_handler_.sendAudioReminder(last_published_);
        last_alert_ts_ = now;
    }
}

}  // namespace fogpass
