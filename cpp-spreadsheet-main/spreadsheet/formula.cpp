#include "formula.h"

#include "FormulaAST.h"

#include <algorithm>
#include <cassert>
#include <cctype>
#include <sstream>

using namespace std::literals;

std::ostream& operator<<(std::ostream& output, FormulaError fe) {
    switch (fe.GetCategory()) {
        case FormulaError::Category::Ref:
            output << "#REF!";
            break;
        case FormulaError::Category::Value:
            output << "#VALUE!";
            break;
        case FormulaError::Category::Arithmetic:
            output << "#ARITHM!";
            break;
    }
    return output;
}

namespace {
class Formula : public FormulaInterface {
public:
    explicit Formula(std::string expression) try
        : ast_(ParseFormulaAST(expression)) {
    } catch (const FormulaException&) {
        throw;
    } catch (const std::exception& exc) {
        throw FormulaException(exc.what());
    }

    Value Evaluate(const SheetInterface& sheet) const override {
        try {
            double result = ast_.Execute(sheet);
            return result;
        } catch (const FormulaError& fe) {
            return fe;
        } catch (const std::exception&) {
            return FormulaError(FormulaError::Category::Arithmetic);
        }
    }

    std::string GetExpression() const override {
        std::ostringstream out;
        ast_.PrintFormula(out);
        return out.str();
    }

    std::vector<Position> GetReferencedCells() const override {
        std::vector<Position> cells{ast_.GetCells().begin(), ast_.GetCells().end()};
        std::sort(cells.begin(), cells.end());
        cells.erase(std::unique(cells.begin(), cells.end()), cells.end());
        return cells;
    }

private:
    FormulaAST ast_;
};
}  // namespace

std::unique_ptr<FormulaInterface> ParseFormula(std::string expression) {
    return std::make_unique<Formula>(std::move(expression));
}