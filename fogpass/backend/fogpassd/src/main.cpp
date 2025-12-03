#include "gnss_reader.h"

#include <iostream>

int main() {
    fogpass::GnssReader reader;
    if (!reader.initialize("/dev/ttyS0")) {
        std::cerr << "Failed to initialize GNSS reader" << std::endl;
        return 1;
    }

    auto samples = reader.poll();
    if (!samples.empty()) {
        const auto &sample = samples.front();
        std::cout << "Lat: " << sample.latitude_deg
                  << " Lon: " << sample.longitude_deg
                  << " Acc: " << sample.accuracy_m << "m" << std::endl;
    }

    return 0;
}
