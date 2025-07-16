# Lukittu License Verification C++ Example

This project demonstrates how to integrate and use the [Lukittu API](https://lukittu.com) for verifying license keys in a C++ application

---

## 🚀 Features

- Initializes `LukittuLicenseSystem` with your team and product credentials 
- Prompts for user license key via standard input
- Verifies the license with optional heartbeat support
- Prints license status and expiration days to the console

---

## 📦 Prerequisites

- C++17 or later compiler (e.g., `g++`, MSVC)
- Lukittu API headers and libraries available in your include path
- Your Lukittu **Team ID**, **Product ID**, and **Base64-encoded RSA Public Key**:

You can find these in your [Lukittu dashboard](https://app.lukittu.com/dashboard):
1. **Team ID**: `Dashboard > Team Settings > Details > ID`
2. **Product ID**: `Dashboard > Products > Select Product > ID`
3. **RSA Public Key (Base64)**: `Dashboard > Team Settings > Security > RSA Base64`

---

## 🛠️ Usage

1. Replace the placeholders in `main.cpp` with your actual credentials:

```cpp
LukittuLicenseSystem licenseSystem(
    "TEAM_ID_HERE",              // Your Team ID
    "PRODUCT_ID_HERE",           // Your Product ID
    "PUBLIC_KEY_BASE64_HERE"     // Your RSA Public Key (Base64)
);
```
---
## ⭐ Extra

Feel free to expand, improve, and contribute
