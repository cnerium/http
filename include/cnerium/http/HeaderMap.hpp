/**
 * @file HeaderMap.hpp
 * @brief cnerium::http — HTTP header container
 *
 * @version 0.1.0
 * @author Gaspard Kirira
 * @copyright (c) 2026 Gaspard Kirira
 * @license MIT
 *
 * @details
 * Defines a lightweight HTTP header container for request and response objects.
 *
 * Design goals:
 *   - Simple and predictable API
 *   - Preserve insertion order
 *   - Good cache locality for small header sets
 *   - No external dependencies
 *
 * Notes:
 *   - This v1 implementation uses a flat vector of key/value pairs.
 *   - Header lookup is currently linear, which is acceptable for the
 *     typical number of HTTP headers in most applications.
 *   - Header name matching is case-insensitive, as required by HTTP.
 *
 * Usage:
 * @code
 *   using namespace cnerium::http;
 *
 *   HeaderMap headers;
 *   headers.set("Content-Type", "application/json");
 *   headers.set("X-App", "Cnerium");
 *
 *   if (headers.contains("content-type"))
 *   {
 *     auto value = headers.get("CONTENT-TYPE");
 *   }
 * @endcode
 */

#pragma once

#include <algorithm>
#include <cctype>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace cnerium::http
{
  /**
   * @brief Ordered container of HTTP headers.
   *
   * Stores headers as a vector of key/value pairs.
   * Keys are compared case-insensitively.
   */
  class HeaderMap
  {
  public:
    using value_type = std::pair<std::string, std::string>;
    using storage_type = std::vector<value_type>;
    using iterator = storage_type::iterator;
    using const_iterator = storage_type::const_iterator;

    /// @brief Default constructor.
    HeaderMap() = default;

    /**
     * @brief Construct from initializer list of headers.
     *
     * @code
     *   HeaderMap h{
     *     {"Content-Type", "application/json"},
     *     {"X-Test", "1"}
     *   };
     * @endcode
     */
    HeaderMap(std::initializer_list<value_type> init)
    {
      headers_.reserve(init.size());
      for (const auto &entry : init)
      {
        set(entry.first, entry.second);
      }
    }

    /**
     * @brief Insert or overwrite a header.
     *
     * If a matching header name already exists, its value is replaced.
     *
     * @param key Header name
     * @param value Header value
     */
    void set(std::string key, std::string value)
    {
      if (auto *existing = find_value(key))
      {
        *existing = std::move(value);
        return;
      }

      headers_.emplace_back(std::move(key), std::move(value));
    }

    /**
     * @brief Check whether a header exists.
     *
     * Header name comparison is case-insensitive.
     *
     * @param key Header name
     * @return true if found
     */
    [[nodiscard]] bool contains(std::string_view key) const noexcept
    {
      return find_index(key) != npos();
    }

    /**
     * @brief Get a header value.
     *
     * @param key Header name
     * @return std::string_view Header value, or empty view if missing
     */
    [[nodiscard]] std::string_view get(std::string_view key) const noexcept
    {
      const auto idx = find_index(key);
      if (idx == npos())
        return {};
      return headers_[idx].second;
    }

    /**
     * @brief Get a header value or fallback.
     *
     * @param key Header name
     * @param fallback Value returned if header does not exist
     * @return std::string_view Header value or fallback
     */
    [[nodiscard]] std::string_view get_or(std::string_view key,
                                          std::string_view fallback) const noexcept
    {
      const auto idx = find_index(key);
      if (idx == npos())
        return fallback;
      return headers_[idx].second;
    }

    /**
     * @brief Remove a header.
     *
     * @param key Header name
     * @return true if removed
     */
    bool erase(std::string_view key)
    {
      const auto idx = find_index(key);
      if (idx == npos())
        return false;

      headers_.erase(headers_.begin() + static_cast<std::ptrdiff_t>(idx));
      return true;
    }

    /// @brief Remove all headers.
    void clear() noexcept
    {
      headers_.clear();
    }

    /// @brief Reserve storage for headers.
    void reserve(std::size_t n)
    {
      headers_.reserve(n);
    }

    /// @brief Number of stored headers.
    [[nodiscard]] std::size_t size() const noexcept
    {
      return headers_.size();
    }

    /// @brief Returns true if no headers are stored.
    [[nodiscard]] bool empty() const noexcept
    {
      return headers_.empty();
    }

    /// @brief Mutable begin iterator.
    [[nodiscard]] iterator begin() noexcept
    {
      return headers_.begin();
    }

    /// @brief Mutable end iterator.
    [[nodiscard]] iterator end() noexcept
    {
      return headers_.end();
    }

    /// @brief Const begin iterator.
    [[nodiscard]] const_iterator begin() const noexcept
    {
      return headers_.begin();
    }

    /// @brief Const end iterator.
    [[nodiscard]] const_iterator end() const noexcept
    {
      return headers_.end();
    }

    /// @brief Const begin iterator.
    [[nodiscard]] const_iterator cbegin() const noexcept
    {
      return headers_.cbegin();
    }

    /// @brief Const end iterator.
    [[nodiscard]] const_iterator cend() const noexcept
    {
      return headers_.cend();
    }

    /// @brief Direct access to underlying storage.
    [[nodiscard]] const storage_type &data() const noexcept
    {
      return headers_;
    }

    /// @brief Mutable access to underlying storage.
    [[nodiscard]] storage_type &data() noexcept
    {
      return headers_;
    }

  private:
    storage_type headers_{};

    static constexpr std::size_t npos() noexcept
    {
      return static_cast<std::size_t>(-1);
    }

    [[nodiscard]] static char ascii_lower(char c) noexcept
    {
      if (c >= 'A' && c <= 'Z')
        return static_cast<char>(c - 'A' + 'a');
      return c;
    }

    [[nodiscard]] static bool iequals(std::string_view a,
                                      std::string_view b) noexcept
    {
      if (a.size() != b.size())
        return false;

      for (std::size_t i = 0; i < a.size(); ++i)
      {
        if (ascii_lower(a[i]) != ascii_lower(b[i]))
          return false;
      }
      return true;
    }

    [[nodiscard]] std::size_t find_index(std::string_view key) const noexcept
    {
      for (std::size_t i = 0; i < headers_.size(); ++i)
      {
        if (iequals(headers_[i].first, key))
          return i;
      }
      return npos();
    }

    [[nodiscard]] std::string *find_value(std::string_view key) noexcept
    {
      for (auto &[k, v] : headers_)
      {
        if (iequals(k, key))
          return &v;
      }
      return nullptr;
    }
  };

} // namespace cnerium::http
