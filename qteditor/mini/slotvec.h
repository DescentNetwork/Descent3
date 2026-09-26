/*
 * Descent 3
 * Copyright (C) 2024-2026 Descent Developers
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <cstddef>
#include <optional>
#include <utility>
#include <vector>

namespace d3
{
  // Table of slots addressed by index.  Each slot is a std::pair whose first
  // member is the slot's reference count and whose second member is the
  // contained value; element i of the base vector is slot i.  The table
  // replaces the slot-based tables that relied on a separate Num_* counter
  // plus a "used" field stored inside the contained type.
  //
  // Value access behaves like std::vector<T>: element i yields T via
  // operator[]/at()/front()/back() while size() reports the number of slots.
  // The reference count is managed separately through acquire()/release()
  // (or observed with refs()); the number of slots whose reference count is
  // zero is tracked in num_empty().
  //
  // slotvec_base_t provides the storage plus reference accounting.  Traversal
  // order (next()/prev()) is left as a pure interface so a derived table can
  // impose its own cycliing rule (for example cycling only over slots whose
  // contained value matches a sub-key instead of over every referenced slot).
  template <typename T>
  class slotvec_base_t : public std::vector<std::pair<int, T>> {
  public:
    using base_type = std::vector<std::pair<int, T>>;
    using value_type = T;
    using size_type = typename base_type::size_type;

    slotvec_base_t() = default;

    // -- std::vector<T>-style value access (hides the pair storage) ----------

    T &operator[](size_type i) { return base_type::operator[](i).second; }
    const T &operator[](size_type i) const { return base_type::operator[](i).second; }

    T &at(size_type i) { return base_type::at(i).second; }
    const T &at(size_type i) const { return base_type::at(i).second; }

    T &front() { return base_type::front().second; }
    const T &front() const { return base_type::front().second; }

    T &back() { return base_type::back().second; }
    const T &back() const { return base_type::back().second; }

    // -- reference counting ---------------------------------------------------

    // Reference count of slot i.
    int refs(size_type i) const { return base_type::at(i).first; }

    // return if slot is used or not
    bool is_used(size_type i) const { return refs(i); }
    bool is_unused(size_type i) const { return !refs(i); }

    // Adds one reference to slot i.
    void acquire(size_type i);

    // Removes one reference from slot i; the slot must be referenced.
    void release(size_type i);

    // -- empty-slot accounting ------------------------------------------------

    // Number of slots whose reference count is zero.
    size_type num_empty(void) const { return m_num_empty; }

    // returns if there are no empty spaces
    bool is_full(void) const  { return !m_num_empty; }

    // returns the position of the next availible slot (or makes one and returns it)
    size_type next_slot(void);

    // Index of the first used slot cyclically after i.  Defined by the derived
    // class: the scan rule is allowed to be sub-key scoped rather than a plain
    // ring over every referenced slot.  std::nullopt means the traversal found
    // no usable slot.
    virtual std::optional<size_type> next(size_type i) const = 0;
    // Index of the first used slot cyclically before i.  Defined by the
    // derived class; std::nullopt means the traversal found no usable slot.
    virtual std::optional<size_type> prev(size_type i) const = 0;

    // Appends a new slot holding `value` with zero references and returns its
    // index.
    size_type add_slot(T value = T());

    // Resizes the table, keeping the empty-slot accounting accurate.  Newly
    // grown slots start unreferenced; shrunk slots are dropped entirely.
    void resize(size_type n);

    void clear() {
      base_type::clear();
      m_num_empty = 0;
    }

    virtual ~slotvec_base_t() = default;

  private:
    size_type m_num_empty = 0; // slots whose reference count is zero
  };

  // Standard slot table: next()/prev() scan for the next referenced slot in a
  // ring, regardless of the contained value.
  template <typename T>
  class slotvec_t : public slotvec_base_t<T> {
  public:
    using base_type = typename slotvec_base_t<T>::base_type;
    using size_type = typename slotvec_base_t<T>::size_type;

    slotvec_t() = default;

    // Index of the first used slot cyclically after i: scans strictly above i,
    // then wraps past the high end and scans up to and including i.  The
    // cursor is the final slot examined, so the ring always terminates on a
    // used slot when any exists; std::nullopt if the table is entirely unused.
    std::optional<size_type> next(size_type i) const override;
    // Index of the first used slot cyclically before i: scans strictly below
    // i, then wraps past the low end and scans down through i.  The cursor is
    // the final slot examined; std::nullopt if the table is entirely unused.
    std::optional<size_type> prev(size_type i) const override;
  };
}