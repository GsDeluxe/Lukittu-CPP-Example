#pragma once
#ifndef LUKITTU_H
#define LUKITTU_H

#include <string>
#include <thread>
#include <vector>
#include <stdexcept> 
#include <iostream>
#include <Windows.h>

#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/rsa.h>
#include <openssl/bio.h>

#include "nlohmann/json.hpp"

#include "functionality/Helpers.hpp"
#include "functionality/Request.h"
#include "functionality/Heartbeat.h"

class LukittuLicenseSystem {
private:
    std::string team_id;
    std::string product_id;
    std::string public_key_base64;
    std::string base_url;
    bool heartbeat_running;
    int expirationDays;
    std::vector<unsigned char> base64Decode(const std::string& base64_input);
    bool VerifyResponse(const std::string& signed_challenge_hex, const std::string& original_challenge, const std::string& public_key_base64);
    bool AuthenticationCheck(const std::string& licenseKey, bool isHeartbeat = false, const std::string& sessionName = "LukittuAPI");
    
    friend class Heartbeat;

public:
    LukittuLicenseSystem(
        const std::string& teamId,
        const std::string& productId,
        const std::string& publicKeyBase64);


    int getExpirationDays() const;
    bool verifyLicense(const std::string& licenseKey, bool enableHeartbeat = true, const std::string& sessionName = "LukittuAPI", const int heartbeatInterval = 30);
    Heartbeat* heartbeat;
};

#endif
