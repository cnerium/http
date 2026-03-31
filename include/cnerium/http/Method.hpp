/**
 * @file Method.hpp
 * @brief cnerium::http — HTTP method definitions
 *
 * @version 0.1.0
 * @author Gaspard Kirira
 * @copyright (c) 2026 Gaspard Kirira
 * @license MIT
 *
 * @details
 * Defines HTTP methods and provides utilities for conversion
 * between enum values and string representations.
 *
 * Designed for high-performance routing and request handling.
 *
 * Usage:
 * @code
 *   using namespace cnerium::http;
 *
 *   Method m = Method::Get;
 *   std::string_view name = to_string(m); // "GET"
 *
 *   Method parsed = method_from_string("POST");
 * @endcode
 */

#pragma once

#include <string_view>
#include <cstdint>

#if defined(__GNUC__) || defined(__clang__)
#define CNERIUM_LIKELY(x) __builtin_expect(!!(x), 1)
#define CNERIUM_UNLIKELY(x) __builtin_expect(!!(x), 0)
#else
#define CNERIUM_LIKELY(x) (x)
#define CNERIUM_UNLIKELY(x) (x)
#endif

namespace cnerium::http
{
  /**
   * @brief HTTP request method.
   *
   * Covers all standard methods defined in RFC 9110.
   */
  enum class Method : uint8_t
  {
    Get,
    Post,
    Put,
    Patch,
    Delete,
    Head,
    Options,
    Trace,
    Connect,
    Unknown
  };

  /**
   * @brief Convert Method to its string representation.
   *
   * @param m HTTP method enum
   * @return std::string_view Uppercase method name
   */
  [[nodiscard]] constexpr std::string_view to_string(Method m) noexcept
  {
    switch (m)
    {
    case Method::Get:
      return "GET";
    case Method::Post:
      return "POST";
    case Method::Put:
      return "PUT";
    case Method::Patch:
      return "PATCH";
    case Method::Delete:
      return "DELETE";
    case Method::Head:
      return "HEAD";
    case Method::Options:
      return "OPTIONS";
    case Method::Trace:
      return "TRACE";
    case Method::Connect:
      return "CONNECT";
    default:
      return "UNKNOWN";
    }
  }

  /**
   * @brief Parse HTTP method from string.
   *
   * Fast comparison without allocation.
   *
   * @param s Input string (case-sensitive, expected uppercase)
   * @return Method enum (Unknown if not recognized)
   */
  [[nodiscard]] constexpr Method method_from_string(std::string_view s) noexcept
  {
    // Fast length-based dispatch (important for perf)
    switch (s.size())
    {
    case 3:
      if (s == "GET")
        return Method::Get;
      if (s == "PUT")
        return Method::Put;
      break;

    case 4:
      if (s == "POST")
        return Method::Post;
      if (s == "HEAD")
        return Method::Head;
      break;

    case 5:
      if (s == "PATCH")
        return Method::Patch;
      if (s == "TRACE")
        return Method::Trace;
      break;

    case 6:
      if (s == "DELETE")
        return Method::Delete;
      break;

    case 7:
      if (s == "OPTIONS")
        return Method::Options;
      if (s == "CONNECT")
        return Method::Connect;
      break;
    }

    return Method::Unknown;
  }

  /**
   * @brief Check if method typically has a request body.
   *
   * @param m HTTP method
   * @return true if method usually carries a body
   */
  [[nodiscard]] constexpr bool has_request_body(Method m) noexcept
  {
    switch (m)
    {
    case Method::Post:
    case Method::Put:
    case Method::Patch:
    case Method::Delete:
      return true;
    default:
      return false;
    }
  }

  /**
   * @brief Check if method is idempotent.
   *
   * @param m HTTP method
   * @return true if idempotent (safe for retries)
   */
  [[nodiscard]] constexpr bool is_idempotent(Method m) noexcept
  {
    switch (m)
    {
    case Method::Get:
    case Method::Put:
    case Method::Delete:
    case Method::Head:
    case Method::Options:
    case Method::Trace:
      return true;
    default:
      return false;
    }
  }

} // namespace cnerium::http
