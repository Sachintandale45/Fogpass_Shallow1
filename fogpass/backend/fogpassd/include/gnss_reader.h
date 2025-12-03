#pragma once

#include <string>
#include <vector>

namespace fogpass {

struct GnssSample {
    double latitude_deg{};
    double longitude_deg{};
    double altitude_m{};
    double accuracy_m{};
};

class GnssReader {
  public:
    GnssReader();

    bool initialize(const std::string &device_path);
    std::vector<GnssSample> poll();

  private:
    std::string device_path_;
};

}  // namespace fogpass
