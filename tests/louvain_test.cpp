#include "gf_graph.hpp"

using namespace gf_graph;

int main() {
    // InteractGraph g("../datasets/test_account.json", 
    //             "../datasets/test_post.json", 
    //             "../datasets/test_interact.json");    

    InteractGraph g("../datasets/accountInfo.json", "../datasets/postInfo.json", "../datasets/interactInfo.json");

    printx("The graph has loaded");

    auto result = LouvainCommunity(g);
    printx("louvain_communitys: ", VecToString(result, [](const auto& vec) {
        return VecToString(vec);
    }));

    return 0;
}