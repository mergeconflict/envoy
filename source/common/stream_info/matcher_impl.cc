#include "common/stream_info/matcher_impl.h"

namespace Envoy {
namespace StreamInfo {

MatcherImplBase::MatcherImplBase(bool matches) : matches_(matches) {}

bool MatcherImplBase::matches() const { return matches_; }

bool MatcherImplBase::canChange() const { return can_change_; }

AllOfMatcher::AllOfMatcher(std::vector<MatcherImplBase>&& matchers)
    : MatcherImplBase(std::all_of(matchers.begin(), matchers.end(),
                                  [](MatcherImplBase& matcher) { return matcher.matches(); })),
      matchers_(std::move(matchers)) {}

// MatcherImplBase
void AllOfMatcher::onRequestHeaders(const Http::HeaderMap& request_headers) {
  if (can_change_) {
    for (auto& matcher : matchers_) {
      matcher.onRequestHeaders(request_headers);
      if (!matcher.matches()) {
        matches_ = false;
        // TODO(mergeconflict): am i even thinking about this correctly?
      }
    }
  }
};
void AllOfMatcher::onRequestTrailers(const Http::HeaderMap& request_trailers) override;
void AllOfMatcher::onResponseHeaders(const Http::HeaderMap& response_headers) override;
void AllOfMatcher::onResponseTrailers(const Http::HeaderMap& response_trailers) override;

} // namespace StreamInfo
} // namespace Envoy
