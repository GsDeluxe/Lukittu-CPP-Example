#include "Request.h"

std::wstring MapToHeaders(const std::map<std::string, std::string>& headers) {
    std::wstring wHeaders;
    for (const auto& header : headers) {
        wHeaders += std::wstring(header.first.begin(), header.first.end()) + L": " +
            std::wstring(header.second.begin(), header.second.end()) + L"\r\n";
    }
    return wHeaders;
}

std::string Request::HttpPostHttps(const std::string& url, const std::map<std::string, std::string>& headers, const std::string& postData, const std::string& sessionName) {
    HINTERNET hSession = NULL, hConnect = NULL, hRequest = NULL;
    DWORD bytesRead = 0;
    CHAR buffer[4096];
    std::string responseData;

    hSession = WinHttpOpen((L"LukittuAPI"),
        WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
        WINHTTP_NO_PROXY_NAME,
        WINHTTP_NO_PROXY_BYPASS, 0);
    if (hSession == NULL) {
        return "";
    }

    URL_COMPONENTS urlComp = { 0 };
    urlComp.dwStructSize = sizeof(URL_COMPONENTS);
    wchar_t host[256] = L"";
    wchar_t urlPath[1024] = L"";
    urlComp.lpszHostName = host;
    urlComp.dwHostNameLength = sizeof(host) / sizeof(wchar_t);
    urlComp.lpszUrlPath = urlPath;
    urlComp.dwUrlPathLength = sizeof(urlPath) / sizeof(wchar_t);

    if (!WinHttpCrackUrl(std::wstring(url.begin(), url.end()).c_str(), 0, 0, &urlComp)) {
        WinHttpCloseHandle(hSession);
        return "";
    }

    hConnect = WinHttpConnect(hSession, urlComp.lpszHostName, INTERNET_DEFAULT_HTTPS_PORT, 0);
    if (hConnect == NULL) {
        WinHttpCloseHandle(hSession);
        return "";
    }

    hRequest = WinHttpOpenRequest(hConnect, (L"POST"), urlComp.lpszUrlPath, NULL,
        WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES,
        WINHTTP_FLAG_SECURE);
    if (hRequest == NULL) {
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        return "";
    }

    std::wstring wHeaders = MapToHeaders(headers);
    if (!WinHttpAddRequestHeaders(hRequest, wHeaders.c_str(), (ULONG)-1, WINHTTP_ADDREQ_FLAG_ADD)) {
        WinHttpCloseHandle(hRequest);
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        return "";
    }

    if (!WinHttpSendRequest(hRequest,
        WINHTTP_NO_ADDITIONAL_HEADERS, 0,
        (LPVOID)postData.c_str(), postData.size(),
        postData.size(), 0)) {
        WinHttpCloseHandle(hRequest);
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        return "";
    }

    if (!WinHttpReceiveResponse(hRequest, NULL)) {
        WinHttpCloseHandle(hRequest);
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        return "";
    }

    while (WinHttpReadData(hRequest, buffer, sizeof(buffer) - 1, &bytesRead) && bytesRead > 0) {
        buffer[bytesRead] = '\0';
        responseData += buffer;
    }

    WinHttpCloseHandle(hRequest);
    WinHttpCloseHandle(hConnect);
    WinHttpCloseHandle(hSession);

    return responseData;
}