/**
 * @file Response.hpp
 * @brief cnerium::http — HTTP response object
 *
 * @version 0.1.0
 * @author Gaspard Kirira
 * @copyright (c) 2026 Gaspard Kirira
 * @license MIT
 *
 * @details
 * Defines the high-level HTTP response object used by the Cnerium framework.
 *
 * Responsibilities:
 *   - Store response status
 *   - Store response headers
 *   - Store response body
 *   - Provide convenient helpers for text, HTML, and JSON responses
 *
 * Notes:
 *   - This is a framework-level response model.
 *   - Serialization to raw HTTP wire format belongs to the server layer.
 *   - JSON serialization is delegated to cnerium::json.
 *
 * Usage:
 * @code
 *   using namespace cnerium::http;
 *
 *   Response res;
 *   res.json({
 *     {"ok", true},
 *     {"framework", "Cnerium"}
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
    Response() = default;

    /**
     * @brief Construct a response with a specific status.
     *
     * @param status HTTP response status
     */
    explicit Response(Status status) noexcept
        : status_(status)
    {
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
     * @brief Sets the response body.
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
     * @brief Clears the full response state.
     *
     * Resets status to 200 OK and clears headers and body.
     */
    void clear()
    {
      status_ = Status::ok;
      headers_.clear();
      body_.clear();
    }

  private:
    Status status_{Status::ok};
    HeaderMap headers_{};
    std::string body_{};
  };

} // namespace cnerium::http
