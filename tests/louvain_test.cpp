#include "gf_graph.hpp"

using namespace gf_graph;

int main() {
    // InteractGraph g("../datasets/test_account.json", 
    //             "../datasets/test_post.json", 
    //             "../datasets/test_interact.json");    

    InteractGraph g("../datasets/accountInfo.json", "../datasets/postInfo.json", "../datasets/interactInfo.json");

    // printx("The graph has loaded");

    auto result = LouvainCommunity(g);

    printx("num_communitys: ", result.size());

    constexpr int NUM = 5;
    for (int i = 0; i < std::min<int>(NUM, result.size()); ++i) {
        printx("community-", i, " size=", result[i].size(), 
            ", members=", VecToString(result[i], ToString<std::string>, 10));
    }

    return 0;
}