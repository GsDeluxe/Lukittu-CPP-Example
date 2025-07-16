#pragma once
#include <map>
#include <windows.h>
#include <winhttp.h>
#include <iostream>
#include <string>
#pragma comment(lib, "winhttp.lib")

class Request {
public:
    static std::string HttpPostHttps(const std::string& url,
        const std::map<std::string, std::string>& headers,
        const std::string& postData,
        const std::string& sessionName);
};