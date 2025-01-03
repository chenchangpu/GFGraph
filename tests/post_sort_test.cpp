#include "gf_graph.hpp"

using namespace gf_graph;

int main() {
    // InteractGraph g("../datasets/test_account.json", 
    //                 "../datasets/test_post.json", 
    //                 "../datasets/test_interact.json");

    InteractGraph g("../datasets/accountInfo.json", "../datasets/postInfo.json", "../datasets/interactInfo.json");

    constexpr int NUM = 10;
    auto result = g.PostsByFreq();
    printx("per day: \n  ", VecToString(result.first, ToString<std::string>, NUM), "\n  ", 
            VecToString(result.second, ToString<int>, NUM));

    result = g.PostsByFreq(TimeRange::FromDdHhMmSs(1),
                           DateTime(2024, 12, 1), DateTime(2024, 12, 10)); 
    printx("per day in (12.01~12.10): \n  ", VecToString(result.first, ToString<std::string>, NUM), "\n  ", 
            VecToString(result.second, ToString<int>, NUM));

    result = g.PostsByFreq(TimeRange::FromHhMmSs(1),
                           "2024-12-05 00:00:00", "2024-12-06 00:00:00"); 
    printx("per hour in (12.05~12.06): \n  ", VecToString(result.first, ToString<std::string>, NUM), "\n  ", 
            VecToString(result.second, ToString<int>, NUM));

    // result = g.PostsByFreq(TimeRange::FromHhMmSs(1));
    // printx("per 1hour: ", VecToString(result.first), "-", VecToString(result.second));

    // result = g.PostsByFreq(TimeRange::FromMmSs(1));
    // printx("per 1min: ", VecToString(result.first), "-", VecToString(result.second));

    return 0;
}