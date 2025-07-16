#include "heartbeat.h"
#include "../Lukittu.h"
#include <chrono>
#include <cstdlib>

Heartbeat::Heartbeat(LukittuLicenseSystem* parentSystem,
    const std::string& licenseKey,
    const std::string& sessionName,
    int intervalSeconds)
    : parent(parentSystem),
    licenseKey(licenseKey),
    sessionName(sessionName),
    intervalSeconds(intervalSeconds),
    running(false) {}

void Heartbeat::start() {
    if (!running) {
        running = true;
        heartbeatThread = std::thread(&Heartbeat::heartbeatLoop, this);
    }
}

void Heartbeat::heartbeatLoop() {
    while (running) {
        bool success = parent->AuthenticationCheck(licenseKey, true, sessionName);
        if (!success) {
            std::terminate();
        }
        std::this_thread::sleep_for(std::chrono::seconds(intervalSeconds));
    }
}

Heartbeat::~Heartbeat() {
    if (heartbeatThread.joinable()) {
        heartbeatThread.detach();
    }
}
