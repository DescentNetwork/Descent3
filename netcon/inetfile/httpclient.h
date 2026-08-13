/*
 * Descent 3
 * Copyright (C) 2024 Descent Developers
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <cstdint>
#include <functional>
#include <string>

namespace D3 {

class HttpClient {
public:
  enum class Error {
    Success = 0,
    ConnectionError,
    Timeout,
    TooManyRedirects,
    NetworkError,
  };

  using Progress = std::function<bool(uint64_t current, uint64_t total)>;
  using ContentReceiver = std::function<bool(const char *data, size_t length)>;

  struct Result {
    Error err = Error::Success;
    int status = 0;
    std::string body;

    Error error() const { return err; }

    const Result *operator->() const { return this; }
  };

  /**
   * Constructor for HttpClient
   * @param URL request URL, should be in form of http://example.com. Don't add "/" on the end as it's part of URIPath.
   * Port can be specified with ":" (http://example.com:8080)
   */
  explicit HttpClient(const std::string &URL);
  ~HttpClient();

  Result Get(const std::string &URIPath);

  /**
   * Get data from URIPath
   * @param URIPath requested path (for example, "/some_dir/my_file.txt")
   * @return standard HTTP code. 200 means is OK.
   */
  Result Get(const std::string &URIPath, const Progress &progress);

  Result Get(const std::string &URIPath, const ContentReceiver &content_receiver);

  Result Get(const std::string &URIPath, const ContentReceiver &content_receiver, const Progress &progress);

  void SetProxy(const std::string &proxy_host, uint16_t port);

  static const char *StatusMessage(int status);

private:
  Result perform(const std::string &URIPath, const ContentReceiver *content_receiver, const Progress *progress);

  std::string m_base_url;
  std::string m_proxy;
};

}
