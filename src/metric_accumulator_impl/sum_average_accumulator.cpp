#include "metric_accumulator_impl/sum_average_accumulator.hpp"

#include <unistd.h>

#include <algorithm>
#include <array>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <ranges>
#include <sstream>
#include <string>
#include <variant>
#include <vector>
#include <stdexcept>

namespace analyzer::metric_accumulator::metric_accumulator_impl {

void SumAverageAccumulator::Accumulate(const metric::MetricResult &metric_result) {
    sum += std::get<int>(metric_result.value);
    count++;
}
void SumAverageAccumulator::Finalize() {
    average = count == 0 ? 0.0 : static_cast<double>(sum) / count;
    is_finalized = true;
}

void SumAverageAccumulator::Reset() {
    is_finalized = false;
    sum = 0;
    count = 0;
    average = 0;
}

SumAverageAccumulator::SumAverage SumAverageAccumulator::Get() const {
    if (!is_finalized)
        throw std::runtime_error("SumAverageAccumulator::Get() called before Finalize()");
    return {sum, average};
}
}  // namespace analyzer::metric_accumulator::metric_accumulator_impl
