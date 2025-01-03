#include "gf_graph.hpp"

using namespace gf_graph;

// #define USE_DEMO

int main() {
#ifdef USE_DEMO
    InteractGraph g("../datasets/test_account.json", 
                    "../datasets/test_post.json", 
                    "../datasets/test_interact.json");

    printx("graph loaded");

    auto result = g.GetCommonInteractingAccounts({"000", "001"});
    printx("000 ^ 001: ", VecToString(result), "\n");

    result = g.GetCommonInteractingAccounts({"000", "001", "004", "002"});
    printx("000 ^ 001 ^ 004 ^ 002: ", VecToString(result), "\n");

    result = g.GetCommonInteractingAccounts({"000", "001", "004", "003"});
    printx("000 ^ 001 ^ 004 ^ 003: ", VecToString(result), "\n");

    result = g.GetCommonInteractingAccounts({"001"});
    printx("001: ", VecToString(result), "\n");
#else
    InteractGraph g("../datasets/accountInfo.json", "../datasets/postInfo.json", "../datasets/interactInfo.json");

    auto result = g.GetCommonInteractingAccounts({"296140661"});
    printx("296140661: ", VecToString(result, ToString<std::string>, 10));    

    result = g.GetCommonInteractingAccounts({"562773398", "808468854"});
    printx("562773398 ^ 808468854: ", VecToString(result, ToString<std::string>, 10));

    result = g.GetCommonInteractingAccounts({"199692837", "10783522"});
    printx("199692837 ^ 10783522: ", VecToString(result));

    result = g.GetCommonInteractingAccounts({"808468854", "795188519115358208"});
    printx("808468854 ^ 795188519115358208:", VecToString(result));

    result = g.GetCommonInteractingAccounts({"562773398", "808468854", "795188519115358208"});
    printx("562773398 ^ 808468854 ^ 795188519115358208:", VecToString(result));

#endif

    return 0;
}