#include "slotvec.h"

#include <QtGlobal>

#include "game/gamepath.h"
#include "game/gametexture.h"
#include "game/lightmap_info.h"
#include "game/special_face.h"
#include "lib/bitmap.h"
#include "lib/lightmap.h"
#include "lib/ship.h"
#include "lib/ssl_lib.h"
#include "lib/weapon.h"

namespace d3
{
  template <typename T>
  void slotvec_t<T>::acquire(size_type i)
  {
    Q_ASSERT(i < this->size());
    int &r = base_type::at(i).first;
    if (r == 0) {
      Q_ASSERT(m_num_empty > 0);
      --m_num_empty;
    }
    ++r;
  }

  template <typename T>
  void slotvec_t<T>::release(size_type i)
  {
    Q_ASSERT(i < this->size());
    int &r = base_type::at(i).first;
    Q_ASSERT(r > 0);
    --r;
    if (r == 0)
      ++m_num_empty;
  }

  template <typename T>
  typename slotvec_t<T>::size_type slotvec_t<T>::next_slot(void)
  {
    // Fast path: no freed slots, so the frontier is the end of the table and a
    // scan is unnecessary.  This keeps the level-load allocation loop O(n)
    // (each load allocates in ascending order with no interleaved frees).
    if (m_num_empty == 0)
      return add_slot();
    for (size_type i = 0; i < this->size(); ++i)
      if (base_type::at(i).first == 0)
        return i;
    return add_slot();
  }

  template <typename T>
  typename slotvec_t<T>::size_type slotvec_t<T>::add_slot(T value)
  {
    base_type::emplace_back(0, std::move(value));
    ++m_num_empty;
    return base_type::size() - 1;
  }

  template <typename T>
  void slotvec_t<T>::resize(size_type n)
  {
    base_type::resize(n);
    // Rebuild the empty count from scratch: the grown tail is unreferenced and
    // shrinking may have dropped referenced slots, so incremental adjustment
    // would be wrong.  resize is rare (level-load time), so a recount is fine.
    m_num_empty = 0;
    for (size_type i = 0; i < n; ++i)
      if (base_type::at(i).first == 0)
        ++m_num_empty;
  }

  template class slotvec_t<bms_bitmap>;
  template class slotvec_t<bms_lightmap>;
  template class slotvec_t<game_path>;
  template class slotvec_t<lightmap_info>;
  template class slotvec_t<ship>;
  template class slotvec_t<sound_info>;
  template class slotvec_t<special_face>;
  template class slotvec_t<texture>;
  template class slotvec_t<weapon>;
}
