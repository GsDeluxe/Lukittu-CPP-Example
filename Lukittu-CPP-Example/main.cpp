#include "LukittuAPI/Lukittu.h"
#include <iostream>

int main()
{
    try {
        // Arguments:
        // - teamId: Your unique Team ID.
        //   You can find this at https://app.lukittu.com/dashboard/team/settings
        //   Under the side panel "Details", copy the "ID" value and paste it here.

        // - productId: The ID of the product you want to license.
        //   Find it at https://app.lukittu.com/dashboard/products by selecting your product.
        //   Copy the product's ID and paste it here.

        // - publicKeyBase64: Your team's RSA public key encoded in Base64.
        //   Obtain it from https://app.lukittu.com/dashboard/team/settings under the "Security" tab.
        //   Copy the "RSA Base64" encoded public key and paste it here.
        //   This key is required to verify server signatures and ensure security.
        LukittuLicenseSystem licenseSystem(
            "TEAM_ID_HERE",              // Replace with your actual Team ID
            "PRODUCT_ID_HERE",           // Replace with your actual Product ID
            "PUBLIC_KEY_BASE64_HERE"     // Replace with your actual Base64 public key
        );

        std::cout << "Enter your license key: ";
        std::string licenseKey;
        std::getline(std::cin, licenseKey);

        // Arguments:
        // - licenseKey: The user's license key string
        // - enableHeartbeat (default=true): Starts a daemon heartbeat thread if true
        // - sessionName (default="LukittuAPI"): Optional session name for API requests
        // - heartbeatInterval (default=30): Interval in seconds for heartbeat checks
        bool success = licenseSystem.verifyLicense(licenseKey, true, "LukittuAPI", 30);
        if (success) {
            std::cout << "License verified successfully!" << std::endl;
            int expiration = licenseSystem.getExpirationDays();
            std::cout << "License expires in " << expiration << " days." << std::endl;
        }
        else {
            std::cout << "License verification failed!" << std::endl;
        }
    }
    catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
    }
    return 0;
}
