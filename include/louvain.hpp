#pragma once

#include <vector>

#include "interact_graph.hpp"
#include "louvain/main.hxx"

namespace gf_graph {

std::vector<std::vector<std::string>> 
LouvainCommunity(const InteractGraph& g) {
    louvain::DiGraph<vid_t, louvain::None, float> x;
    vid_t num_vertices = g.num_accounts();
    LOG_DEBUG("num_vertices: ", num_vertices);
    louvain::addVerticesU(x, 0, num_vertices);

    const auto& account_graph = g.account_graph();
    thrust::for_each(thrust::omp::par, thrust::make_counting_iterator<vid_t>(0),
        thrust::make_counting_iterator<vid_t>(num_vertices), [&account_graph, &x](const vid_t& src) {
            for (const auto& [dst, edata]: account_graph[src]) {
                x.addEdge(src, dst, edata.size());
            }
        });
    louvain::updateOmpU(x);

    x = louvain::symmetricizeOmp(x);    
    LOG_DEBUG("DiGraph: |V|=", x.order(), " |E|=", x.size());

    auto louvain_result = louvain::louvainStaticOmp(x);
    // for (size_t i = 0; i < louvain_result.membership.size(); ++i) {
    //     LOG_DEBUG("[", i, "] membership: ", louvain_result.membership[i], ", vertexWeight: ", louvain_result.vertexWeight[i]);
    // }

    // put accounts in the same community into the same bucket
    hashmap<vid_t, std::vector<vid_t>> community_map;
    for (vid_t uidx = 0; uidx < num_vertices; ++uidx) {
        vid_t cid = louvain_result.membership[uidx];
        community_map[cid].push_back(uidx);
    }

    // change the account from user-index to userId and move it to 2D-vector
    std::vector<std::vector<std::string>> result;
    for (auto& [_, community]: community_map) {
        std::vector<std::string> vec;
        for (vid_t uidx: community) {
            vec.push_back(g.idx2account(uidx));
        }
        result.push_back(vec);
    }

    return result;
}
    
} // namespace gf_graph 