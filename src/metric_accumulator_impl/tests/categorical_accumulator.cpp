

#include "metric_accumulator_impl/categorical_accumulator.hpp"

#include <gtest/gtest.h>

#include <stdexcept>
#include <string>
#include <variant>

namespace analyzer::metric_accumulator::metric_accumulator_impl::test {

static metric::MetricResult MakeStringResult(const std::string& value) 
{
    return 
    {
        "naming_style",
        metric::MetricResult::ValueType{value}
    };
}

TEST(CategoricalAccumulatorTest, ThrowsBeforeFinalize) 
{
    CategoricalAccumulator accumulator;

    EXPECT_THROW(accumulator.Get(), std::runtime_error);
}

TEST(CategoricalAccumulatorTest, CountsCategoriesCorrectly) 
{
    CategoricalAccumulator accumulator;

    accumulator.Accumulate(MakeStringResult("Snake Case"));
    accumulator.Accumulate(MakeStringResult("Camel Case"));
    accumulator.Accumulate(MakeStringResult("Snake Case"));

    accumulator.Finalize();

    const auto& result = accumulator.Get();

    EXPECT_EQ(result.at("Snake Case"), 2);
    EXPECT_EQ(result.at("Camel Case"), 1);
}

TEST(CategoricalAccumulatorTest, ResetClearsState) 
{
    CategoricalAccumulator accumulator;

    accumulator.Accumulate(MakeStringResult("Pascal Case"));

    accumulator.Finalize();

    EXPECT_EQ(accumulator.Get().at("Pascal Case"), 1);

    accumulator.Reset();

    EXPECT_THROW(accumulator.Get(), std::runtime_error);

    accumulator.Accumulate(MakeStringResult("Lower Case"));

    accumulator.Finalize();

    EXPECT_FALSE(accumulator.Get().contains("Pascal Case"));
    EXPECT_EQ(accumulator.Get().at("Lower Case"), 1);
}


}  // namespace analyzer::metric_accumulator::metric_accumulator_impl::test
