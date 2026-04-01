/**
 * @file Method.hpp
 * @brief cnerium::http — HTTP method definitions
 *
 * @version 0.2.0
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
 * Responsibilities:
 *   - define supported HTTP request methods
 *   - convert Method values to uppercase string names
 *   - parse method tokens without allocation
 *   - expose small semantic helpers for HTTP behavior
 *
 * Notes:
 *   - Parsing is case-sensitive and expects uppercase method names
 *   - Unknown methods map to Method::Unknown
 *   - Semantic helpers follow common HTTP expectations
 *
 * Usage:
 * @code
 *   using namespace cnerium::http;
 *
 *   Method m = Method::Get;
 *   std::string_view name = to_string(m); // "GET"
 *
 *   Method parsed = method_from_string("POST");
 *   bool safe = is_safe(Method::Head);
 * @endcode
 */

#pragma once

#include <cstdint>
#include <string_view>

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
   * Covers the standard methods currently supported by the framework.
   */
  enum class Method : std::uint8_t
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
   * @param method HTTP method enum
   * @return std::string_view Uppercase method name
   */
  [[nodiscard]] constexpr std::string_view to_string(Method method) noexcept
  {
    switch (method)
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
    case Method::Unknown:
    default:
      return "UNKNOWN";
    }
  }

  /**
   * @brief Parse HTTP method from string.
   *
   * Fast comparison without allocation.
   *
   * Parsing is case-sensitive and expects uppercase method names.
   *
   * @param text Input string
   * @return Method enum, or Method::Unknown if not recognized
   */
  [[nodiscard]] constexpr Method method_from_string(std::string_view text) noexcept
  {
    switch (text.size())
    {
    case 3:
      if (text == "GET")
        return Method::Get;
      if (text == "PUT")
        return Method::Put;
      break;

    case 4:
      if (text == "POST")
        return Method::Post;
      if (text == "HEAD")
        return Method::Head;
      break;

    case 5:
      if (text == "PATCH")
        return Method::Patch;
      if (text == "TRACE")
        return Method::Trace;
      break;

    case 6:
      if (text == "DELETE")
        return Method::Delete;
      break;

    case 7:
      if (text == "OPTIONS")
        return Method::Options;
      if (text == "CONNECT")
        return Method::Connect;
      break;

    default:
      break;
    }

    return Method::Unknown;
  }

  /**
   * @brief Return true if the method is known.
   *
   * @param method HTTP method
   * @return true if method is not Unknown
   */
  [[nodiscard]] constexpr bool is_known(Method method) noexcept
  {
    return method != Method::Unknown;
  }

  /**
   * @brief Check if method typically has a request body.
   *
   * @param method HTTP method
   * @return true if method usually carries a body
   */
  [[nodiscard]] constexpr bool has_request_body(Method method) noexcept
  {
    switch (method)
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
   * @brief Check if method is safe.
   *
   * Safe methods do not intend to modify server state.
   *
   * @param method HTTP method
   * @return true if safe
   */
  [[nodiscard]] constexpr bool is_safe(Method method) noexcept
  {
    switch (method)
    {
    case Method::Get:
    case Method::Head:
    case Method::Options:
    case Method::Trace:
      return true;
    default:
      return false;
    }
  }

  /**
   * @brief Check if method is idempotent.
   *
   * Idempotent methods can be retried without changing the
   * intended final state beyond the first successful call.
   *
   * @param method HTTP method
   * @return true if idempotent
   */
  [[nodiscard]] constexpr bool is_idempotent(Method method) noexcept
  {
    switch (method)
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
