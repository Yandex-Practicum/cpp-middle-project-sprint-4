#include <unistd.h>

#include <algorithm>
#include <array>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <print>
#include <ranges>
#include <sstream>
#include <string>
#include <variant>
#include <vector>

#include "file.hpp"
#include "function.hpp"
#include "metric.hpp"
#include "metric_accumulator.hpp"

namespace analyzer {

namespace rv = std::ranges::views;
namespace rs = std::ranges;

using FunctionAnalysis = std::pair<function::Function, metric::MetricResults>;
using Analysis = std::vector<FunctionAnalysis>;
using GroupedAnalysis = std::vector<Analysis>;

/**
 * @brief Анализирует список Python-файлов и извлекает метрики для всех функций и методов.
 *
 * Эта функция — центральный "конвейер" обработки:
 * 1. Принимает имена файлов.
 * 2. Для каждого файла создаёт объект `File`, который автоматически парсит его через tree-sitter
 *    и строит AST.
 * 3. Извлекает из AST все функции и методы с помощью `FunctionExtractor`.
 * 4. Объединяет все функции из всех файлов в один плоский список (`join`).
 * 5. Для каждой функции вычисляет набор метрик через переданный `metric_extractor`.
 * 6. Возвращает вектор пар: (функция, результаты её метрик).
 */
inline Analysis AnalyseFunctions(
    const std::vector<std::string>& files,
    const metric::MetricExtractor& metric_extractor)
{
    Analysis result;

    std::ranges::for_each(files, [&](const auto& filename) {
        const file::File file{filename};

        function::FunctionExtractor extractor;
        const auto functions = extractor.Get(file);

        std::ranges::transform(
            functions,
            std::back_inserter(result),
            [&](const auto& func) {
                return FunctionAnalysis{func, metric_extractor.Get(func)};
            });
    });

    return result;
}

/**
 * 
 * @brief Группирует результаты анализа по классам.
 *
 * Эта функция:
 * 1. Отфильтровывает только те функции, которые являются **методами классов**
 *    (у них `class_name.has_value()` == true).
 * 2. Группирует последовательные элементы с одинаковым именем класса с помощью `chunk_by`.
 *
 * Важно:
 * - `chunk_by` работает только с **последовательными** одинаковыми элементами!
 *   Поэтому предполагается, что входной диапазон уже упорядочен по классам
 *   (например, порядок методов в AST сохраняется как в исходном файле).
 * - Если порядок нарушен, один и тот же класс может быть разбит на несколько групп.
 *
 *  Чтобы убедиться, что фильтрация работает, проверьте, что свободные функции (без class_name)
 * действительно исчезают из результата.
 */
inline GroupedAnalysis SplitByClasses(const Analysis& analysis)
{
    GroupedAnalysis result;

    std::ranges::for_each(analysis, [&](const auto& item) {
        if (!item.first.class_name.has_value()) {
            return;
        }

        if (result.empty() ||
            result.back().empty() ||
            result.back().front().first.class_name != item.first.class_name) {
            result.emplace_back();
        }

        result.back().push_back(item);
    });

    return result;
}

/**
 * @brief Группирует результаты анализа по исходным файлам.
 *
 * Эта функция:
 * - Разбивает весь список функций на группы, где каждая группа содержит
 *   только функции из одного и того же файла (`filename`).
 * - Использует `chunk_by`, поэтому **порядок функций в `analysis` должен быть по файлам**.
 */
inline GroupedAnalysis SplitByFiles(const Analysis& analysis)
{
    GroupedAnalysis result;

    std::ranges::for_each(analysis, [&](const auto& item) {
        if (result.empty() ||
            result.back().empty() ||
            result.back().front().first.filename != item.first.filename) {
            result.emplace_back();
        }

        result.back().push_back(item);
    });

    return result;
}

/**
 * @brief Агрегирует метрики всех функций с помощью аккумулятора.
 *
 * Эта функция:
 * - Проходит по каждому элементу результата `AnalyseFunctions`
 *   (то есть по каждой функции и её метрикам).
 * - Передаёт результаты метрик (`elem.second`) в аккумулятор через `AccumulateNextFunctionResults`.
 */
inline void AccumulateFunctionAnalysis(
    const Analysis& analysis,
    const metric_accumulator::MetricsAccumulator& accumulator)
{
    std::ranges::for_each(analysis, [&](const auto& elem) {
        accumulator.AccumulateNextFunctionResults(elem.second);
    });
}


}  // namespace analyzer
