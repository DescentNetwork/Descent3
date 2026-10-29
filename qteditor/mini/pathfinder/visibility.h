#pragma once
#include <functional>
#include <optional>

#include "vecmat.h"
#include "graph.h"

namespace visibility
{
  // --------------------
  // Visibility tester type
  // --------------------
  // The user's engine must provide a callable that answers whether a sphere of
  // `radius` centered at `from` can reach `to` unobstructed (line-of-sight /
  // swept-sphere test). Signature: bool(vector3 from, vector3 to, float
  // radius) It must return true when visible (no obstruction), false
  // otherwise. This keeps collision logic out of the graph and allows unit
  // testing.
  using testfunc_t = std::function<bool(const vector3& from, const vector3& to,
                                        float radius)>;

  // find a node that is both visible and "forward" relative to `forward_dir`.
  // Two-pass:
  //   - pass 0: require node's max clearance >= min_clearance and perform
  //   visibility test, prefer higher dot and closer distance
  //   - pass 1: relax clearance requirement and accept nodes previously marked
  //   visible
  //
  // returns optional<nodeid_t> (std::nullopt if none found).
  // If allow_random_fallback == true, returns a random node when no visible
  // node is found.
  std::optional<graph::nodeid_t>
  directional_visible_node(const graph::graph_t& g, const vector3& from_pos,
                           const vector3& forward_dir, float entity_radius,
                           const testfunc_t& testfunc);

  // find the physically closest node that is visible to `from_pos`
  // Two-pass:
  //   - pass 0: require node max clearance >= min_clearance, must pass vis
  //   test
  //   - pass 1: relax clearance requirement
  std::optional<graph::nodeid_t>
  closest_visible_node(const graph::graph_t& g, const vector3& from_pos,
                       float entity_radius, const testfunc_t& testfunc);
}
