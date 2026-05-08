#include "metric_impl/code_lines_count.hpp"

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

TEST(CodeLinesCountMetricTest, CountsCodeLinesWithoutDeclarationLine) 
{
    CodeLinesCountMetric metric;

    const auto ast =
        "(function_definition [0,0] - [3,0]\n"
        "  body: (block [1,4] - [3,0]\n"
        "    (expression_statement [1,4] - [1,10])\n"
        "    (return_statement [2,4] - [2,12])))";

    EXPECT_EQ(std::get<int>(metric.Calculate(MakeFunction(ast)).value), 3);
}

TEST(CodeLinesCountMetricTest, IgnoresCommentLine) 
{
    CodeLinesCountMetric metric;

    const auto ast =
        "(function_definition [0,0] - [4,0]\n"
        "  body: (block [1,4] - [4,0]\n"
        "    (expression_statement [1,4] - [1,10])\n"
        "    (comment [2,4] - [2,12])\n"
        "    (return_statement [3,4] - [3,12])))";

    EXPECT_EQ(std::get<int>(metric.Calculate(MakeFunction(ast)).value), 3);
}


}  // namespace analyzer::metric::metric_impl
