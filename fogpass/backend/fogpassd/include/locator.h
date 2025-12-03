#pragma once

#include <optional>

#include "gnss_reader.h"

namespace fogpass {

class IPCHandler;

class Locator {
  public:
    Locator(GnssReader &reader, IPCHandler &ipc_handler);

    std::optional<GnssSample> update();
    std::optional<GnssSample> currentFix() const { return last_fix_; }
    bool hasStableFix() const { return stable_fix_; }

  private:
    void maybeNotifyFirstFix();

    GnssReader &reader_;
    IPCHandler &ipc_handler_;
    std::optional<GnssSample> last_fix_;
    bool stable_fix_{false};
    bool first_fix_notified_{false};
};

}  // namespace fogpass
