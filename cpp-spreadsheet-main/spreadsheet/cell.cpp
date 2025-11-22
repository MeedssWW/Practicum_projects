#include "cell.h"

#include <cassert>
#include <iostream>
#include <string>
#include <optional>
#include <utility>

using namespace std;

namespace {
inline bool IsFormulaString(const std::string& text) {
    return !text.empty() && text[0] == '=' && text.size() > 1;
}

inline bool IsEscapedText(const std::string& text) {
    return !text.empty() && text[0] == '\'';
}
}

// Базовая реализация ячейки
class Cell::Impl {
public:
    virtual ~Impl() = default;
    virtual CellInterface::Value GetValue(const SheetInterface& sheet) const = 0;
    virtual std::string GetText() const = 0;
    virtual std::vector<Position> GetReferencedCells() const { return {}; }
};

class Cell::EmptyImpl : public Cell::Impl {
public:
    CellInterface::Value GetValue(const SheetInterface& /*sheet*/) const override {
        return std::string();
    }
    std::string GetText() const override {
        return std::string();
    }
};

class Cell::TextImpl : public Cell::Impl {
public:
    explicit TextImpl(std::string text)
        : raw_text_(std::move(text)) {}

    CellInterface::Value GetValue(const SheetInterface& /*sheet*/) const override {
        if (IsEscapedText(raw_text_)) {
            return raw_text_.substr(1);
        }
        return raw_text_;
    }

    std::string GetText() const override {
        return raw_text_;
    }

private:
    std::string raw_text_;
};

class Cell::FormulaImpl : public Cell::Impl {
public:
    explicit FormulaImpl(std::string expression)
        : formula_(ParseFormula(std::move(expression))) {}

    CellInterface::Value GetValue(const SheetInterface& sheet) const override {
        auto value = formula_->Evaluate(sheet);
        if (std::holds_alternative<double>(value)) {
            return std::get<double>(value);
        }
        return std::get<FormulaError>(value);
    }

    std::string GetText() const override {
        return std::string("=") + formula_->GetExpression();
    }

    std::vector<Position> GetReferencedCells() const override {
        return formula_->GetReferencedCells();
    }

private:
    std::unique_ptr<FormulaInterface> formula_;
};

Cell::Cell(SheetInterface& sheet) : impl_(std::make_unique<EmptyImpl>()), sheet_(sheet){}

Cell::~Cell() {}

void Cell::Set(std::string text) {
    if (text.empty()) {
        impl_ = std::make_unique<EmptyImpl>();
        return;
    }

    if (text.front() == '=' && text.size() > 1) {
        std::string expression = text.substr(1);
        std::unique_ptr<Impl> new_impl;
        new_impl = std::make_unique<FormulaImpl>(std::move(expression));
        impl_ = std::move(new_impl);
        return;
    }

    impl_ = std::make_unique<TextImpl>(std::move(text));
}

void Cell::Clear() {
    impl_ = std::make_unique<EmptyImpl>();
}

Cell::Value Cell::GetValue() const {
    return impl_->GetValue(sheet_);
}
std::string Cell::GetText() const {
    return impl_->GetText();
}

std::vector<Position> Cell::GetReferencedCells() const {
    return impl_->GetReferencedCells();
}

bool Cell::IsReferenced() const {
    return false;
}