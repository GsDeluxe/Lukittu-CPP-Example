#pragma once

#include <thread>
#include <atomic>
#include <string>

class LukittuLicenseSystem;

class Heartbeat {
private:
    LukittuLicenseSystem* parent;
    std::atomic<bool> running;
    std::thread heartbeatThread;
    int intervalSeconds;
    std::string licenseKey;
    std::string sessionName;

    void heartbeatLoop();

public:
    Heartbeat(LukittuLicenseSystem* parentSystem,
        const std::string& licenseKey,
        const std::string& sessionName = "LukittuAPI",
        int intervalSeconds = 60);

    void start();

    ~Heartbeat();
};
