#include "slotvec.h"

#include <QtGlobal>

#include "game/lightmap_info.h"
#include "game/special_face.h"
#include "lib/lightmap.h"

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
  typename slotvec_t<T>::size_type slotvec_t<T>::add_slot(const T &value)
  {
    base_type::emplace_back(0, value);
    ++m_num_empty;
    return base_type::size() - 1;
  }

  template class slotvec_t<bms_lightmap>;
  template class slotvec_t<lightmap_info>;
  template class slotvec_t<special_face>;
}
