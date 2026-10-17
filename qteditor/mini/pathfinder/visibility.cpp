#include "visibility.h"

namespace visibility
{
    // find a node that is both visible and "forward" relative to `forward_dir`.
    // Two-pass:
    //   - pass 0: require node's max clearance >= min_clearance and perform visibility test, prefer higher dot and closer distance
    //   - pass 1: relax clearance requirement and accept nodes previously marked visible
    //
    // returns optional<graph::nodeid_t> (std::nullopt if none found).
    // If allow_random_fallback == true, returns a random node when no visible node is found.

    std::optional<graph::nodeid_t> directional_visible_node(
        const graph::graph_t& g,
        const vector3& from_pos,
        const vector3& forward_dir,
        float entity_radius,
        const testfunc_t& testfunc)
    {
        if (g.size() == 0)
        	return std::nullopt;
        	
        // minimum node "clearance" threshold
        float min_clearance = entity_radius * 0.25f;
        if (min_clearance > 20.0f)
        	min_clearance = 20.0f; // match MAX_BNODE_SIZE semantics

        const vector3 fdir = normalized(forward_dir);

        // precompute per-node max clearance
        std::vector<float> node_clearance(g.size());
        for (std::size_t i = 0; i < g.size(); ++i
         {
            float maxc = 0.0f;
            for (auto const& e : g.node(i).edges) maxc = std::max(maxc, e.clearance);
            node_clearance[i] = maxc;
        }

        // states remembered only inside this call (stateless external API)
        std::vector<char> cached_vis(g.size(), -1); // -1 unknown, 0 blocked, 1 visible

        for (int attempt = 0; attempt < 2; ++attempt)
        {
            float best_dot = -2.0f;
            float best_dist = std::numeric_limits<float>::infinity();
            std::optional<graph::nodeid_t> best_node;

            for (std::size_t i = 0; i < g.size(); ++i)
            {
                // prefer nearer nodes
                vector3 to = g.node(i).position - from_pos;
                float dist = len(to);
                if (!(dist < best_dist && dist < 800.0f))
                	continue;

                float dp = dot(fdir, normalized(to));
                if (dp <= 0.0f && attempt == 0)
                	continue; // want nodes in front on first pass

                // skip small-clearance nodes on first pass
                if (attempt == 0 && node_clearance[i] < min_clearance)
                	continue;

                bool visible;
                if (cached_vis[i] == 1)
                	visible = true;
                else if (cached_vis[i] == 0)
                	visible = false;
                else
                {
                    // call out to injected visibility tester
                    visible = testfunc(from_pos, g.node(i).position, min_clearance);
                    cached_vis[i] = visible ? 1 : 0;
                }

                if (!visible && attempt == 0)
                	continue;

                // choose best by dot (prefer more forward), tie-break by distance
                if (dp > best_dot || (dp == best_dot && dist < best_dist))
                {
                    best_dot = dp;
                    best_dist = dist;
                    best_node = static_cast<graph::nodeid_t>(i);
                }
            }

            if (best_node.has_value()) return best_node;
        }

        std::uniform_int_distribution<std::size_t> dist(0, g.size()-1);
        return static_cast<graph::nodeid_t>(dist(d3::rand()));
    }

    // find the physically closest node that is visible to `from_pos`
    // Two-pass:
    //   - pass 0: require node max clearance >= min_clearance, must pass vis test
    //   - pass 1: relax clearance requirement

    std::optional<graph::graph::nodeid_t> closest_visible_node(
        const graph::graph_t& g,
        const vector3& from_pos,
        float entity_radius,
        const testfunc_t& testfunc)
    {
        if (g.size() == 0) return std::nullopt;
        float min_clearance = entity_radius * 0.25f;
        if (min_clearance > 20.0f) min_clearance = 20.0f;

        std::vector<float> node_clearance(g.size());
        for (std::size_t i = 0; i < g.size(); ++i) {
            float maxc = 0.0f;
            for (auto const& e : g.node(i).edges) maxc = std::max(maxc, e.clearance);
            node_clearance[i] = maxc;
        }

        for (int attempt = 0; attempt < 2; ++attempt) {
            std::optional<graph::nodeid_t> best_node;
            float best_dist = std::numeric_limits<float>::infinity();

            for (std::size_t i = 0; i < g.size(); ++i)
            {
                float d = std::fabs(g.node(i).position.x() - from_pos.x())
                        + std::fabs(g.node(i).position.y() - from_pos.y())
                        + std::fabs(g.node(i).position.z() - from_pos.z());
                if (!(d < best_dist && d < 800.0f)) continue;

                if (attempt == 0 && node_clearance[i] < min_clearance) continue;

                bool visible = testfunc(from_pos, g.node(i).position, min_clearance);
                if (!visible && attempt == 0)
                	continue;
                if (visible)
                {
                    best_dist = d;
                    best_node = static_cast<graph::nodeid_t>(i);
                }
            }

            if (best_node.has_value())
            	return best_node;
        }

        std::uniform_int_distribution<std::size_t> dist(0, g.size()-1);
        return static_cast<graph::graph::nodeid_t>(dist(d3::rand());
    }
}
