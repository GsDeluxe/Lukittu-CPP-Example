#pragma once
#include <string>
#include <windows.h>
#include <vector>
#include <stdexcept>
#include <iostream>
#include <random>


inline std::wstring stringToWString(const std::string& str) {
    return std::wstring(str.begin(), str.end());
}

inline std::string generateRandomString(size_t length) {
    const std::string characters = ("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789");
    std::random_device rd;
    std::uniform_int_distribution<size_t> dis(0, characters.size() - 1);

    std::string randomString;
    for (size_t i = 0; i < length; ++i) {
        randomString += characters[dis(rd)];
    }

    return randomString;
}


inline std::string getDeviceIdentifier() {
    try {
        const std::string command = "reg query HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Cryptography /v MachineGuid";

        std::wstring wideCommand = stringToWString(command);

        HANDLE hReadPipe, hWritePipe;
        SECURITY_ATTRIBUTES saAttr;
        saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
        saAttr.bInheritHandle = TRUE;
        saAttr.lpSecurityDescriptor = NULL;

        if (!CreatePipe(&hReadPipe, &hWritePipe, &saAttr, 0)) {
            throw std::runtime_error("CreatePipe failed.");
        }

        STARTUPINFO si;
        PROCESS_INFORMATION pi;
        ZeroMemory(&si, sizeof(si));
        ZeroMemory(&pi, sizeof(pi));
        si.cb = sizeof(si);
        si.hStdOutput = hWritePipe;
        si.hStdError = hWritePipe;
        si.dwFlags |= STARTF_USESTDHANDLES;

        if (!CreateProcess(NULL, &wideCommand[0], NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi)) {
            CloseHandle(hReadPipe);
            CloseHandle(hWritePipe);
            throw std::runtime_error("CreateProcess failed.");
        }

        CloseHandle(hWritePipe);

        DWORD bytesRead;
        char buffer[4096];
        std::string output;
        while (ReadFile(hReadPipe, buffer, sizeof(buffer) - 1, &bytesRead, NULL) && bytesRead > 0) {
            buffer[bytesRead] = '\0';
            output += buffer;
        }

        CloseHandle(hReadPipe);
        WaitForSingleObject(pi.hProcess, INFINITE);

        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);

        size_t pos = output.find("MachineGuid");
        if (pos != std::string::npos) {
            size_t startPos = output.find_last_of("\r\n", pos);
            std::string guid = output.substr(startPos, output.find("\r\n", startPos) - startPos);
            size_t lastSpace = guid.find_last_of(" ");
            return guid.substr(lastSpace + 1);
        }
        throw std::runtime_error("MachineGuid not found in the registry output.");
    }
    catch (const std::exception& e) {
        std::cerr << "Error retrieving device identifier: " << e.what() << std::endl;
        return "";
    }
}