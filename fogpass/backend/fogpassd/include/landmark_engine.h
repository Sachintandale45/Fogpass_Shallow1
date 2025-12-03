#pragma once

#include <optional>
#include <string>
#include <vector>

#include "gnss_reader.h"

namespace fogpass {

struct Landmark {
    std::string name;
    double latitude_deg{};
    double longitude_deg{};
    double pre_warning_distance_m{600.0};
};

struct LandmarkDistance {
    Landmark landmark;
    double distance_m{};
};

class LandmarkEngine {
  public:
    enum class Mode { Automatic, Manual };

    bool loadFromCsv(const std::string &path);
    void setMode(Mode mode);
    Mode mode() const { return mode_; }

    std::vector<LandmarkDistance> upcomingLandmarks(const GnssSample &current,
                                                    std::size_t count = 3) const;

  private:
    static double haversine(const GnssSample &current, const Landmark &landmark);

    std::vector<Landmark> landmarks_;
    Mode mode_{Mode::Automatic};
};

}  // namespace fogpass
