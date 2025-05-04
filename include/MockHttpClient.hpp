#pragma once
#include "IHttpClient.hpp"

class MockHttpClient : public IHttpClient {
public:
  HttpResponse
  get(const std::string &url,
      const std::map<std::string, std::string> &headers = {}) override {
    return HttpResponse{200,
                        R"({"message":"mocked GET"})",
                        headers,
                        {{"Content-Type", "application/json"}}};
  }

  HttpResponse
  post(const std::string &url, const std::string &body,
       const std::map<std::string, std::string> &headers = {}) override {
    return HttpResponse{201,
                        R"({"message":"mocked POST","body":)" + body + "}",
                        headers,
                        {{"Content-Type", "application/json"}}};
  }
};
