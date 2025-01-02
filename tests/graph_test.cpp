#include <iostream>
#include "interact_graph.hpp"

#include "str.hpp"

using namespace std;

int main(int argc, char *argv[]) {
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

    // {
    //     InteractGraph g("../datasets/test_account.json", "../datasets/test_post.json", "../datasets/test_interact.json");

    //     printx("account2idx:", HashmapToString(g.account2idx_map_));
    //     printx("idx2accout:", HashmapToString(g.idx2account_map_, [](const auto& p) {
    //         return std::to_string(p.first) + "->" + p.second->userId;
    //     }));

    //     printx("post2idx:", HashmapToString(g.post2idx_map_));
    //     printx("idx2post:", HashmapToString(g.idx2post_map_, [](const auto& p) {
    //         return std::to_string(p.first) + "->" + p.second->post_id;
    //     }));

    //     printx("accout_posts:", VecToString(g.account_posts_, [](const auto& vec) {
    //         return VecToString(vec);
    //     }));

    //     printx("post_interacts:", VecToString(g.post_interactions_, [](const auto& vec) {
    //         return VecToString(vec);
    //     }));
    // }

    // {
    //     InteractGraph gg("../datasets/accountInfo.json", "../datasets/postInfo.json", "../datasets/interactInfo.json");
    // }


    return 0;
}