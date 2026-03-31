/**
 * @file Request.hpp
 * @brief cnerium::http — HTTP request object
 *
 * @version 0.1.0
 * @author Gaspard Kirira
 * @copyright (c) 2026 Gaspard Kirira
 * @license MIT
 *
 * @details
 * Defines the high-level HTTP request object used by the Cnerium framework.
 *
 * Responsibilities:
 *   - Store request method
 *   - Store request target path
 *   - Store request headers
 *   - Store request body
 *   - Provide convenient header access
 *   - Parse request body as JSON when needed
 *
 * Notes:
 *   - This is a lightweight request model for framework-level handling.
 *   - Parsing of the raw HTTP wire format belongs to the server layer,
 *     not to this class.
 *   - JSON parsing is delegated to cnerium::json.
 *
 * Usage:
 * @code
 *   using namespace cnerium::http;
 *
 *   Request req;
 *   req.set_method(Method::Post);
 *   req.set_path("/users");
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
        : method_(method), path_(std::move(path))
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
     * Example: /users/42
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
     * Resets method to GET and clears path, headers, and body.
     */
    void clear()
    {
      method_ = Method::Get;
      path_.clear();
      headers_.clear();
      body_.clear();
    }

  private:
    Method method_{Method::Get};
    std::string path_{};
    HeaderMap headers_{};
    std::string body_{};
  };

} // namespace cnerium::http
