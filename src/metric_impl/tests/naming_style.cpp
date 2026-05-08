#include "metric_impl/naming_style.hpp"

#include <gtest/gtest.h>

#include <string>
#include <variant>

namespace analyzer::metric::metric_impl {

static function::Function MakeFunction(std::string name) 
{
    function::Function f;
    f.name = std::move(name);
    f.filename = "test.py";
    f.ast = "";
    return f;
}

TEST(NamingStyleMetricTest, DetectsSnakeCase) 
{
    NamingStyleMetric metric;

    EXPECT_EQ(std::get<std::string>(metric.Calculate(MakeFunction("load_user_data")).value), "Snake Case");
}

TEST(NamingStyleMetricTest, DetectsCamelCase) 
{
    NamingStyleMetric metric;

    EXPECT_EQ(std::get<std::string>(metric.Calculate(MakeFunction("loadUserData")).value), "Camel Case");
}

TEST(NamingStyleMetricTest, DetectsPascalCase) 
{
    NamingStyleMetric metric;

    EXPECT_EQ(std::get<std::string>(metric.Calculate(MakeFunction("LoadUserData")).value), "Pascal Case");
}

TEST(NamingStyleMetricTest, DetectsLowerCase) 
{
    NamingStyleMetric metric;

    EXPECT_EQ(std::get<std::string>(metric.Calculate(MakeFunction("loaduserdata")).value), "Lower Case");
}

TEST(NamingStyleMetricTest, DetectsUnknownForHyphen) 
{
    NamingStyleMetric metric;

    EXPECT_EQ(std::get<std::string>(metric.Calculate(MakeFunction("load-user-data")).value), "Unknown");
}

}  // namespace analyzer::metric::metric_impl
