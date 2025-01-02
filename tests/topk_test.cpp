#include "gf_graph.hpp"

int main() {
    InteractGraph g("../datasets/test_account.json", 
                    "../datasets/test_post.json", 
                    "../datasets/test_interact.json");

    // InteractGraph g("../datasets/accountInfo.json", "../datasets/postInfo.json", "../datasets/interactInfo.json");

    printx("graph loaded");

    int topk = 2;

    auto result = g.GetTopkAccounts(topk, topk_label_t::POSTS);
    printx("posts_topk: ", VecToString(result, ToString<std::string, int>), "\n");

    result = g.GetTopkAccounts(topk, topk_label_t::INTERACTED_TIMES);
    printx("interacted_times_topk: ", VecToString(result, ToString<std::string, int>), "\n");

    result = g.GetTopkAccounts(topk, topk_label_t::INTERACTED_ACCOUNTS);
    printx("interacted_accounts_topk: ", VecToString(result, ToString<std::string, int>), "\n");

    result = g.GetTopkAccounts(topk, topk_label_t::INTERACTING_TIMES);
    printx("interacting_times_topk: ", VecToString(result, ToString<std::string, int>), "\n");

    result = g.GetTopkAccounts(topk, topk_label_t::INTERACTING_ACCOUNTS);
    printx("interacting_accounts_topk: ", VecToString(result, ToString<std::string, int>), "\n");


    result = g.GetTopkAccounts(topk, topk_label_t::INTERACTING_POSTS);
    printx("interacting_posts_topk: ", VecToString(result, ToString<std::string, int>), "\n");

    return 0;
}