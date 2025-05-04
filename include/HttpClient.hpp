#pragma once
#include <algorithm>
#include <curl/curl.h>
#include <map>
#include <nlohmann/json.hpp>
#include <sstream>
#include <stdexcept>
#include <string>

#include "IHttpClient.hpp"

class HttpClient : public IHttpClient {
public:
  using Json = nlohmann::json;

  HttpClient() {
    curl = curl_easy_init();
    if (!curl)
      throw std::runtime_error("Failed to initialize CURL");
  }

  ~HttpClient() {
    if (curl)
      curl_easy_cleanup(curl);
  }

  HttpResponse get(const std::string &url,
                   const std::map<std::string, std::string> &headers = {}) {
    return performRequest("GET", url, "", headers);
  }

  HttpResponse post(const std::string &url, const std::string &body,
                    const std::map<std::string, std::string> &headers = {}) {
    return performRequest("POST", url, body, headers);
  }

  Json getJson(const std::string &url,
               const std::map<std::string, std::string> &headers = {}) {
    auto res = get(url, headers);
    return Json::parse(res.body);
  }

  Json postJson(const std::string &url, const Json &body,
                const std::map<std::string, std::string> &headers = {}) {
    auto res = post(url, body.dump(), headers);
    return Json::parse(res.body);
  }

private:
  CURL *curl;

  static size_t writeCallback(void *contents, size_t size, size_t nmemb,
                              std::string *output) {
    output->append((char *)contents, size * nmemb);
    return size * nmemb;
  }

  static size_t headerCallback(char *buffer, size_t size, size_t nitems,
                               void *userdata) {
    size_t total = size * nitems;
    std::string header_line(buffer, total);
    auto *headers = static_cast<std::map<std::string, std::string> *>(userdata);

    size_t colon = header_line.find(':');
    if (colon != std::string::npos) {
      std::string key = header_line.substr(0, colon);
      std::string value = header_line.substr(colon + 1);
      // trim whitespace
      key.erase(key.find_last_not_of(" \t\r\n") + 1);
      value.erase(0, value.find_first_not_of(" \t\r\n"));
      value.erase(value.find_last_not_of(" \t\r\n") + 1);
      headers->emplace(std::move(key), std::move(value));
    }
    return total;
  }

  HttpResponse
  performRequest(const std::string &method, const std::string &url,
                 const std::string &body,
                 const std::map<std::string, std::string> &headers) {
    std::string response;
    long status_code = 0;
    std::map<std::string, std::string> response_headers;

    curl_easy_reset(curl);
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, headerCallback);
    curl_easy_setopt(curl, CURLOPT_HEADERDATA, &response_headers);

    if (method == "POST") {
      curl_easy_setopt(curl, CURLOPT_POST, 1L);
      curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());
    }

    struct curl_slist *headerList = nullptr;
    for (const auto &[key, value] : headers) {
      std::string h = key + ": " + value;
      headerList = curl_slist_append(headerList, h.c_str());
    }

    if (headerList)
      curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerList);

    CURLcode res = curl_easy_perform(curl);
    if (headerList)
      curl_slist_free_all(headerList);

    if (res != CURLE_OK)
      throw std::runtime_error("CURL error: " +
                               std::string(curl_easy_strerror(res)));

    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &status_code);

    return HttpResponse{status_code, response, headers, response_headers};
  }
};
