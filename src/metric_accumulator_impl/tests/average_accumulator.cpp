#include "metric_accumulator_impl/average_accumulator.hpp"

#include <gtest/gtest.h>

#include <stdexcept>
#include <variant>

namespace analyzer::metric_accumulator::metric_accumulator_impl::test {

static metric::MetricResult MakeIntResult(int value) 
{
    return 
    {
        "test",
        metric::MetricResult::ValueType{value}
    };
}

TEST(AverageAccumulatorTest, ThrowsBeforeFinalize) 
{
    AverageAccumulator accumulator;

    EXPECT_THROW(accumulator.Get(), std::runtime_error);
}

TEST(AverageAccumulatorTest, CalculatesAverageCorrectly) 
{
    AverageAccumulator accumulator;

    accumulator.Accumulate(MakeIntResult(10));
    accumulator.Accumulate(MakeIntResult(20));
    accumulator.Accumulate(MakeIntResult(30));

    accumulator.Finalize();

    EXPECT_DOUBLE_EQ(accumulator.Get(), 20.0);
}

TEST(AverageAccumulatorTest, ResetClearsState) 
{
    AverageAccumulator accumulator;

    accumulator.Accumulate(MakeIntResult(5));
    accumulator.Accumulate(MakeIntResult(15));

    accumulator.Finalize();

    EXPECT_DOUBLE_EQ(accumulator.Get(), 10.0);

    accumulator.Reset();

    EXPECT_THROW(accumulator.Get(), std::runtime_error);

    accumulator.Accumulate(MakeIntResult(100));

    accumulator.Finalize();

    EXPECT_DOUBLE_EQ(accumulator.Get(), 100.0);
}

}  // namespace analyzer::metric_accumulator::metric_accumulator_impl::test
