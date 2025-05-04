#include "HttpClient.hpp"
#include <iostream>

int main2() {
  HttpClient client;

  try {
    auto response = client.get("https://httpbin.org/get");

    std::cout << "Status Code: " << response.status_code << "\n";
    std::cout << "Body:\n" << response.body << "\n";

    auto json = client.getJson("https://httpbin.org/json");
    std::cout << "Parsed Title: " << json["slideshow"]["title"] << "\n";
  } catch (const std::exception &ex) {
    std::cerr << "Request failed: " << ex.what() << "\n";
  }

  auto res = client.get("https://httpbin.org/headers");

  std::cout << "Status: " << res.status_code << "\n";

  for (const auto &[k, v] : res.response_headers)
    std::cout << k << ": " << v << "\n";

  for (const auto &[k, v] : res.request_headers)
    std::cout << "[Sent] " << k << ": " << v << "\n";

  return 0;
}
