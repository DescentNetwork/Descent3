
// Adapters: connect existing bn_list / vector3 / fvi_FindIntersection to the
// modern_bnode API.
//
// Requires: modern_bnode.hpp in include path.
// Provides:
//   std::optional<std::vector<std::size_t>> modern_find_path_from_bnlist(int
//   start_room, const bn_list& bnlist, std::size_t start_idx, std::size_t
//   goal_idx, float rad); std::optional<std::size_t>
//   modern_find_dir_local_visible_node(int roomnum, const vector3& pos, const
//   vector3& fvec, float rad); std::optional<std::size_t>
//   modern_find_closest_local_visible_node(int roomnum, const vector3& pos,
//   float rad);
//
// Notes:
// - These adapter functions return std::optional values and do not touch
// global BNode_* variables.
// - They construct a modern_bnode::Graph from bn_list, filtering edges to the
// current room (preserving original intra-room semantics).
// - The VisibilityTester delegates to engine fvi_FindIntersection using a
// small conversion helper.
// - If your engine's vector3 constructor or API differs, update
// to_engine/from_engine conversions below.

#include "graph.h"
#include "visibility.h"

#include "bnode.h"            // bn_list, bn_node, bn_edge
#include "findintersection.h" // fvi_FindIntersection, fvi_query, fvi_info, fvi_hit_type
#include "terrain.h"          // GetTerrainRoomFromPos (optional)
#include "BOA.h"              // BOA_INDEX (optional)
#include <optional>
#include <vector>
#include <cstddef>

// Build a modern Graph from a bn_list. We only include edges whose end_room
// equals start_room (intra-room edges), which mirrors the original
// BNode_FindPath filtering.
static graph::graph_t build_graph_from_bnlist(int start_room,
                                              const bn_list& bnlist)
{
  graph::graph_t g;
  // Add nodes with their positions
  for(const auto& n : bnlist.nodes)
  {
    // bn_node::pos is engine vector3; convert to modern Vec3
    Vec3 p = graph::nodeid_t(n.pos);
    g.add_node(p);
  }

  // Add edges (directed) - include only edges that lead to the same room as
  // start_room
  for(std::size_t u = 0; u < bnlist.nodes.size(); ++u)
  {
    const auto& node = bnlist.nodes[u];
    for(const auto& e : node.edges)
    {
      if(e.end_room != start_room)
        continue; // preserve original intra-room-only behavior
      std::size_t v = static_cast<std::size_t>(e.end_index);
      float cost = static_cast<float>(e.cost);
      float clearance = e.max_rad;
      // Defensive: ensure indices are valid within the bn_list
      if(v < bnlist.nodes.size())
      {
        g.add_edge(u, v, cost, clearance);
      }
    }
  }

  return g;
}

// Build a VisibilityTester that delegates to the engine fvi_FindIntersection.
// The lambda captures roomnum because startroom selection may depend on
// terrain/outside rooms. This uses the same logic as earlier code: when
// roomnum denotes a terrain/outside room, we call GetTerrainRoomFromPos(pos)
static visibility::testfunc_t make_engine_visibility_tester(uint32_t roomnum)
{
  return [roomnum](const vector3& p0, const vector3& p1, float rad) -> bool {
    fvi_query fq;
    fvi_info hit;

    fq.p0 = &p0;
    // pick an appropriate startroom for terrain/outside handling
    if(roomnum >= Rooms.size() && roomnum < Rooms.size() + 8)
    {
      fq.startroom = GetTerrainRoomFromPos(p0);
    }
    else
    {
      fq.startroom = roomnum;
    }
    fq.p1 = &p1;
    fq.rad = rad;
    fq.thisobjnum.reset();
    fq.ignore_obj_list
        .clear()
        // Set flags that closely match earlier code (the exact enum/flag names
        // may differ)
        fq.flags
        = fvi_query_flags_t{};
    fq.flags.check_objs = true;
    fq.flags.no_relink = true;
    fq.flags.ignore_non_lightmap_objects = true;

    auto hit_type = fvi_FindIntersection(fq, hit);
    return (hit_type == fvi_hit_type::none);
  };
}

std::optional<std::vector<uint32_t>>
find_path(index_t start_room, const bn_list& bnlist, index_t start_idx,
          index_t goal_idx, float rad = 0.0f)
{
  // The original code often applied BOA_INDEX on start_room; follow same
  // pattern if needed: start_room = BOA_INDEX(start_room); But leave that to
  // the caller if desired.
  if(bnlist.nodes.empty() || start_idx >= bnlist.nodes.size()
     || goal_idx >= bnlist.nodes.size())
    return std::nullopt;

  graph::graph_t g = build_graph_from_bnlist(start_room, bnlist);
  return shortest_path(g, start_idx, goal_idx);
}

index_t find_direction_visible_node(index_t roomnum, const vector3& pos,
                                    const vector3& fvec, float rad)
{
  if(!roomnum)
    return std::nullopt;

  // Build Graph so the visibility functions can use node positions
  bn_list* bnlist_ptr = BNode_GetBNListPtr(roomnum);
  if(!bnlist_ptr)
    return std::nullopt;
  const bn_list& bnlist = *bnlist_ptr;
  if(bnlist.nodes.empty())
    return std::nullopt;

  Graph g = build_graph_from_bnlist(roomnum, bnlist);
  auto vistest = make_engine_visibility_tester(roomnum);
  return visibility::directional_visible_node(g, pos, fvec, rad, vistest);
}

index_t find_closest_visible_node(index_t roomnum, const vector3& pos,
                                  float rad)
{
  if(!roomnum)
    return std::nullopt;

  bn_list* bnlist_ptr = BNode_GetBNListPtr(roomnum);
  if(!bnlist_ptr)
    return std::nullopt;
  const bn_list& bnlist = *bnlist_ptr;
  if(bnlist.nodes.empty())
    return std::nullopt;

  graph::graph_t g = build_graph_from_bnlist(roomnum, bnlist);
  auto vistest = make_engine_visibility_tester(roomnum);
  return visibility::closest_visible_node(g, pos, rad, vistest);
}
