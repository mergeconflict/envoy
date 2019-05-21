#pragma once

#include "envoy/http/header_map.h"
#include "envoy/stream_info/matcher.h"

namespace Envoy {
namespace StreamInfo {

/**
 * TODO(mergeconflict): document me.
 */
class MatcherImplBase : public Matcher {
public:
  MatcherImplBase(bool matches);

  // StreamInfo::Matcher
  bool matches() const override;
  bool canChange() const override;

  /**
   * Update match status given HTTP request headers.
   * @param request_headers supplies the request headers.
   */
  virtual void onRequestHeaders(const Http::HeaderMap& request_headers) PURE;

  /**
   * Update match status given HTTP request trailers.
   * @param request_trailers supplies the request trailers.
   */
  virtual void onRequestTrailers(const Http::HeaderMap& request_trailers) PURE;

  /**
   * Update match status given HTTP response headers.
   * @param response_headers supplies the response headers.
   */
  virtual void onResponseHeaders(const Http::HeaderMap& response_headers) PURE;

  /**
   * Update match status given HTTP response trailers.
   * @param response_headers supplies the response trailers.
   */
  virtual void onResponseTrailers(const Http::HeaderMap& response_trailers) PURE;

protected:
  bool matches_{};
  bool can_change_{true};
};

class AllOfMatcher : public MatcherImplBase {
public:
  AllOfMatcher(std::vector<MatcherImplBase>&& matchers);

  // MatcherImplBase
  void onRequestHeaders(const Http::HeaderMap& request_headers) override;
  void onRequestTrailers(const Http::HeaderMap& request_trailers) override;
  void onResponseHeaders(const Http::HeaderMap& response_headers) override;
  void onResponseTrailers(const Http::HeaderMap& response_trailers) override;

private:
  std::vector<MatcherImplBase> matchers_;
};

} // namespace StreamInfo
} // namespace Envoy
