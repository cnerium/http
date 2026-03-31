/**
 * @file Status.hpp
 * @brief cnerium::http — HTTP status code definitions
 *
 * @version 0.1.0
 * @author Gaspard Kirira
 * @copyright (c) 2026 Gaspard Kirira
 * @license MIT
 *
 * @details
 * Defines HTTP response status codes and provides utilities
 * for converting them to integers and reason phrases.
 *
 * Designed for HTTP request handling, response building,
 * and future integration with the Cnerium router/server stack.
 *
 * Usage:
 * @code
 *   using namespace cnerium::http;
 *
 *   Status s = Status::ok;
 *   int code = to_int(s);                    // 200
 *   auto text = reason_phrase(s);            // "OK"
 * @endcode
 */

#pragma once

#include <cstdint>
#include <string_view>

namespace cnerium::http
{
  /**
   * @brief HTTP response status code.
   *
   * Covers the most commonly used HTTP status codes for
   * web applications, APIs, and services.
   */
  enum class Status : std::uint16_t
  {
    // 1xx Informational
    continue_ = 100,
    switching_protocols = 101,
    processing = 102,

    // 2xx Success
    ok = 200,
    created = 201,
    accepted = 202,
    no_content = 204,

    // 3xx Redirection
    moved_permanently = 301,
    found = 302,
    not_modified = 304,
    temporary_redirect = 307,
    permanent_redirect = 308,

    // 4xx Client Error
    bad_request = 400,
    unauthorized = 401,
    forbidden = 403,
    not_found = 404,
    method_not_allowed = 405,
    conflict = 409,
    gone = 410,
    payload_too_large = 413,
    unsupported_media_type = 415,
    unprocessable_content = 422,
    too_many_requests = 429,

    // 5xx Server Error
    internal_server_error = 500,
    not_implemented = 501,
    bad_gateway = 502,
    service_unavailable = 503,
    gateway_timeout = 504
  };

  /**
   * @brief Convert Status to its numeric HTTP code.
   *
   * @param status HTTP status enum
   * @return int Numeric HTTP status code
   */
  [[nodiscard]] constexpr int to_int(Status status) noexcept
  {
    return static_cast<int>(status);
  }

  /**
   * @brief Returns the standard HTTP reason phrase for a status.
   *
   * @param status HTTP status enum
   * @return std::string_view Standard reason phrase
   */
  [[nodiscard]] constexpr std::string_view reason_phrase(Status status) noexcept
  {
    switch (status)
    {
    case Status::continue_:
      return "Continue";
    case Status::switching_protocols:
      return "Switching Protocols";
    case Status::processing:
      return "Processing";

    case Status::ok:
      return "OK";
    case Status::created:
      return "Created";
    case Status::accepted:
      return "Accepted";
    case Status::no_content:
      return "No Content";

    case Status::moved_permanently:
      return "Moved Permanently";
    case Status::found:
      return "Found";
    case Status::not_modified:
      return "Not Modified";
    case Status::temporary_redirect:
      return "Temporary Redirect";
    case Status::permanent_redirect:
      return "Permanent Redirect";

    case Status::bad_request:
      return "Bad Request";
    case Status::unauthorized:
      return "Unauthorized";
    case Status::forbidden:
      return "Forbidden";
    case Status::not_found:
      return "Not Found";
    case Status::method_not_allowed:
      return "Method Not Allowed";
    case Status::conflict:
      return "Conflict";
    case Status::gone:
      return "Gone";
    case Status::payload_too_large:
      return "Payload Too Large";
    case Status::unsupported_media_type:
      return "Unsupported Media Type";
    case Status::unprocessable_content:
      return "Unprocessable Content";
    case Status::too_many_requests:
      return "Too Many Requests";

    case Status::internal_server_error:
      return "Internal Server Error";
    case Status::not_implemented:
      return "Not Implemented";
    case Status::bad_gateway:
      return "Bad Gateway";
    case Status::service_unavailable:
      return "Service Unavailable";
    case Status::gateway_timeout:
      return "Gateway Timeout";
    }

    return "Unknown Status";
  }

  /**
   * @brief Returns true if the status is informational (1xx).
   */
  [[nodiscard]] constexpr bool is_informational(Status status) noexcept
  {
    const int code = to_int(status);
    return code >= 100 && code < 200;
  }

  /**
   * @brief Returns true if the status is successful (2xx).
   */
  [[nodiscard]] constexpr bool is_success(Status status) noexcept
  {
    const int code = to_int(status);
    return code >= 200 && code < 300;
  }

  /**
   * @brief Returns true if the status is a redirection (3xx).
   */
  [[nodiscard]] constexpr bool is_redirection(Status status) noexcept
  {
    const int code = to_int(status);
    return code >= 300 && code < 400;
  }

  /**
   * @brief Returns true if the status is a client error (4xx).
   */
  [[nodiscard]] constexpr bool is_client_error(Status status) noexcept
  {
    const int code = to_int(status);
    return code >= 400 && code < 500;
  }

  /**
   * @brief Returns true if the status is a server error (5xx).
   */
  [[nodiscard]] constexpr bool is_server_error(Status status) noexcept
  {
    const int code = to_int(status);
    return code >= 500 && code < 600;
  }

} // namespace cnerium::http
