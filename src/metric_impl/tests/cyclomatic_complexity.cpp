#include "metric_impl/cyclomatic_complexity.hpp"

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

TEST(CyclomaticComplexityMetricTest, EmptyFunctionHasBaseComplexityOne) 
{
    CyclomaticComplexityMetric metric;

    EXPECT_EQ(std::get<int>(metric.Calculate(MakeFunction("(function_definition)")).value), 1);
}

TEST(CyclomaticComplexityMetricTest, CountsIfStatement) 
{
    CyclomaticComplexityMetric metric;

    EXPECT_EQ(std::get<int>(metric.Calculate(MakeFunction("(function_definition (if_statement))")).value), 2);
}

TEST(CyclomaticComplexityMetricTest, CountsSeveralBranches) 
{
    CyclomaticComplexityMetric metric;

    const auto ast =
        "(function_definition "
        "(if_statement) "
        "(elif_statement) "
        "(for_statement) "
        "(while_statement))";

    EXPECT_EQ(std::get<int>(metric.Calculate(MakeFunction(ast)).value), 6);
}

TEST(CyclomaticComplexityMetricTest, CountsTryFinallyCaseAssertAndConditional)
{
    CyclomaticComplexityMetric metric;

    const auto ast =
        "(function_definition "
        "(try_statement) "
        "(finally_clause) "
        "(case_clause) "
        "(assert) "
        "(conditional_expression))";

    EXPECT_EQ(std::get<int>(metric.Calculate(MakeFunction(ast)).value), 6);
}


}  // namespace analyzer::metric::metric_impl
