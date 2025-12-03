#pragma once

#include <functional>
#include <string>
#include <vector>

#include "gnss_reader.h"
#include "landmark_engine.h"

namespace fogpass {

class IPCHandler {
  public:
    using ModeCallback = std::function<void(LandmarkEngine::Mode)>;

    explicit IPCHandler(const std::string &config_path);

    void emitImuFirstFix(const GnssSample &fix);
    void publishUiUpcomingLandmarks(const std::vector<LandmarkDistance> &upcoming,
                                    bool changed);
    void publishDistanceTick(const std::vector<LandmarkDistance> &upcoming);
    void sendAudioAlert(const LandmarkDistance &landmark);
    void sendAudioReminder(const std::vector<LandmarkDistance> &landmarks);

    LandmarkEngine::Mode uiMode() const { return ui_mode_; }
    void registerModeChangedCallback(ModeCallback callback);
    void onUiModeChange(LandmarkEngine::Mode mode);

  private:
    void loadConfig();

    std::string config_path_;
    LandmarkEngine::Mode ui_mode_{LandmarkEngine::Mode::Automatic};
    double alert_distance_m_{600.0};
    ModeCallback mode_callback_{};
};

}  // namespace fogpass
