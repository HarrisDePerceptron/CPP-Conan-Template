
#pragma once
#include <map>
#include <string>

struct HttpResponse {
  long status_code;
  std::string body;
  std::map<std::string, std::string> request_headers;
  std::map<std::string, std::string> response_headers;
};

class IHttpClient {
public:
  virtual ~IHttpClient() = default;
  virtual HttpResponse
  get(const std::string &url,
      const std::map<std::string, std::string> &headers = {}) = 0;

  virtual HttpResponse
  post(const std::string &url, const std::string &body,
       const std::map<std::string, std::string> &headers = {}) = 0;
};
