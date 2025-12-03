#include "landmark_engine.h"

#include <algorithm>
#include <cmath>
#include <fstream>
#include <sstream>

namespace fogpass {

namespace {
constexpr double kEarthRadiusMeters = 6371000.0;
constexpr double kPi = 3.14159265358979323846;
constexpr double kDegToRad = kPi / 180.0;
}

bool LandmarkEngine::loadFromCsv(const std::string &path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        return false;
    }

    landmarks_.clear();
    std::string line;
    bool header_skipped = false;
    while (std::getline(file, line)) {
        if (line.empty()) {
            continue;
        }

        if (!header_skipped) {
            header_skipped = true;
            if (line.find("name") != std::string::npos) {
                continue;
            }
        }

        std::stringstream ss(line);
        std::string name;
        std::string lat_str;
        std::string lon_str;
        std::string warn_str;

        if (!std::getline(ss, name, ',')) {
            continue;
        }
        if (!std::getline(ss, lat_str, ',')) {
            continue;
        }
        if (!std::getline(ss, lon_str, ',')) {
            continue;
        }
        if (!std::getline(ss, warn_str, ',')) {
            warn_str = "600";
        }

        Landmark lm;
        lm.name = name;
        lm.latitude_deg = std::stod(lat_str);
        lm.longitude_deg = std::stod(lon_str);
        lm.pre_warning_distance_m = std::stod(warn_str);
        lm.pre_warning_distance_m = std::clamp(lm.pre_warning_distance_m, 500.0, 900.0);

        landmarks_.push_back(lm);
    }

    return !landmarks_.empty();
}

void LandmarkEngine::setMode(Mode mode) {
    mode_ = mode;
}

std::vector<LandmarkDistance> LandmarkEngine::upcomingLandmarks(const GnssSample &current,
                                                                std::size_t count) const {
    std::vector<LandmarkDistance> result;
    result.reserve(landmarks_.size());

    for (const auto &landmark : landmarks_) {
        LandmarkDistance ld{
            .landmark = landmark,
            .distance_m = haversine(current, landmark),
        };
        result.push_back(ld);
    }

    std::sort(result.begin(), result.end(), [](const auto &lhs, const auto &rhs) {
        return lhs.distance_m < rhs.distance_m;
    });

    if (result.size() > count) {
        result.resize(count);
    }

    return result;
}

double LandmarkEngine::haversine(const GnssSample &current, const Landmark &landmark) {
    const double lat1 = current.latitude_deg * kDegToRad;
    const double lon1 = current.longitude_deg * kDegToRad;
    const double lat2 = landmark.latitude_deg * kDegToRad;
    const double lon2 = landmark.longitude_deg * kDegToRad;

    const double dlat = lat2 - lat1;
    const double dlon = lon2 - lon1;

    const double a = std::pow(std::sin(dlat / 2.0), 2) +
                     std::cos(lat1) * std::cos(lat2) * std::pow(std::sin(dlon / 2.0), 2);
    const double c = 2.0 * std::atan2(std::sqrt(a), std::sqrt(1.0 - a));

    return kEarthRadiusMeters * c;
}

}  // namespace fogpass
