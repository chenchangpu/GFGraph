#include "gf_graph.hpp"

using namespace gf_graph;

// #define USE_DEMO

int main() {
#ifdef USE_DEMO
    InteractGraph g("../datasets/test_account.json", 
                    "../datasets/test_post.json", 
                    "../datasets/test_interact.json");
    std::string sep = "\n";
    int topk = 3;
#else
    InteractGraph g("../datasets/accountInfo.json", "../datasets/postInfo.json", "../datasets/interactInfo.json");
    std::string sep = "";
    int topk = 5;
#endif

    printx("TOPK = ", topk, ":");

    auto result = g.GetTopkAccounts(topk, topk_label_t::POSTS);
    printx("posts_topk: ", VecToString(result, ToString<std::string, int>), sep);

    result = g.GetTopkAccounts(topk, topk_label_t::INTERACTED_TIMES);
    printx("interacted_times_topk: ", VecToString(result, ToString<std::string, int>), sep);

    result = g.GetTopkAccounts(topk, topk_label_t::INTERACTED_ACCOUNTS);
    printx("interacted_accounts_topk: ", VecToString(result, ToString<std::string, int>), sep);

    result = g.GetTopkAccounts(topk, topk_label_t::INTERACTING_TIMES);
    printx("interacting_times_topk: ", VecToString(result, ToString<std::string, int>), sep);

    result = g.GetTopkAccounts(topk, topk_label_t::INTERACTING_ACCOUNTS);
    printx("interacting_accounts_topk: ", VecToString(result, ToString<std::string, int>), sep);


    result = g.GetTopkAccounts(topk, topk_label_t::INTERACTING_POSTS);
    printx("interacting_posts_topk: ", VecToString(result, ToString<std::string, int>), sep);

    return 0;
}