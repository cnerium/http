/**
 * @file version.hpp
 * @brief cnerium::http — Version definitions for the HTTP module
 *
 * @version 0.1.0
 * @author Gaspard Kirira
 * @copyright (c) 2026 Gaspard Kirira
 * @license MIT
 *
 * @details
 * Defines compile-time version information for the Cnerium HTTP module.
 *
 * This header provides:
 *   - Semantic version macros (major, minor, patch)
 *   - String representation of the version
 *   - Optional helpers for runtime access
 *
 * Usage:
 * @code
 *   #include <cnerium/http/version.hpp>
 *
 *   std::cout << CNERIUM_HTTP_VERSION << std::endl; // "0.1.0"
 * @endcode
 */

#pragma once

#include <string_view>

namespace cnerium::http
{
  /// @brief Major version of the HTTP module.
  inline constexpr int version_major = 0;

  /// @brief Minor version of the HTTP module.
  inline constexpr int version_minor = 1;

  /// @brief Patch version of the HTTP module.
  inline constexpr int version_patch = 0;

  /// @brief Full semantic version string.
  inline constexpr std::string_view version = "0.1.0";

  /**
   * @brief Returns the semantic version string.
   *
   * @return std::string_view Version string (e.g. "0.1.0")
   */
  [[nodiscard]] constexpr std::string_view version_string() noexcept
  {
    return version;
  }

  /**
   * @brief Returns version as a single integer for comparisons.
   *
   * Format: MAJOR * 10000 + MINOR * 100 + PATCH
   *
   * Example:
   *   0.1.0 -> 100
   *
   * @return int Encoded version number
   */
  [[nodiscard]] constexpr int version_number() noexcept
  {
    return version_major * 10000 + version_minor * 100 + version_patch;
  }

} // namespace cnerium::http
