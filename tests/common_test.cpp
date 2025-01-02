#include "gf_graph.hpp"

int main() {
    InteractGraph g("../datasets/test_account.json", 
                    "../datasets/test_post.json", 
                    "../datasets/test_interact.json");

    // InteractGraph g("../datasets/accountInfo.json", "../datasets/postInfo.json", "../datasets/interactInfo.json");

    printx("graph loaded");

    auto result = g.GetCommonInteractingAccounts({"000", "001"});
    printx("000 ^ 001: ", VecToString(result), "\n");

    result = g.GetCommonInteractingAccounts({"000", "001", "004", "002"});
    printx("000 ^ 001 ^ 004 ^ 002: ", VecToString(result), "\n");

    result = g.GetCommonInteractingAccounts({"000", "001", "004", "003"});
    printx("000 ^ 001 ^ 004 ^ 003: ", VecToString(result), "\n");

    result = g.GetCommonInteractingAccounts({"001"});
    printx("001: ", VecToString(result), "\n");

    return 0;
}