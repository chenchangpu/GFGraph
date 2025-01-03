#include <iostream>
#include "gf_graph.hpp"

#include "str.hpp"

using namespace std;
using namespace gf_graph;

// #define  USE_DEMO
int main(int argc, char *argv[]) {
#ifdef USE_DEMO
    {    
        InteractGraph g;

        {
            printx("AddAccounts:");
            g.AddAccounts("../datasets/test_account.json");
            printx("account2idx:", HashmapToString(g.account2idx_map_));
            printx("idx2accout:", HashmapToString(g.idx2account_map_, [](const auto& p) {
                return std::to_string(p.first) + "->" + p.second->userId;
            }));

        }

        {
            printx("AddPosts:");
            g.AddPosts("../datasets/test_post.json");
            printx("account2idx:", HashmapToString(g.account2idx_map_));
            printx("post2idx:", HashmapToString(g.post2idx_map_));
            printx("idx2post:", HashmapToString(g.idx2post_map_, [](const auto& p) {
                return std::to_string(p.first) + "->" + p.second->post_id;
            }));

            printx("accout_posts:", VecToString(g.account_posts_, [](const auto& vec) {
                return VecToString(vec);
            }));

        }


        {
            printx("AddInteractions:");
            g.AddInteractions("../datasets/test_interact.json");
            printx("accout_posts:", VecToString(g.account_posts_, [](const auto& vec) {
                return VecToString(vec);
            }));
            printx("post_interacts:", VecToString(g.post_interactions_, [](const auto& vec) {
                return VecToString(vec);
            }));

            printx("account_graph:", VecToString(g.account_graph_, [](const auto& hmap) {
                return HashmapToString(hmap, [](const auto& p) { 
                    return to_string(p.first) + "->" + VecToString(p.second); 
                });
            }));
        }
    }
#endif

#ifndef USE_DEMO
    {
        InteractGraph g("../datasets/accountInfo.json", "../datasets/postInfo.json", "../datasets/interactInfo.json");

        constexpr size_t NUM = 10;
        printx("num_accounts: ", g.num_accounts());
        printx("num_posts: ", g.num_posts());
        printx("num_interacts: ", g.num_interactions());

        printx("account2idx: ", HashmapToString(g.account2idx_map_, MapPairToString<std::string, vid_t>, NUM));
        printx("idx2accout: ", HashmapToString(g.idx2account_map_, [](const auto& p) {
            return std::to_string(p.first) + "->" + p.second->userId;
        }, NUM));

        printx("post2idx: ", HashmapToString(g.post2idx_map_, MapPairToString<std::string, vid_t>, NUM));
        printx("idx2post: ", HashmapToString(g.idx2post_map_, [](const auto& p) {
            return std::to_string(p.first) + "->" + p.second->post_id;
        }, NUM));

        printx("accout_posts: ", VecToString(g.account_posts_, [](const auto& vec) {
            return VecToString(vec);
        }, NUM));

        printx("post_interacts: ", VecToString(g.post_interactions_, [](const auto& vec) {
            return VecToString(vec);
        }, NUM));
    }

    // {
    //     InteractGraph gg("../datasets/accountInfo.json", "../datasets/postInfo.json", "../datasets/interactInfo.json");
    // }
#endif

    return 0;
}