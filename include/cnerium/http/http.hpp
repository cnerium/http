/**
 * @file http.hpp
 * @brief cnerium::http — Main public header for the HTTP module
 *
 * @version 0.1.0
 * @author Gaspard Kirira
 * @copyright (c) 2026 Gaspard Kirira
 * @license MIT
 *
 * @details
 * Aggregates the main public headers of the Cnerium HTTP module.
 *
 * Include this file when you want access to the full high-level HTTP API:
 *   - version information
 *   - HTTP methods
 *   - HTTP status codes
 *   - header container
 *   - request object
 *   - response object
 *
 * Usage:
 * @code
 *   #include <cnerium/http/http.hpp>
 *
 *   using namespace cnerium::http;
 *
 *   Request req;
 *   Response res;
 * @endcode
 */

#pragma once

#include <cnerium/http/version.hpp>
#include <cnerium/http/Method.hpp>
#include <cnerium/http/Status.hpp>
#include <cnerium/http/HeaderMap.hpp>
#include <cnerium/http/Request.hpp>
#include <cnerium/http/Response.hpp>
