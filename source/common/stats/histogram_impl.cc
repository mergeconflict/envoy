#include "common/stats/histogram_impl.h"

#include <algorithm>
#include <string>

#include "common/common/utility.h"

#include "absl/strings/str_join.h"

namespace Envoy {
namespace Stats {

HistogramStatisticsImpl::HistogramStatisticsImpl(const histogram_t* histogram_ptr)
    : computed_quantiles_(supportedQuantiles().size(), 0.0) {
  hist_approx_quantile(histogram_ptr, supportedQuantiles().data(), supportedQuantiles().size(),
                       computed_quantiles_.data());

  sample_count_ = hist_sample_count(histogram_ptr);
  sample_sum_ = hist_approx_sum(histogram_ptr);

  const std::vector<double>& supported_buckets = supportedBuckets();
  computed_buckets_.reserve(supported_buckets.size());
  for (const auto bucket : supported_buckets) {
    computed_buckets_.emplace_back(hist_approx_count_below(histogram_ptr, bucket));
  }
}

const std::vector<double>& HistogramStatisticsImpl::supportedQuantiles() const {
  static const std::vector<double> supported_quantiles = {0,    0.25, 0.5,   0.75,  0.90,
                                                          0.95, 0.99, 0.995, 0.999, 1};
  return supported_quantiles;
}

const std::vector<double>& HistogramStatisticsImpl::supportedBuckets() const {
  static const std::vector<double> supported_buckets = {
      0.5,  1,    5,     10,    25,    50,     100,    250,     500,    1000,
      2500, 5000, 10000, 30000, 60000, 300000, 600000, 1800000, 3600000};
  return supported_buckets;
}

/**
 * Clears the old computed values and refreshes it with values computed from passed histogram.
 */
void HistogramStatisticsImpl::refresh(const histogram_t* new_histogram_ptr) {
  std::fill(computed_quantiles_.begin(), computed_quantiles_.end(), 0.0);
  ASSERT(supportedQuantiles().size() == computed_quantiles_.size());
  hist_approx_quantile(new_histogram_ptr, supportedQuantiles().data(), supportedQuantiles().size(),
                       computed_quantiles_.data());

  sample_count_ = hist_sample_count(new_histogram_ptr);
  sample_sum_ = hist_approx_sum(new_histogram_ptr);

  ASSERT(supportedBuckets().size() == computed_buckets_.size());
  computed_buckets_.clear();
  const std::vector<double>& supported_buckets = supportedBuckets();
  computed_buckets_.reserve(supported_buckets.size());
  for (const auto bucket : supported_buckets) {
    computed_buckets_.emplace_back(hist_approx_count_below(new_histogram_ptr, bucket));
  }
}

ParentHistogramImpl::ParentHistogramImpl(const std::string& name, Store& parent,
                                         std::string&& tag_extracted_name, std::vector<Tag>&& tags)
    : MetricImpl(std::move(tag_extracted_name), std::move(tags)), parent_(parent),
      interval_histogram_(hist_alloc()), cumulative_histogram_(hist_alloc()),
      interval_statistics_(interval_histogram_), cumulative_statistics_(cumulative_histogram_),
      used_(false), name_(name) {}

ParentHistogramImpl::~ParentHistogramImpl() {
  hist_free(interval_histogram_);
  hist_free(cumulative_histogram_);
}

void ParentHistogramImpl::recordValue(uint64_t value) {
  used_ = true;
  hist_insert_intscale(interval_histogram_, value, 0, 1);
  parent_.deliverHistogramToSinks(*this, value);
}

void ParentHistogramImpl::merge() {
  hist_accumulate(cumulative_histogram_, &interval_histogram_, 1);
  interval_statistics_.refresh(interval_histogram_);
  cumulative_statistics_.refresh(cumulative_histogram_);
  hist_clear(interval_histogram_);
}

} // namespace Stats
} // namespace Envoy
