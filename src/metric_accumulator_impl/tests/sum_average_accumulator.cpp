#include "metric_accumulator_impl/sum_average_accumulator.hpp"

#include <gtest/gtest.h>

#include <stdexcept>
#include <variant>

namespace analyzer::metric_accumulator::metric_accumulator_impl::test {

static metric::MetricResult MakeIntResult(int value) 
{
    return {
        "test",
        metric::MetricResult::ValueType{value}
    };
}

TEST(SumAverageAccumulatorTest, ThrowsBeforeFinalize) 
{
    SumAverageAccumulator accumulator;

    EXPECT_THROW(accumulator.Get(), std::runtime_error);
}

TEST(SumAverageAccumulatorTest, CalculatesSumAndAverageCorrectly) 
{
    SumAverageAccumulator accumulator;

    accumulator.Accumulate(MakeIntResult(10));
    accumulator.Accumulate(MakeIntResult(20));
    accumulator.Accumulate(MakeIntResult(30));

    accumulator.Finalize();

    const auto result = accumulator.Get();

    EXPECT_EQ(result.sum, 60);
    EXPECT_DOUBLE_EQ(result.average, 20.0);
}

TEST(SumAverageAccumulatorTest, ResetClearsState) 
{
    SumAverageAccumulator accumulator;

    accumulator.Accumulate(MakeIntResult(7));
    accumulator.Accumulate(MakeIntResult(13));

    accumulator.Finalize();

    EXPECT_EQ(accumulator.Get().sum, 20);
    EXPECT_DOUBLE_EQ(accumulator.Get().average, 10.0);

    accumulator.Reset();

    EXPECT_THROW(accumulator.Get(), std::runtime_error);

    accumulator.Accumulate(MakeIntResult(50));

    accumulator.Finalize();

    EXPECT_EQ(accumulator.Get().sum, 50);
    EXPECT_DOUBLE_EQ(accumulator.Get().average, 50.0);
}


}  // namespace analyzer::metric_accumulator::metric_accumulator_impl::test
