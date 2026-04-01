/**
 * @file Response.hpp
 * @brief cnerium::http — HTTP response object
 *
 * @version 0.2.0
 * @author Gaspard Kirira
 * @copyright (c) 2026 Gaspard Kirira
 * @license MIT
 *
 * @details
 * Defines the high-level HTTP response object used by the Cnerium framework.
 *
 * Responsibilities:
 *   - store response status
 *   - store response headers
 *   - store response body
 *   - provide convenient helpers for text, HTML, and JSON responses
 *   - provide a framework signature header set
 *   - provide convenient response metadata helpers
 *
 * Notes:
 *   - This is a framework-level response model
 *   - Serialization to raw HTTP wire format belongs to the server layer
 *   - JSON serialization is delegated to cnerium::json
 *   - Content-Length is normally finalized by the response writer
 *
 * Usage:
 * @code
 *   using namespace cnerium::http;
 *
 *   Response res;
 *   res.json({
 *     {"ok", true},
 *     {"framework", "cnerium"}
 *   });
 *
 *   res.set_status(Status::created);
 * @endcode
 */

#pragma once

#include <string>
#include <string_view>
#include <utility>

#include <cnerium/http/HeaderMap.hpp>
#include <cnerium/http/Status.hpp>
#include <cnerium/json/json.hpp>

namespace cnerium::http
{
  /**
   * @brief High-level HTTP response object.
   *
   * Holds the response metadata and payload before transport-level serialization.
   */
  class Response
  {
  public:
    /// @brief Default constructor.
    Response()
    {
      apply_default_framework_headers();
    }

    /**
     * @brief Construct a response with a specific status.
     *
     * @param status HTTP response status
     */
    explicit Response(Status status) noexcept
        : status_(status)
    {
      apply_default_framework_headers();
    }

    /**
     * @brief Returns the response status.
     *
     * @return Status Current HTTP status
     */
    [[nodiscard]] Status status() const noexcept
    {
      return status_;
    }

    /**
     * @brief Sets the response status.
     *
     * @param status HTTP response status
     */
    void set_status(Status status) noexcept
    {
      status_ = status;
    }

    /**
     * @brief Returns the response body.
     *
     * @return std::string_view Response body
     */
    [[nodiscard]] std::string_view body() const noexcept
    {
      return body_;
    }

    /**
     * @brief Sets the raw response body.
     *
     * @param body Response body
     */
    void set_body(std::string body)
    {
      body_ = std::move(body);
    }

    /**
     * @brief Returns mutable access to response headers.
     *
     * @return HeaderMap& Mutable headers
     */
    [[nodiscard]] HeaderMap &headers() noexcept
    {
      return headers_;
    }

    /**
     * @brief Returns const access to response headers.
     *
     * @return const HeaderMap& Const headers
     */
    [[nodiscard]] const HeaderMap &headers() const noexcept
    {
      return headers_;
    }

    /**
     * @brief Sets or overwrites a response header.
     *
     * @param key Header name
     * @param value Header value
     */
    void set_header(std::string key, std::string value)
    {
      headers_.set(std::move(key), std::move(value));
    }

    /**
     * @brief Sets a response header only if it does not already exist.
     *
     * @param key Header name
     * @param value Header value
     * @return true if inserted, false if already present
     */
    bool try_set_header(std::string key, std::string value)
    {
      return headers_.try_set(std::move(key), std::move(value));
    }

    /**
     * @brief Removes a response header if present.
     *
     * @param key Header name
     * @return true if removed
     */
    bool remove_header(std::string_view key)
    {
      return headers_.erase(key);
    }

    /**
     * @brief Checks whether a header exists.
     *
     * @param key Header name
     * @return true if present
     */
    [[nodiscard]] bool has_header(std::string_view key) const noexcept
    {
      return headers_.contains(key);
    }

    /**
     * @brief Returns the value of a header.
     *
     * Returns an empty string_view if the header is missing.
     *
     * @param key Header name
     * @return std::string_view Header value
     */
    [[nodiscard]] std::string_view header(std::string_view key) const noexcept
    {
      return headers_.get(key);
    }

    /**
     * @brief Set the Content-Type header.
     *
     * @param value MIME type value
     * @return Response& Self for chaining
     */
    Response &content_type(std::string value)
    {
      headers_.set("Content-Type", std::move(value));
      return *this;
    }

    /**
     * @brief Set the Connection header.
     *
     * @param value Connection value, for example "close" or "keep-alive"
     * @return Response& Self for chaining
     */
    Response &connection(std::string value)
    {
      headers_.set("Connection", std::move(value));
      return *this;
    }

    /**
     * @brief Set the Server header.
     *
     * @param value Server signature
     * @return Response& Self for chaining
     */
    Response &server(std::string value)
    {
      headers_.set("Server", std::move(value));
      return *this;
    }

    /**
     * @brief Set the X-Powered-By header.
     *
     * @param value Framework/runtime signature
     * @return Response& Self for chaining
     */
    Response &powered_by(std::string value)
    {
      headers_.set("X-Powered-By", std::move(value));
      return *this;
    }

    /**
     * @brief Apply the default Cnerium framework signature headers.
     *
     * Sets:
     *   - Server: cnerium
     *   - X-Powered-By: cnerium
     *
     * Existing values are preserved if already present.
     *
     * @return Response& Self for chaining
     */
    Response &with_cnerium_signature()
    {
      headers_.try_set("Server", "cnerium");
      headers_.try_set("X-Powered-By", "cnerium");
      return *this;
    }

    /**
     * @brief Builds a plain text response body.
     *
     * Automatically sets:
     *   Content-Type: text/plain; charset=utf-8
     *
     * Does not modify the status code.
     *
     * @param body Plain text body
     * @return Response& Self for chaining
     */
    Response &text(std::string body)
    {
      body_ = std::move(body);
      headers_.set("Content-Type", "text/plain; charset=utf-8");
      return *this;
    }

    /**
     * @brief Builds an HTML response body.
     *
     * Automatically sets:
     *   Content-Type: text/html; charset=utf-8
     *
     * Does not modify the status code.
     *
     * @param body HTML body
     * @return Response& Self for chaining
     */
    Response &html(std::string body)
    {
      body_ = std::move(body);
      headers_.set("Content-Type", "text/html; charset=utf-8");
      return *this;
    }

    /**
     * @brief Builds a JSON response body from a const JSON value.
     *
     * Automatically sets:
     *   Content-Type: application/json; charset=utf-8
     *
     * Does not modify the status code.
     *
     * @param value JSON value to serialize
     * @param pretty Pretty-print JSON if true
     * @return Response& Self for chaining
     */
    Response &json(const cnerium::json::value &value, bool pretty = false)
    {
      body_ = value.dump(pretty);
      headers_.set("Content-Type", "application/json; charset=utf-8");
      return *this;
    }

    /**
     * @brief Builds a JSON response body from an rvalue JSON value.
     *
     * Automatically sets:
     *   Content-Type: application/json; charset=utf-8
     *
     * Does not modify the status code.
     *
     * @param value JSON value to serialize
     * @param pretty Pretty-print JSON if true
     * @return Response& Self for chaining
     */
    Response &json(cnerium::json::value &&value, bool pretty = false)
    {
      body_ = value.dump(pretty);
      headers_.set("Content-Type", "application/json; charset=utf-8");
      return *this;
    }

    /**
     * @brief Build a JSON success response helper.
     *
     * Example output:
     *   { "ok": true, "message": "..."}
     *
     * @param message Human-readable message
     * @return Response& Self for chaining
     */
    Response &ok(std::string message = "ok")
    {
      status_ = Status::ok;
      return json({{"ok", true},
                   {"message", std::move(message)},
                   {"framework", "cnerium"}});
    }

    /**
     * @brief Build a JSON error response helper.
     *
     * Example output:
     *   { "ok": false, "error": "..."}
     *
     * @param status HTTP error status
     * @param message Human-readable error message
     * @return Response& Self for chaining
     */
    Response &error(Status status, std::string message)
    {
      status_ = status;
      return json({{"ok", false},
                   {"error", std::move(message)},
                   {"framework", "cnerium"}});
    }

    /**
     * @brief Builds an empty response body.
     *
     * Clears the body and sets the provided status.
     * Does not remove existing headers except that the body becomes empty.
     *
     * @param status HTTP response status, default is no_content
     * @return Response& Self for chaining
     */
    Response &empty(Status status = Status::no_content)
    {
      status_ = status;
      body_.clear();
      return *this;
    }

    /**
     * @brief Returns true if the response body is empty.
     *
     * @return true if body is empty
     */
    [[nodiscard]] bool is_empty() const noexcept
    {
      return body_.empty();
    }

    /**
     * @brief Returns true if the response is JSON.
     *
     * @return true if Content-Type starts with application/json
     */
    [[nodiscard]] bool is_json() const noexcept
    {
      return starts_with_ci(header("Content-Type"), "application/json");
    }

    /**
     * @brief Returns true if the response requests connection close.
     *
     * @return true if Connection contains "close"
     */
    [[nodiscard]] bool connection_close() const noexcept
    {
      return header_contains_token(header("Connection"), "close");
    }

    /**
     * @brief Returns true if the response requests keep-alive.
     *
     * @return true if Connection contains "keep-alive"
     */
    [[nodiscard]] bool connection_keep_alive() const noexcept
    {
      return header_contains_token(header("Connection"), "keep-alive");
    }

    /**
     * @brief Clears the full response state.
     *
     * Resets status to 200 OK, clears headers and body,
     * then reapplies the default framework signature.
     */
    void clear()
    {
      status_ = Status::ok;
      headers_.clear();
      body_.clear();
      apply_default_framework_headers();
    }

  private:
    Status status_{Status::ok};
    HeaderMap headers_{};
    std::string body_{};

    /**
     * @brief Apply default framework signature headers.
     */
    void apply_default_framework_headers()
    {
      headers_.try_set("Server", "cnerium");
      headers_.try_set("X-Powered-By", "cnerium");
    }

    /**
     * @brief Return true if a comma-separated header value contains a token.
     *
     * Matching is case-insensitive and trims surrounding ASCII whitespace.
     *
     * @param value Raw header value
     * @param token Token to search
     * @return true if token is present
     */
    [[nodiscard]] static bool header_contains_token(
        std::string_view value,
        std::string_view token) noexcept
    {
      while (!value.empty())
      {
        const auto comma = value.find(',');
        const auto part = trim(
            comma == std::string_view::npos
                ? value
                : value.substr(0, comma));

        if (iequals(part, token))
        {
          return true;
        }

        if (comma == std::string_view::npos)
        {
          break;
        }

        value.remove_prefix(comma + 1);
      }

      return false;
    }

    /**
     * @brief Return true if a string starts with a prefix using ASCII case-insensitive comparison.
     *
     * @param value Full string
     * @param prefix Prefix to test
     * @return true if value starts with prefix
     */
    [[nodiscard]] static bool starts_with_ci(
        std::string_view value,
        std::string_view prefix) noexcept
    {
      if (value.size() < prefix.size())
      {
        return false;
      }

      for (std::size_t i = 0; i < prefix.size(); ++i)
      {
        if (ascii_lower(value[i]) != ascii_lower(prefix[i]))
        {
          return false;
        }
      }

      return true;
    }

    /**
     * @brief Trim ASCII whitespace from both ends of a string view.
     *
     * @param value Input view
     * @return std::string_view Trimmed view
     */
    [[nodiscard]] static std::string_view trim(std::string_view value) noexcept
    {
      while (!value.empty() &&
             (value.front() == ' ' || value.front() == '\t' ||
              value.front() == '\r' || value.front() == '\n'))
      {
        value.remove_prefix(1);
      }

      while (!value.empty() &&
             (value.back() == ' ' || value.back() == '\t' ||
              value.back() == '\r' || value.back() == '\n'))
      {
        value.remove_suffix(1);
      }

      return value;
    }

    /**
     * @brief Compare two ASCII strings case-insensitively.
     *
     * @param a First string
     * @param b Second string
     * @return true if equal ignoring ASCII case
     */
    [[nodiscard]] static bool iequals(
        std::string_view a,
        std::string_view b) noexcept
    {
      if (a.size() != b.size())
      {
        return false;
      }

      for (std::size_t i = 0; i < a.size(); ++i)
      {
        if (ascii_lower(a[i]) != ascii_lower(b[i]))
        {
          return false;
        }
      }

      return true;
    }

    /**
     * @brief Convert an ASCII character to lowercase.
     *
     * @param ch Input character
     * @return char Lowercased character
     */
    [[nodiscard]] static char ascii_lower(char ch) noexcept
    {
      const unsigned char c = static_cast<unsigned char>(ch);
      if (c >= 'A' && c <= 'Z')
      {
        return static_cast<char>(c - 'A' + 'a');
      }
      return static_cast<char>(c);
    }
  };

} // namespace cnerium::http
