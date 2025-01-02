#pragma once
#include <string>

#include "nlohmann/json.hpp"

namespace gf_graph {

struct AccountInfo {
    std::string userId;
    // std::string screenName;
    // std::string userName;
    // int relatePost_count;
    // int relatePostInteract_count;
    // int relatePostView_count;
    // int relatePostLike_count;
    // int relatePostQuote_count;
    // int relatePostForward_count;
    // int relatePostComment_count;
    // int userTopicComment;
    // int userTopicForward;
    // int userTopicQuote;
    // int fan_count;
    // int follow_count;
    // std::string description;
    // std::string stand;
    // int taskId;

    nlohmann::json data;
};


enum class interact_t {
    FORWARD,
    QUOTE,
    COMMENT
};

struct PostInfo {
    std::string userId;
    std::string post_id;
    // std::string screenName;
    // std::string userName;
    // std::string content;
    // std::string publishTime;
    // std::string postType;
    // int likeNum;
    // int forwardNum;
    // int quoteNum;
    // int commentNum;
    // int interactNum;
    // int taskId;

    nlohmann::json data;
};

struct InteractInfo {
    interact_t itype;
    std::string id;
    std::string userId;
    
    nlohmann::json data;
};

// struct InteractUserInfo : InteractInfo {
//     std::string user_id;
//     std::string user_name;
//     std::string screen_name;
//     std::string description;
//     int fan_count;
//     int follow_count;
//     std::string address;
//     int allPostNum;
//     std::string lastPostLang;
//     std::string lastPostTime;
//     int lastPostLike;
//     int lastPostShare;
// };

// struct CommentInfo : InteractUserInfo {
//     int commentNum;
//     std::string comment_id;
//     std::string full_text;
//     int like_count;
//     int reply_count;
// };


// struct QuoteInfo : InteractUserInfo {
//     int quoteNum;
//     std::string quote_id;
//     std::string full_text;
//     std::string group_name;
//     std::string group_id;
//     int like_count;
//     int reply_count;
//     int forward_count;
//     int view_count;
// };


// struct ForwardInfo : InteractUserInfo {
//     int forwardNum;
//     int forward_id;
//     std::string group_name;
//     std::string group_id;
//     int like_count;
//     int reply_count;
//     int forward_count;
//     int view_count;
// };

}   // namespace gf_graph