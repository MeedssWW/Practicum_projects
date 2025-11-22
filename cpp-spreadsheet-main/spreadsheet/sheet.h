#pragma once

#include "cell.h"
#include "common.h"

#include <functional>
#include <memory>
#include <unordered_map>
#include <unordered_set>

class Sheet : public SheetInterface {
public:
    ~Sheet();

    void SetCell(Position pos, std::string text) override;

    const CellInterface* GetCell(Position pos) const override;
    CellInterface* GetCell(Position pos) override;

    void ClearCell(Position pos) override;

    Size GetPrintableSize() const override;

    void PrintValues(std::ostream& output) const override;
    void PrintTexts(std::ostream& output) const override;

private:
    using ColToCell = std::unordered_map<int, std::unique_ptr<Cell>>;
    std::unordered_map<int, ColToCell> rows_;

    int max_used_row_ = -1;
    int max_used_col_ = -1;

    std::unordered_map<Position, std::unordered_set<Position>> dependencies_;

    static void EnsureValid(Position pos);

    void RecomputePrintableBounds();

    bool EraseIfEmpty(Position pos);

    void SetDependencies(Position from, const std::vector<Position>& new_deps);
    void RemoveAllDependencies(Position from);

    bool CreatesCycle(Position from, const std::vector<Position>& new_deps) const;
    bool HasPath(Position from, Position to) const;
};