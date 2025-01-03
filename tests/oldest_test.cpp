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
    printx("oldest in 2024-12-7~: ", ArrayToString(result1));

    auto result2 = g.GetOldestPost(DateTime(2024, 12, 5), DateTime(2024, 12, 6));
    printx("oldest in 2024-12-5 ~ 2024-12-6 : ", ArrayToString(result2));

    auto result3 = g.GetOldestPost("2024-12-05 20:03:00", "2024-12-05 20:04:00");
    printx("oldest in 2024-12-5 20:03 ~ 2024-12-5 20:04: ", ArrayToString(result3));

    auto result4 = g.GetOldestPost("2024-12-07 00:10:00", "2024-12-07 01:00:00");
    printx("oldest in 2024-12-7 00:10 ~ 2024-12-7 01:00: ", ArrayToString(result4));

    return 0;
}