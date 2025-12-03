#include "locator.h"

#include "ipc_handler.h"

namespace fogpass {

Locator::Locator(GnssReader &reader, IPCHandler &ipc_handler)
    : reader_(reader), ipc_handler_(ipc_handler) {}

std::optional<GnssSample> Locator::update() {
    const auto samples = reader_.poll();
    if (!samples.empty()) {
        last_fix_ = samples.front();
        stable_fix_ = reader_.isSignalStable();
        maybeNotifyFirstFix();
    }
    return last_fix_;
}

void Locator::maybeNotifyFirstFix() {
    if (!stable_fix_ || first_fix_notified_ || !last_fix_) {
        return;
    }

    ipc_handler_.emitImuFirstFix(*last_fix_);
    first_fix_notified_ = true;
}

}  // namespace fogpass
