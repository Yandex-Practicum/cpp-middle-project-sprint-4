#include "metric_impl/parameters_count.hpp"

#include <gtest/gtest.h>

#include <string>
#include <variant>

namespace analyzer::metric::metric_impl {

static function::Function MakeFunction(std::string ast) 
{
    function::Function f;
    f.name = "func";
    f.filename = "test.py";
    f.ast = std::move(ast);
    return f;
}

TEST(CountParametersMetricTest, NoParametersBlockReturnsZero) 
{
    CountParametersMetric metric;

    EXPECT_EQ(std::get<int>(metric.Calculate(MakeFunction("(function_definition)")).value), 0);
}

TEST(CountParametersMetricTest, EmptyParametersReturnsZero) 
{
    CountParametersMetric metric;

    EXPECT_EQ(std::get<int>(metric.Calculate(MakeFunction("(function_definition (parameters))")).value), 0);
}

TEST(CountParametersMetricTest, CountsOneParameter) 
{
    CountParametersMetric metric;

    const auto ast = "(function_definition (parameters (identifier)))";

    EXPECT_EQ(std::get<int>(metric.Calculate(MakeFunction(ast)).value), 1);
}

TEST(CountParametersMetricTest, CountsSeveralParameters) 
{
    CountParametersMetric metric;

    const auto ast =
        "(function_definition "
        "(parameters "
        "(identifier) "
        "(identifier) "
        "(identifier)))";

    EXPECT_EQ(std::get<int>(metric.Calculate(MakeFunction(ast)).value), 3);
}

}  // namespace analyzer::metric::metric_impl
