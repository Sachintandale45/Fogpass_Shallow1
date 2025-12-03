#include "gnss_reader.h"

#include <chrono>
#include <random>

namespace fogpass {

GnssReader::GnssReader() = default;

bool GnssReader::initialize(const std::string &device_path) {
    device_path_ = device_path;
    has_fix_ = false;
    stable_ = false;
    return !device_path_.empty();
}

std::vector<GnssSample> GnssReader::poll() {
    static std::mt19937 rng{std::random_device{}()};
    static std::uniform_real_distribution<double> coord(-90.0, 90.0);
    static std::uniform_real_distribution<double> accuracy(1.0, 5.0);
    static std::bernoulli_distribution stability_dist(0.7);

    GnssSample sample{
        .latitude_deg = coord(rng),
        .longitude_deg = coord(rng),
        .altitude_m = 100.0,
        .accuracy_m = accuracy(rng),
    };

    last_sample_ = sample;
    has_fix_ = true;
    stable_ = stability_dist(rng);

    return {sample};
}

bool GnssReader::isSignalStable() const {
    return stable_;
}

std::optional<GnssSample> GnssReader::latestFix() const {
    if (!has_fix_) {
        return std::nullopt;
    }
    return last_sample_;
}

}  // namespace fogpass
