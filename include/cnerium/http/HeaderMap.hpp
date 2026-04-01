/**
 * @file HeaderMap.hpp
 * @brief cnerium::http — HTTP header container
 *
 * @version 0.2.0
 * @author Gaspard Kirira
 * @copyright (c) 2026 Gaspard Kirira
 * @license MIT
 *
 * @details
 * Defines a lightweight HTTP header container for request and response objects.
 *
 * Responsibilities:
 *   - store ordered HTTP headers
 *   - provide case-insensitive lookup
 *   - preserve insertion order
 *   - allow insertion, replacement, removal, and iteration
 *   - expose convenient helpers for common header access patterns
 *
 * Design goals:
 *   - Simple and predictable API
 *   - Preserve insertion order
 *   - Good cache locality for small header sets
 *   - No external dependencies
 *
 * Notes:
 *   - This implementation uses a flat vector of key/value pairs
 *   - Header lookup is linear, which is acceptable for the typical
 *     number of HTTP headers in most applications
 *   - Header name matching is case-insensitive, as required by HTTP
 *   - Setting an existing header replaces its first matching value
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
 *
 *   headers.erase("X-App");
 * @endcode
 */

#pragma once

#include <cstddef>
#include <initializer_list>
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
     * @brief Construct from an initializer list of headers.
     *
     * Existing keys are normalized by replacement semantics:
     * the first matching key is kept and updated.
     *
     * @code
     *   HeaderMap h{
     *     {"Content-Type", "application/json"},
     *     {"X-Test", "1"}
     *   };
     * @endcode
     *
     * @param init Initial header entries
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
     * Otherwise a new header entry is appended, preserving insertion order.
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
     * @brief Insert a header only if it does not already exist.
     *
     * @param key Header name
     * @param value Header value
     * @return true if inserted, false if a matching header already exists
     */
    bool try_set(std::string key, std::string value)
    {
      if (contains(key))
      {
        return false;
      }

      headers_.emplace_back(std::move(key), std::move(value));
      return true;
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
      {
        return {};
      }

      return headers_[idx].second;
    }

    /**
     * @brief Get a header value or fallback.
     *
     * @param key Header name
     * @param fallback Value returned if header does not exist
     * @return std::string_view Header value or fallback
     */
    [[nodiscard]] std::string_view get_or(
        std::string_view key,
        std::string_view fallback) const noexcept
    {
      const auto idx = find_index(key);
      if (idx == npos())
      {
        return fallback;
      }

      return headers_[idx].second;
    }

    /**
     * @brief Remove a header by name.
     *
     * Only the first matching header is removed.
     *
     * @param key Header name
     * @return true if removed
     */
    bool erase(std::string_view key)
    {
      const auto idx = find_index(key);
      if (idx == npos())
      {
        return false;
      }

      headers_.erase(headers_.begin() + static_cast<std::ptrdiff_t>(idx));
      return true;
    }

    /**
     * @brief Remove all headers matching a given name.
     *
     * @param key Header name
     * @return std::size_t Number of removed headers
     */
    std::size_t erase_all(std::string_view key)
    {
      std::size_t removed = 0;

      for (auto it = headers_.begin(); it != headers_.end();)
      {
        if (iequals(it->first, key))
        {
          it = headers_.erase(it);
          ++removed;
        }
        else
        {
          ++it;
        }
      }

      return removed;
    }

    /// @brief Remove all headers.
    void clear() noexcept
    {
      headers_.clear();
    }

    /**
     * @brief Reserve storage for headers.
     *
     * @param n Desired capacity
     */
    void reserve(std::size_t n)
    {
      headers_.reserve(n);
    }

    /**
     * @brief Number of stored headers.
     *
     * @return std::size_t Header count
     */
    [[nodiscard]] std::size_t size() const noexcept
    {
      return headers_.size();
    }

    /**
     * @brief Returns true if no headers are stored.
     *
     * @return true if empty
     */
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

    /**
     * @brief Direct access to the underlying storage.
     *
     * @return const storage_type& Stored header entries
     */
    [[nodiscard]] const storage_type &data() const noexcept
    {
      return headers_;
    }

    /**
     * @brief Mutable access to the underlying storage.
     *
     * @return storage_type& Stored header entries
     */
    [[nodiscard]] storage_type &data() noexcept
    {
      return headers_;
    }

  private:
    storage_type headers_{};

    /**
     * @brief Sentinel value used when a header index is not found.
     *
     * @return std::size_t Invalid index
     */
    [[nodiscard]] static constexpr std::size_t npos() noexcept
    {
      return static_cast<std::size_t>(-1);
    }

    /**
     * @brief Convert one ASCII character to lowercase.
     *
     * @param c Input character
     * @return char Lowercased character
     */
    [[nodiscard]] static char ascii_lower(char c) noexcept
    {
      const unsigned char ch = static_cast<unsigned char>(c);

      if (ch >= 'A' && ch <= 'Z')
      {
        return static_cast<char>(ch - 'A' + 'a');
      }

      return static_cast<char>(ch);
    }

    /**
     * @brief Compare two strings case-insensitively using ASCII rules.
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
     * @brief Find the index of a header by name.
     *
     * Header name comparison is case-insensitive.
     *
     * @param key Header name
     * @return std::size_t Matching index, or npos() if missing
     */
    [[nodiscard]] std::size_t find_index(std::string_view key) const noexcept
    {
      for (std::size_t i = 0; i < headers_.size(); ++i)
      {
        if (iequals(headers_[i].first, key))
        {
          return i;
        }
      }

      return npos();
    }

    /**
     * @brief Find the value string of a header by name.
     *
     * @param key Header name
     * @return std::string* Pointer to value if found, otherwise nullptr
     */
    [[nodiscard]] std::string *find_value(std::string_view key) noexcept
    {
      for (auto &[k, v] : headers_)
      {
        if (iequals(k, key))
        {
          return &v;
        }
      }

      return nullptr;
    }
  };

} // namespace cnerium::http
