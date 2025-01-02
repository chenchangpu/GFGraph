#include "gf_graph.hpp"

using namespace gf_graph;

int main() {
    // InteractGraph g("../datasets/test_account.json", 
    //                 "../datasets/test_post.json", 
    //                 "../datasets/test_interact.json");

    InteractGraph g("../datasets/accountInfo.json", "../datasets/postInfo.json", "../datasets/interactInfo.json");

    auto result = g.GetOldestPost();
    printx("oldest: ", ArrayToString(result));

    auto result1 = g.GetOldestPost(DateTime(2024, 12, 7));
    printx("[2024-12-7, ] oldest: ", ArrayToString(result1));

    auto result2 = g.GetOldestPost(DateTime(2024, 12, 7, 0, 10), DateTime(2024, 12, 8));
    printx("[2024-12-7 00:10, 2024-12-8] oldest: ", ArrayToString(result2));


    return 0;
}