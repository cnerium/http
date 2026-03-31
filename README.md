# cnerium/http

HTTP primitives for the Cnerium web framework.

**Header-only. Lightweight. Framework-ready.**

---

## Download

https://vixcpp.com/registry/pkg/cnerium/http

---

## Overview

`cnerium/http` provides the foundational HTTP types used across the Cnerium ecosystem:

- request methods
- status codes
- headers
- request objects
- response objects

It is designed to be:

- minimal
- reusable
- independent from transport layers

No server required.
No router required.
No middleware required.

Just the HTTP core.

---

## Why cnerium/http?

Most projects end up re-implementing:

- request methods
- status codes
- header storage
- request containers
- response builders
- JSON helpers

These concerns are often:

- mixed with transport code
- duplicated across modules
- hard to test
- inconsistent

`cnerium/http` fixes this by providing a clean, shared HTTP foundation.

---

## Dependency

Depends on:

- `cnerium/json`

This ensures:

- shared JSON model
- consistent parsing
- consistent serialization

---

## Installation

### Using Vix

```bash
vix add @cnerium/http
vix install
```

### Manual

```bash
git clone https://github.com/cnerium/http.git
```

Add `include/` to your project.

---

## Core Concepts

### HTTP Methods

```cpp
using namespace cnerium::http;

Method m = Method::Post;

if (m == Method::Post)
{
  // handle POST
}
```

Convert:

```cpp
auto name = to_string(Method::Get);
auto parsed = method_from_string("PATCH");
```

Helpers:

```cpp
has_request_body(Method::Post);
is_idempotent(Method::Get);
```

---

### Status Codes

```cpp
Status s = Status::created;

int code = to_int(s);
auto text = reason_phrase(s);
```

Helpers:

```cpp
is_success(Status::ok);
is_client_error(Status::bad_request);
is_server_error(Status::internal_server_error);
```

---

### Headers

```cpp
HeaderMap headers;
headers.set("Content-Type", "application/json");

if (headers.contains("content-type"))
{
  auto v = headers.get("CONTENT-TYPE");
}
```

Case-insensitive lookup.

---

### Request

```cpp
Request req;
req.set_method(Method::Post);
req.set_path("/users");
req.set_body(R"({"name":"Gaspard"})");

auto data = req.json();
```

---

### Response

```cpp
Response res;
res.set_status(Status::ok);
res.text("Hello");
```

HTML:

```cpp
res.html("<h1>Hello</h1>");
```

JSON:

```cpp
res.json({
  {"ok", true}
});
```

---

## Typical Flow

```cpp
Request req;
req.set_method(Method::Post);
req.set_path("/api/users");
req.set_body(R"({"name":"Alice"})");

auto payload = req.json();

Response res;
res.set_status(Status::created);
res.json({
  {"ok", true},
  {"name", payload["name"]}
});
```

---

## Example

```cpp
#include <cnerium/http/http.hpp>

using namespace cnerium::http;

int main()
{
  Request req;
  req.set_method(Method::Post);
  req.set_path("/users");
  req.set_body(R"({"name":"Gaspard","age":25})");

  auto data = req.json();

  Response res;
  res.set_status(Status::created);
  res.json({
    {"ok", true},
    {"name", data["name"]},
    {"age", data["age"]}
  });

  return 0;
}
```

---

## Complexity

| Operation | Complexity |
|----------|-----------|
| header lookup | O(n) |
| header insertion | amortized O(1) |
| request body access | O(1) |
| response body assignment | O(n) |

---

## Design Philosophy

- minimal HTTP primitives
- no transport coupling
- predictable behavior
- framework-friendly
- clean JSON integration

---

## Tests

```bash
vix build
vix test
```

---

## License

MIT License
Copyright (c) Gaspard Kirira

