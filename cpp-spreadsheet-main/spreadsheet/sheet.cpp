#include "sheet.h"

#include "cell.h"
#include "common.h"

#include <algorithm>
#include <functional>
#include <iostream>
#include <optional>
#include <sstream>

using namespace std::literals;

Sheet::~Sheet() {}

void Sheet::EnsureValid(Position pos) {
	if (!pos.IsValid()) {
		throw InvalidPositionException("Invalid position");
	}
}

void Sheet::RecomputePrintableBounds() {
	max_used_row_ = -1;
	max_used_col_ = -1;
	for (const auto& [row, cols] : rows_) {
		for (const auto& [col, cell_ptr] : cols) {
			if (cell_ptr && !cell_ptr->GetText().empty()) {
				if (row > max_used_row_) max_used_row_ = row;
				if (col > max_used_col_) max_used_col_ = col;
			}
		}
	}
}

bool Sheet::EraseIfEmpty(Position pos) {
	auto row_it = rows_.find(pos.row);
	if (row_it == rows_.end()) {
		return false;
	}
	auto& cols = row_it->second;
	auto col_it = cols.find(pos.col);
	if (col_it == cols.end()) {
		return false;
	}
	Cell* cell = col_it->second.get();
	if (cell && cell->GetText().empty()) {
		cols.erase(col_it);
		if (cols.empty()) {
			rows_.erase(row_it);
		}
		return true;
	}
	return false;
}

bool Sheet::HasPath(Position from, Position to) const {
	if (from == to) return true;
	std::unordered_set<Position> visited;
	std::vector<Position> stack;
	stack.push_back(from);
	while (!stack.empty()) {
		Position cur = stack.back();
		stack.pop_back();
		if (!visited.insert(cur).second) continue;
		auto it = dependencies_.find(cur);
		if (it == dependencies_.end()) continue;
		for (const Position& nxt : it->second) {
			if (nxt == to) return true;
			stack.push_back(nxt);
		}
	}
	return false;
}

bool Sheet::CreatesCycle(Position from, const std::vector<Position>& new_deps) const {
	for (const Position& dep : new_deps) {
		if (HasPath(dep, from)) {
			return true;
		}
	}
	return false;
}

void Sheet::RemoveAllDependencies(Position from) {
	auto it = dependencies_.find(from);
	if (it != dependencies_.end()) {
		it->second.clear();
	}
}

void Sheet::SetDependencies(Position from, const std::vector<Position>& new_deps) {
	auto& set = dependencies_[from];
	set.clear();
	for (const Position& p : new_deps) {
		set.insert(p);
	}
}

void Sheet::SetCell(Position pos, std::string text) {
	EnsureValid(pos);

	auto& cols = rows_[pos.row];
	auto& cell_uptr = cols[pos.col];
	if (!cell_uptr) {
		cell_uptr = std::make_unique<Cell>(static_cast<SheetInterface&>(*this));
	}
	Cell& cell = *cell_uptr;

	bool was_nonempty = !cell.GetText().empty();

	bool is_formula = !text.empty() && text.front() == '=' && text.size() > 1;
	std::vector<Position> new_refs;
	if (is_formula) {
		std::unique_ptr<FormulaInterface> tmp;
		try {
			tmp = ParseFormula(text.substr(1));
		} catch (...) {
			throw;
		}
		new_refs = tmp->GetReferencedCells();
		if (CreatesCycle(pos, new_refs)) {
			throw CircularDependencyException("circular");
		}
	}

	cell.Set(std::move(text));

	if (is_formula) {
		SetDependencies(pos, new_refs);
	} else {
		RemoveAllDependencies(pos);
	}

	if (cell.GetText().empty()) {
		cols.erase(pos.col);
		if (cols.empty()) {
			rows_.erase(pos.row);
		}
	}

	bool now_nonempty = false;
	{
		auto cit = rows_.find(pos.row);
		if (cit != rows_.end()) {
			auto ccit = cit->second.find(pos.col);
			if (ccit != cit->second.end() && ccit->second && !ccit->second->GetText().empty()) {
				now_nonempty = true;
			}
		}
	}

	if (now_nonempty) {
		if (pos.row > max_used_row_) max_used_row_ = pos.row;
		if (pos.col > max_used_col_) max_used_col_ = pos.col;
	} else if (was_nonempty) {
		RecomputePrintableBounds();
	}
}

const CellInterface* Sheet::GetCell(Position pos) const {
	EnsureValid(pos);
	auto row_it = rows_.find(pos.row);
	if (row_it == rows_.end()) {
		return nullptr;
	}
	auto col_it = row_it->second.find(pos.col);
	if (col_it == row_it->second.end()) {
		return nullptr;
	}
	return col_it->second.get();
}

CellInterface* Sheet::GetCell(Position pos) {
	EnsureValid(pos);
	auto row_it = rows_.find(pos.row);
	if (row_it == rows_.end()) {
		return nullptr;
	}
	auto col_it = row_it->second.find(pos.col);
	if (col_it == row_it->second.end()) {
		return nullptr;
	}
	return col_it->second.get();
}

void Sheet::ClearCell(Position pos) {
	EnsureValid(pos);
	auto row_it = rows_.find(pos.row);
	if (row_it == rows_.end()) {
		return;
	}
	auto& cols = row_it->second;
	auto col_it = cols.find(pos.col);
	if (col_it == cols.end()) {
		return;
	}
	cols.erase(col_it);
	if (cols.empty()) {
		rows_.erase(row_it);
	}
	RemoveAllDependencies(pos);
	RecomputePrintableBounds();
}

Size Sheet::GetPrintableSize() const {
	if (max_used_row_ < 0 || max_used_col_ < 0) {
		return {0, 0};
	}
	return {max_used_row_ + 1, max_used_col_ + 1};
}

void Sheet::PrintValues(std::ostream& output) const {
	Size sz = GetPrintableSize();
	for (int r = 0; r < sz.rows; ++r) {
		for (int c = 0; c < sz.cols; ++c) {
			const CellInterface* cell = nullptr;
			auto row_it = rows_.find(r);
			if (row_it != rows_.end()) {
				auto col_it = row_it->second.find(c);
				if (col_it != row_it->second.end()) {
					cell = col_it->second.get();
				}
			}

			if (cell) {
				const auto value = cell->GetValue();
				std::visit([
					&output
				](const auto& x) {
					output << x;
				}, value);
			}
			if (c + 1 < sz.cols) {
				output << '\t';
			}
		}
		output << '\n';
	}
}

void Sheet::PrintTexts(std::ostream& output) const {
	Size sz = GetPrintableSize();
	for (int r = 0; r < sz.rows; ++r) {
		for (int c = 0; c < sz.cols; ++c) {
			const CellInterface* cell = nullptr;
			auto row_it = rows_.find(r);
			if (row_it != rows_.end()) {
				auto col_it = row_it->second.find(c);
				if (col_it != row_it->second.end()) {
					cell = col_it->second.get();
				}
			}
			if (cell) {
				output << cell->GetText();
			}
			if (c + 1 < sz.cols) {
				output << '\t';
			}
		}
		output << '\n';
	}
}

std::unique_ptr<SheetInterface> CreateSheet() {
	return std::make_unique<Sheet>();
}