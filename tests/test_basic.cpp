/**
 * @file test_basic.cpp
 * @brief Basic integration tests for cnerium::http
 */

#include <iostream>
#include <string>

#include <cnerium/http/http.hpp>

#define OK(x)                                                  \
  do                                                           \
  {                                                            \
    if (!(x))                                                  \
    {                                                          \
      std::cerr << "FAIL " #x "  (line " << __LINE__ << ")\n"; \
      std::exit(1);                                            \
    }                                                          \
    else                                                       \
    {                                                          \
      std::cout << "OK   " #x "\n";                            \
    }                                                          \
  } while (0)

using namespace cnerium::http;
using namespace cnerium::json;

static void suite(const char *name)
{
  std::cout << "\n=== " << name << " ===\n";
}

void test_request_response_basic()
{
  suite("http.basic");

  // -------------------------
  // Request
  // -------------------------
  Request req;
  req.set_method(Method::Post);
  req.set_path("/users");
  req.set_header("Content-Type", "application/json");
  req.set_body(R"({"name":"Gaspard","age":25})");

  OK(req.method() == Method::Post);
  OK(req.path() == "/users");
  OK(req.has_header("content-type"));
  OK(req.header("Content-Type") == "application/json");

  auto data = req.json();
  OK(data["name"].as_string() == "Gaspard");
  OK(data["age"].as_integer() == 25);

  // -------------------------
  // Response text
  // -------------------------
  Response res;
  res.text("Hello");

  OK(res.status() == Status::ok);
  OK(res.body() == "Hello");
  OK(res.header("Content-Type") == "text/plain; charset=utf-8");

  // -------------------------
  // Response JSON
  // -------------------------
  Response res2;
  res2.json({{"ok", true},
             {"framework", "Cnerium"}});

  OK(res2.status() == Status::ok);
  OK(res2.has_header("content-type"));

  auto parsed = parse(res2.body());
  OK(parsed["ok"].as_bool() == true);
  OK(parsed["framework"].as_string() == "Cnerium");

  // -------------------------
  // Response status change
  // -------------------------
  res2.set_status(Status::created);
  OK(res2.status() == Status::created);

  // -------------------------
  // HeaderMap behavior
  // -------------------------
  HeaderMap h;
  h.set("X-Test", "123");
  h.set("x-test", "456"); // overwrite

  OK(h.size() == 1);
  OK(h.get("X-Test") == "456");
  OK(h.contains("x-test"));

  h.erase("X-Test");
  OK(h.empty());

  // -------------------------
  // Method helpers
  // -------------------------
  OK(to_string(Method::Get) == "GET");
  OK(method_from_string("POST") == Method::Post);
  OK(has_request_body(Method::Post));
  OK(is_idempotent(Method::Get));

  // -------------------------
  // Status helpers
  // -------------------------
  OK(to_int(Status::ok) == 200);
  OK(reason_phrase(Status::not_found) == "Not Found");
  OK(is_success(Status::ok));
  OK(is_client_error(Status::bad_request));
}

int main()
{
  test_request_response_basic();

  std::cout << "\nAll tests passed.\n";
  return 0;
}
