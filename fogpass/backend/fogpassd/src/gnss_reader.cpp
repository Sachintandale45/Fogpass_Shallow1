#include "gnss_reader.h"

#include <chrono>
#include <random>

namespace fogpass {

GnssReader::GnssReader() = default;

bool GnssReader::initialize(const std::string &device_path) {
    device_path_ = device_path;
    return !device_path_.empty();
}

std::vector<GnssSample> GnssReader::poll() {
    static std::mt19937 rng{std::random_device{}()};
    static std::uniform_real_distribution<double> coord(-90.0, 90.0);
    static std::uniform_real_distribution<double> accuracy(1.0, 5.0);

    GnssSample sample{
        .latitude_deg = coord(rng),
        .longitude_deg = coord(rng),
        .altitude_m = 100.0,
        .accuracy_m = accuracy(rng),
    };

    return {sample};
}

}  // namespace fogpass
