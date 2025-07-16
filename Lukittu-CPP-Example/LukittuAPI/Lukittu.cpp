#include "Lukittu.h"

LukittuLicenseSystem::LukittuLicenseSystem(
    const std::string& teamId,
    const std::string& productId,
    const std::string& publicKeyBase64)
    : team_id(teamId),
    product_id(productId),
    public_key_base64(publicKeyBase64),
    base_url("https://app.lukittu.com/api/v1"),
    heartbeat_running(false),
    expirationDays(-1),
    heartbeat(nullptr)
{
    if (team_id.empty()) {
        throw std::invalid_argument("teamId must not be empty.");
    }
    if (product_id.empty()) {
        throw std::invalid_argument("productId must not be empty.");
    }
    if (public_key_base64.empty()) {
        throw std::invalid_argument("publicKeyBase64 must not be empty.");
    }
}


// PUBLIC FUNCTIONS

int LukittuLicenseSystem::getExpirationDays() const {
    return expirationDays;
}

bool LukittuLicenseSystem::verifyLicense(const std::string& licenseKey, bool enableHeartbeat, const std::string& sessionName, const int heartbeatInterval) {
    if (AuthenticationCheck(licenseKey)) {
        if (enableHeartbeat) {
            if (!heartbeat) {
                heartbeat = new Heartbeat(this, licenseKey, sessionName, heartbeatInterval);
                heartbeat->start();
            }
        }
        return true;
    }
    else {
        return false;
    }
}


// PRIVATE FUNCTIONS


bool LukittuLicenseSystem::AuthenticationCheck(const std::string& licenseKey, bool isHeartbeat, const std::string& sessionName) {
    std::string url;
    if (isHeartbeat) {
        url = base_url + "/client/teams/" + team_id + "/verification/heartbeat";
    }
    else {
        url = base_url + "/client/teams/" + team_id + "/verification/verify";
    }
    
    std::map<std::string, std::string> headers = {
        {"Content-Type", "application/json"}
    };

    std::string challenge = generateRandomString(16);
    std::string deviceIdentifier = getDeviceIdentifier();

    nlohmann::json payloadJson = {
        {"licenseKey", licenseKey},
        {"productId", product_id},
        {"challenge", challenge},
        {"deviceIdentifier", deviceIdentifier}
    };
    std::string payload = payloadJson.dump();

    std::string response = Request::HttpPostHttps(url, headers, payload, sessionName);

    if (response.empty()) {
        MessageBox(NULL, L"Malformed request or no response!", L"Error", MB_OK | MB_ICONERROR);
        return false;
    }

    std::cout << response;

    nlohmann::json j;
    try {
        j = nlohmann::json::parse(response);
    }
    catch (const std::exception& e) {
        MessageBox(NULL, L"Failed to parse JSON response!", L"Error", MB_OK | MB_ICONERROR);
        return false;
    }

    std::string code = j["result"]["code"];
    if (j.contains("data") && j["data"].contains("license") && j["data"]["license"].contains("expirationDays")) {
        expirationDays = j["data"]["license"]["expirationDays"];
    }

    if (code != "VALID") {
        return false;
    }

    std::string challengeResponse = j["result"]["challengeResponse"];
    bool verifiedResponse = VerifyResponse(challengeResponse, challenge, public_key_base64);
    if (verifiedResponse) {
        return true;
    }
    return false;
}

std::vector<unsigned char> LukittuLicenseSystem::base64Decode(const std::string& base64_input) {
    BIO* bio = BIO_new_mem_buf(base64_input.data(), base64_input.length());
    BIO* b64 = BIO_new(BIO_f_base64());
    bio = BIO_push(b64, bio);

    BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL);

    std::vector<unsigned char> decoded(base64_input.length());
    int decoded_length = BIO_read(bio, decoded.data(), base64_input.length());

    if (decoded_length < 0) {
        BIO_free_all(bio);
        std::cerr << "Failed to decode base64";
    }

    decoded.resize(decoded_length);
    BIO_free_all(bio);
    return decoded;
}

bool LukittuLicenseSystem::VerifyResponse(const std::string& signed_challenge_hex, const std::string& original_challenge, const std::string& public_key_base64)
{
    try {
        std::vector<unsigned char> decoded_key = base64Decode(public_key_base64);

        BIO* bio = BIO_new_mem_buf(decoded_key.data(), decoded_key.size());
        if (!bio) {
            MessageBox(NULL, (L"failed to create bio for public key"), (L""), MB_OK);
            return false;
        }

        EVP_PKEY* pubkey = PEM_read_bio_PUBKEY(bio, nullptr, nullptr, nullptr);
        BIO_free(bio);

        if (!pubkey) {
            MessageBox(NULL, (L"failed to load public key"), (L""), MB_OK);
            return false;
        }

        std::vector<unsigned char> signature_bytes;
        for (size_t i = 0; i < signed_challenge_hex.length(); i += 2) {
            std::string byte_string = signed_challenge_hex.substr(i, 2);
            unsigned char byte = (unsigned char)strtol(byte_string.c_str(), nullptr, 16);
            signature_bytes.push_back(byte);
        }

        EVP_MD_CTX* ctx = EVP_MD_CTX_new();
        if (!ctx) {
            EVP_PKEY_free(pubkey);
            MessageBox(NULL, (L"failed to create evp_md_ctx"), (L""), MB_OK);
            return false;
        }

        if (EVP_DigestVerifyInit(ctx, nullptr, EVP_sha256(), nullptr, pubkey) <= 0) {
            EVP_MD_CTX_free(ctx);
            EVP_PKEY_free(pubkey);
            MessageBox(NULL, (L"digestverifyinit failed"), (L""), MB_OK);
            return false;
        }

        if (EVP_DigestVerifyUpdate(ctx, original_challenge.data(), original_challenge.size()) <= 0) {
            EVP_MD_CTX_free(ctx);
            EVP_PKEY_free(pubkey);
            MessageBox(NULL, (L"digestverifyupdate failed"), (L""), MB_OK);
            return false;
        }

        int verify_ok = EVP_DigestVerifyFinal(ctx, signature_bytes.data(), signature_bytes.size());

        EVP_MD_CTX_free(ctx);
        EVP_PKEY_free(pubkey);

        return verify_ok == 1;
    }
    catch (const std::exception& ex) {
        MessageBox(NULL, (L"failed to verify session"), (L""), MB_OK);
        return false;
    }
}