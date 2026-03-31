/**
 * @file 02_response_json.cpp
 * @brief JSON response example for cnerium/http
 *
 * This example demonstrates:
 *   - building structured JSON responses
 *   - pretty vs compact serialization
 *   - nested JSON objects and arrays
 */

#include <iostream>

#include <cnerium/http/http.hpp>

int main()
{
  using namespace cnerium::http;
  using namespace cnerium::json;

  // -------------------------
  // Build JSON data
  // -------------------------
  value data = object{
      {"framework", "Cnerium"},
      {"runtime", "Vix"},
      {"version", "0.1.0"},
      {"features", array{
                       "HTTP",
                       "JSON",
                       "Router (coming soon)"}},
      {"meta", object{{"author", "Gaspard"}, {"active", true}}}};

  // -------------------------
  // Response with compact JSON
  // -------------------------
  Response res_compact;
  res_compact.json(data);

  std::cout << "=== Compact JSON ===\n";
  std::cout << res_compact.body() << "\n\n";

  // -------------------------
  // Response with pretty JSON
  // -------------------------
  Response res_pretty;
  res_pretty.set_status(Status::ok);
  res_pretty.json(data, true);

  std::cout << "=== Pretty JSON ===\n";
  std::cout << res_pretty.body() << "\n\n";

  // -------------------------
  // Manual construction example
  // -------------------------
  Response res_manual;
  res_manual.set_status(Status::created);
  res_manual.json({{"ok", true},
                   {"message", "Resource created"},
                   {"data", data}},
                  true);

  std::cout << "=== Manual JSON Response ===\n";
  std::cout << to_int(res_manual.status())
            << ' ' << reason_phrase(res_manual.status()) << "\n";
  std::cout << res_manual.body() << "\n";

  return 0;
}
