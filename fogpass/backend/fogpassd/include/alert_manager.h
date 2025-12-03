#pragma once

#include <chrono>
#include <optional>
#include <vector>

#include "landmark_engine.h"

namespace fogpass {

class IPCHandler;

class AlertManager {
  public:
    AlertManager(LandmarkEngine &engine, IPCHandler &ipc_handler);

    void processFix(const GnssSample &fix);
    void setReminderInterval(std::chrono::minutes interval);

  private:
    void publishUpdates(const std::vector<LandmarkDistance> &upcoming);
    void maybeTriggerAudioAlert(const std::vector<LandmarkDistance> &upcoming);
    void maybeReplayLastAlert();

    LandmarkEngine &engine_;
    IPCHandler &ipc_handler_;
    std::chrono::steady_clock::time_point last_alert_ts_{};
    std::vector<LandmarkDistance> last_published_;
    std::chrono::minutes reminder_interval_{std::chrono::minutes(3)};
};

}  // namespace fogpass
