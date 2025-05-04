#include "../include/HttpClient.hpp" // Adjust path if needed
#include <catch2/catch_test_macros.hpp>
#include <nlohmann/json.hpp>

TEST_CASE("GET request returns status, body, and headers",
          "[HttpClient][GET]") {
  HttpClient client;
  HttpResponse res = client.get("https://httpbin.org/get");

  REQUIRE(res.status_code == 200);
  REQUIRE_FALSE(res.body.empty());
  REQUIRE(res.response_headers.contains("Content-Type"));
  REQUIRE_NOTHROW(nlohmann::json::parse(res.body));
}

TEST_CASE("POST request includes request headers and receives JSON back",
          "[HttpClient][POST]") {
  HttpClient client;
  HttpClient::Json payload = {{"key", "value"}, {"count", 42}};

  std::map<std::string, std::string> headers = {
      {"Content-Type", "application/json"}, {"X-Test-Header", "Catch2-Test"}};

  HttpResponse res =
      client.post("https://httpbin.org/post", payload.dump(), headers);

  REQUIRE(res.status_code == 200);
  REQUIRE_FALSE(res.body.empty());

  auto parsed = nlohmann::json::parse(res.body);
  REQUIRE(parsed.contains("json"));
  REQUIRE(parsed["json"]["key"] == "value");
  REQUIRE(parsed["json"]["count"] == 42);

  // Check echoed headers in response
  REQUIRE(res.request_headers.contains("X-Test-Header"));
  REQUIRE(res.request_headers["Content-Type"] == "application/json");
}

TEST_CASE("GET JSON returns parsed JSON structure", "[HttpClient][getJson]") {
  HttpClient client;
  auto json = client.getJson("https://httpbin.org/json");

  REQUIRE(json.contains("slideshow"));
  REQUIRE(json["slideshow"].contains("title"));
  REQUIRE(json["slideshow"]["title"].is_string());
}

TEST_CASE("Handles 404 status code gracefully", "[HttpClient][Error]") {
  HttpClient client;
  HttpResponse res = client.get("https://httpbin.org/status/404");

  REQUIRE(res.status_code == 404);
}

TEST_CASE("Throws on DNS failure or invalid domain",
          "[HttpClient][Exception]") {
  HttpClient client;

  REQUIRE_THROWS_AS(client.get("http://nonexistent.invalid"),
                    std::runtime_error);
}
