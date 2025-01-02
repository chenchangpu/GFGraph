#include "gf_graph.hpp"


int main() {
    // InteractGraph g("../datasets/test_account.json", 
    //                 "../datasets/test_post.json", 
    //                 "../datasets/test_interact.json");

    InteractGraph g("../datasets/accountInfo.json", "../datasets/postInfo.json", "../datasets/interactInfo.json");

    auto result = g.PostsSortByFreq();
    printx("per 1day: ", VecToString(result.first), "-", VecToString(result.second));

    result = g.PostsSortByFreq(TimeRange::FromDdHhMmSs(1),
                                    DateTime(2024, 12, 1), DateTime(2024, 12, 10));
    printx("per 1day (12.01~12.10): ", VecToString(result.first), "-", VecToString(result.second));

    // result = g.PostsSortByFreq(TimeRange::FromHhMmSs(1));
    // printx("per 1hour: ", VecToString(result.first), "-", VecToString(result.second));


    // result = g.PostsSortByFreq(TimeRange::FromMmSs(1));
    // printx("per 1min: ", VecToString(result.first), "-", VecToString(result.second));

    return 0;
}