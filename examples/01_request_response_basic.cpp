/**
 * @file 01_request_response_basic.cpp
 * @brief Basic request/response example for cnerium/http
 *
 * This example demonstrates:
 *   - building a Request
 *   - accessing method, path, headers, and body
 *   - parsing the request body as JSON
 *   - building a Response
 *   - generating a JSON response body
 */

#include <iostream>

#include <cnerium/http/http.hpp>

int main()
{
  using namespace cnerium::http;

  Request req;
  req.set_method(Method::Post);
  req.set_path("/users");
  req.set_header("Content-Type", "application/json");
  req.set_body(R"({"name":"Gaspard","age":25})");

  std::cout << "Request\n";
  std::cout << "  Method: " << to_string(req.method()) << '\n';
  std::cout << "  Path:   " << req.path() << '\n';
  std::cout << "  Type:   " << req.header("Content-Type") << '\n';
  std::cout << "  Body:   " << req.body() << '\n';

  auto payload = req.json();

  Response res;
  res.set_status(Status::created);
  res.json({{"ok", true},
            {"message", "User created"},
            {"user", {{"name", payload["name"]}, {"age", payload["age"]}}}},
           true);

  std::cout << "\nResponse\n";
  std::cout << "  Status: " << to_int(res.status())
            << ' ' << reason_phrase(res.status()) << '\n';
  std::cout << "  Type:   " << res.header("Content-Type") << '\n';
  std::cout << "  Body:\n"
            << res.body() << '\n';

  return 0;
}
