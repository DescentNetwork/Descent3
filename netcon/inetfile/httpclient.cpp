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

#include "httpclient.h"

#include <curl/curl.h>

namespace D3 {

namespace {

struct WriteContext {
  const HttpClient::ContentReceiver *receiver = nullptr;
  std::string *body = nullptr;
};

size_t WriteCallback(char *ptr, size_t size, size_t nmemb, void *userdata) {
  WriteContext *ctx = static_cast<WriteContext *>(userdata);
  const size_t length = size * nmemb;
  if (ctx->receiver != nullptr && !(*ctx->receiver)(ptr, length)) {
    return 0; // Abort transfer
  }
  if (ctx->body != nullptr) {
    ctx->body->append(ptr, length);
  }
  return length;
}

struct ProgressContext {
  const HttpClient::Progress *progress = nullptr;
};

int ProgressCallback(void *userdata, curl_off_t total_to_download, curl_off_t now_downloaded, curl_off_t,
                     curl_off_t) {
  ProgressContext *ctx = static_cast<ProgressContext *>(userdata);
  if (ctx->progress != nullptr &&
      !(*ctx->progress)(static_cast<uint64_t>(now_downloaded), static_cast<uint64_t>(total_to_download))) {
    return 1; // Abort transfer
  }
  return 0;
}

HttpClient::Error MapError(CURLcode code) {
  switch (code) {
    case CURLE_OK:
      return HttpClient::Error::Success;
    case CURLE_OPERATION_TIMEDOUT:
      return HttpClient::Error::Timeout;
    case CURLE_TOO_MANY_REDIRECTS:
      return HttpClient::Error::TooManyRedirects;
    case CURLE_COULDNT_CONNECT:
    case CURLE_COULDNT_RESOLVE_HOST:
    case CURLE_COULDNT_RESOLVE_PROXY:
      return HttpClient::Error::ConnectionError;
    default:
      return HttpClient::Error::NetworkError;
  }
}

} // namespace

HttpClient::HttpClient(const std::string &URL) : m_base_url(URL) {}

HttpClient::~HttpClient() = default;

HttpClient::Result HttpClient::Get(const std::string &URIPath) { return perform(URIPath, nullptr, nullptr); }

HttpClient::Result HttpClient::Get(const std::string &URIPath, const Progress &progress) {
  return perform(URIPath, nullptr, &progress);
}

HttpClient::Result HttpClient::Get(const std::string &URIPath, const ContentReceiver &content_receiver) {
  return perform(URIPath, &content_receiver, nullptr);
}

HttpClient::Result HttpClient::Get(const std::string &URIPath, const ContentReceiver &content_receiver,
                                   const Progress &progress) {
  return perform(URIPath, &content_receiver, &progress);
}

HttpClient::Result HttpClient::perform(const std::string &URIPath, const ContentReceiver *content_receiver,
                                       const Progress *progress) {
  Result result;

  CURL *curl = curl_easy_init();
  if (curl == nullptr) {
    result.err = Error::NetworkError;
    return result;
  }

  std::string url = m_base_url;
  if (!URIPath.empty()) {
    if (URIPath[0] != '/') {
      url += '/';
    }
    url += URIPath;
  }

  curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
  curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
  curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 5L);
  curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);
  curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, ""); // Enable gzip/deflate decoding

  if (!m_proxy.empty()) {
    curl_easy_setopt(curl, CURLOPT_PROXY, m_proxy.c_str());
  }

  WriteContext write_ctx;
  write_ctx.receiver = content_receiver;
  write_ctx.body = content_receiver ? nullptr : &result.body;
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &write_ctx);

  ProgressContext progress_ctx;
  progress_ctx.progress = progress;
  if (progress) {
    curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
    curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, ProgressCallback);
    curl_easy_setopt(curl, CURLOPT_XFERINFODATA, &progress_ctx);
  }

  const CURLcode code = curl_easy_perform(curl);
  result.err = MapError(code);
  if (code == CURLE_OK) {
    long status = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &status);
    result.status = static_cast<int>(status);
  }
  curl_easy_cleanup(curl);
  return result;
}

void HttpClient::SetProxy(const std::string &proxy_host, uint16_t port) {
  m_proxy = proxy_host + ":" + std::to_string(port);
}

const char *HttpClient::StatusMessage(int status) {
  switch (status) {
    case 200:
      return "OK";
    case 301:
      return "Moved Permanently";
    case 302:
      return "Found";
    case 304:
      return "Not Modified";
    case 400:
      return "Bad Request";
    case 401:
      return "Unauthorized";
    case 403:
      return "Forbidden";
    case 404:
      return "Not Found";
    case 408:
      return "Request Timeout";
    case 429:
      return "Too Many Requests";
    case 500:
      return "Internal Server Error";
    case 502:
      return "Bad Gateway";
    case 503:
      return "Service Unavailable";
    case 504:
      return "Gateway Timeout";
    default:
      return "Unknown";
  }
}

} // namespace D3
