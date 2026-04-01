/**
 * @file Request.hpp
 * @brief cnerium::http — HTTP request object
 *
 * @version 0.2.0
 * @author Gaspard Kirira
 * @copyright (c) 2026 Gaspard Kirira
 * @license MIT
 *
 * @details
 * Defines the high-level HTTP request object used by the Cnerium framework.
 *
 * Responsibilities:
 *   - store request method
 *   - store request target path
 *   - store request query string
 *   - store request headers
 *   - store request body
 *   - provide convenient header access
 *   - provide convenient request-target helpers
 *   - parse request body as JSON when needed
 *
 * Notes:
 *   - This is a lightweight request model for framework-level handling
 *   - Parsing of the raw HTTP wire format belongs to the server layer,
 *     not to this class
 *   - JSON parsing is delegated to cnerium::json
 *   - Connection persistence decisions are handled by higher layers
 *
 * Usage:
 * @code
 *   using namespace cnerium::http;
 *
 *   Request req;
 *   req.set_method(Method::Post);
 *   req.set_path("/users");
 *   req.set_query("active=true");
 *   req.set_header("Content-Type", "application/json");
 *   req.set_body(R"({"name":"Gaspard"})");
 *
 *   auto data = req.json();
 *   auto name = data["name"].as_string();
 * @endcode
 */

#pragma once

#include <string>
#include <string_view>
#include <utility>

#include <cnerium/http/HeaderMap.hpp>
#include <cnerium/http/Method.hpp>
#include <cnerium/json/json.hpp>

namespace cnerium::http
{
  /**
   * @brief High-level HTTP request object.
   *
   * Holds the request metadata and payload after transport-level parsing.
   */
  class Request
  {
  public:
    /// @brief Default constructor.
    Request() = default;

    /**
     * @brief Construct a request with method and path.
     *
     * @param method HTTP method
     * @param path Request path
     */
    Request(Method method, std::string path)
        : method_(method),
          path_(std::move(path))
    {
    }

    /**
     * @brief Construct a request with method, path, and query string.
     *
     * @param method HTTP method
     * @param path Request path
     * @param query Raw query string without the leading '?'
     */
    Request(Method method, std::string path, std::string query)
        : method_(method),
          path_(std::move(path)),
          query_(std::move(query))
    {
    }

    /**
     * @brief Returns the HTTP method.
     *
     * @return Method Request method
     */
    [[nodiscard]] Method method() const noexcept
    {
      return method_;
    }

    /**
     * @brief Sets the HTTP method.
     *
     * @param method Request method
     */
    void set_method(Method method) noexcept
    {
      method_ = method;
    }

    /**
     * @brief Returns the request path.
     *
     * Example:
     *   /users/42
     *
     * @return std::string_view Request path
     */
    [[nodiscard]] std::string_view path() const noexcept
    {
      return path_;
    }

    /**
     * @brief Sets the request path.
     *
     * @param path Request path
     */
    void set_path(std::string path)
    {
      path_ = std::move(path);
    }

    /**
     * @brief Returns the raw query string.
     *
     * The returned value does not include the leading '?'.
     *
     * Example:
     *   active=true&page=2
     *
     * @return std::string_view Raw query string
     */
    [[nodiscard]] std::string_view query() const noexcept
    {
      return query_;
    }

    /**
     * @brief Sets the raw query string.
     *
     * The provided value must not include the leading '?'.
     *
     * @param query Raw query string
     */
    void set_query(std::string query)
    {
      query_ = std::move(query);
    }

    /**
     * @brief Returns true if the request has a query string.
     *
     * @return true if query is not empty
     */
    [[nodiscard]] bool has_query() const noexcept
    {
      return !query_.empty();
    }

    /**
     * @brief Returns the full request target.
     *
     * This reconstructs:
     *   - path            when query is empty
     *   - path?query      when query exists
     *
     * @return std::string Full request target
     */
    [[nodiscard]] std::string target() const
    {
      if (query_.empty())
      {
        return path_;
      }

      std::string out;
      out.reserve(path_.size() + 1 + query_.size());
      out += path_;
      out.push_back('?');
      out += query_;
      return out;
    }

    /**
     * @brief Returns the request body.
     *
     * @return std::string_view Request body
     */
    [[nodiscard]] std::string_view body() const noexcept
    {
      return body_;
    }

    /**
     * @brief Sets the request body.
     *
     * @param body Request body
     */
    void set_body(std::string body)
    {
      body_ = std::move(body);
    }

    /**
     * @brief Returns mutable access to the request headers.
     *
     * @return HeaderMap& Mutable header map
     */
    [[nodiscard]] HeaderMap &headers() noexcept
    {
      return headers_;
    }

    /**
     * @brief Returns const access to the request headers.
     *
     * @return const HeaderMap& Const header map
     */
    [[nodiscard]] const HeaderMap &headers() const noexcept
    {
      return headers_;
    }

    /**
     * @brief Checks whether a header exists.
     *
     * Header name comparison is case-insensitive.
     *
     * @param key Header name
     * @return true if present
     */
    [[nodiscard]] bool has_header(std::string_view key) const noexcept
    {
      return headers_.contains(key);
    }

    /**
     * @brief Sets or overwrites a header.
     *
     * @param key Header name
     * @param value Header value
     */
    void set_header(std::string key, std::string value)
    {
      headers_.set(std::move(key), std::move(value));
    }

    /**
     * @brief Removes a header if present.
     *
     * @param key Header name
     */
    void remove_header(std::string_view key)
    {
      headers_.erase(key);
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
     * @brief Returns true if the request advertises Connection: close.
     *
     * @return true if the Connection header contains the token "close"
     */
    [[nodiscard]] bool connection_close() const noexcept
    {
      return header_contains_token(header("Connection"), "close");
    }

    /**
     * @brief Returns true if the request advertises Connection: keep-alive.
     *
     * @return true if the Connection header contains the token "keep-alive"
     */
    [[nodiscard]] bool connection_keep_alive() const noexcept
    {
      return header_contains_token(header("Connection"), "keep-alive");
    }

    /**
     * @brief Parses the request body as JSON.
     *
     * @throws cnerium::json::parse_error if the body is not valid JSON
     *
     * @return cnerium::json::value Parsed JSON value
     */
    [[nodiscard]] cnerium::json::value json() const
    {
      return cnerium::json::parse(body_);
    }

    /**
     * @brief Returns true if the request body is empty.
     *
     * @return true if body is empty
     */
    [[nodiscard]] bool empty() const noexcept
    {
      return body_.empty();
    }

    /**
     * @brief Clears the request content.
     *
     * Resets method to GET and clears path, query, headers, and body.
     */
    void clear()
    {
      method_ = Method::Get;
      path_.clear();
      query_.clear();
      headers_.clear();
      body_.clear();
    }

  private:
    Method method_{Method::Get};
    std::string path_{};
    std::string query_{};
    HeaderMap headers_{};
    std::string body_{};

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
