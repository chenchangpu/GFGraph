#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <array>
#include <memory>
#include <fstream>
#include <cassert>

#include <thrust/system/omp/execution_policy.h>
#include <thrust/sequence.h>
#include <thrust/transform.h>
#include <thrust/for_each.h>
#include <thrust/sort.h>

#include "nlohmann/json.hpp"
#include "parallel_hashmap/phmap.h"

#include "info.hpp"
#include "datetime.hpp"
#include "str.hpp"
#include "log.hpp"

namespace gf_graph {

using vid_t = int32_t;
using eid_t = int32_t;

template <typename K, typename V>
using hashmap = phmap::flat_hash_map<K, V>;
template <typename K>
using hashset = phmap::flat_hash_set<K>;

using json = nlohmann::json;

enum class topk_label_t {
    // 发帖数量
    POSTS,
    // 被互动次数
    INTERACTED_TIMES,
    // 被互动账号数
    INTERACTED_ACCOUNTS,
    // 参与互动次数
    INTERACTING_TIMES,
    // 参与互动的帖子数
    INTERACTING_POSTS,
    // 参与互动的发帖账号数
    INTERACTING_ACCOUNTS
};

class InteractGraph {
public:
    // each directed edge between two accounts, with interactions of posts [post-index,...]
    using acc_edata_t = std::vector<vid_t>;

    InteractGraph() = default;

    InteractGraph(const std::string& account_path, const std::string& post_path,
                  const std::string& interact_path, bool replace = true) {
        AddAccounts(account_path, replace);
        AddPosts(post_path, replace);
        AddInteractions(interact_path, replace);
    }

    vid_t num_accounts() const {
        return account2idx_map_.size();
    }

    vid_t num_posts() const {
        return post2idx_map_.size();
    }

    eid_t num_interactions() const {
        return interact_map_.size();
    }

    std::string idx2account(vid_t uidx) const {
        auto it = idx2account_map_.find(uidx);
        if (it == idx2account_map_.end()) {
            return {};
        }
        return it->second->userId;
    }


    std::string idx2post(vid_t post_idx) const {
        auto it = idx2post_map_.find(post_idx);
        if (it == idx2post_map_.end()) {
            return {};
        }
        return it->second->post_id;       
    }


    const std::vector<hashmap<vid_t, acc_edata_t>>& account_graph() const {
        return account_graph_;
    }

    void AddAccounts(const std::string& account_path, bool replace = true) {
        std::ifstream fin(account_path);
        json data = json::parse(fin);
        assert(data.is_array());

        for (json::iterator it = data.begin(); it != data.end(); ++it) {
            if (!AddAccount_(*it, replace)) {
                LOG_WARNING("the account has been added before: userId=", it->at("userId"));
            }        
        }
    }

    void AddPosts(const std::string& post_path, bool replace = true) {
        std::ifstream fin(post_path);
        json data = json::parse(fin);
        assert(data.is_array());

        for (json::iterator it = data.begin(); it != data.end(); ++it) {
            if (!AddPost_(*it, replace)) {
                LOG_WARNING("the post has been added before: post_id=", it->at("post_id"));
            }        
        }     
    }

    void AddInteractions(const std::string& interact_path, bool replace = true) {
        std::ifstream fin(interact_path);
        json data = json::parse(fin);
        assert(data.is_array());

        for (json::iterator it = data.begin(); it != data.end(); ++it) {
            AddInteractions_(*it, replace);
        }
    }

    std::shared_ptr<AccountInfo> get_account(const std::string& userId) const {
        auto it = account2idx_map_.find(userId);
        if (it == account2idx_map_.end()) {
            return {};
        }
        vid_t uidx = it->second;
        return idx2account_map_.at(uidx);
    }

    std::shared_ptr<PostInfo> get_post(const std::string& post_id) const {
        auto it = post2idx_map_.find(post_id);
        if (it == post2idx_map_.end()) {
            return {};
        }
        vid_t post_idx = it->second;
        return idx2post_map_.at(post_idx);
    }

    std::shared_ptr<InteractInfo> get_interact(const std::string& id) const {
        auto it = interact_map_.find(id);
        if (it == interact_map_.end()) {
            return {};
        }
        return it->second;
    }


    std::vector<std::pair<std::string, int>> 
    GetTopkAccounts(vid_t topk, topk_label_t label) const {
        vid_t num_accounts = account2idx_map_.size();
        topk = std::min(topk, num_accounts);
        std::vector<int> indices(num_accounts);
        std::vector<int> counts(num_accounts, 0);
        thrust::sequence(thrust::omp::par, indices.begin(), indices.end());

        LOG_DEBUG("before setting counts");
        switch (label) {
            case topk_label_t::POSTS: {
                thrust::transform(thrust::omp::par, account_posts_.begin(), account_posts_.end(), 
                    counts.begin(), [](const std::vector<vid_t>& posts) {
                        return posts.size();
                    });
                break;
            }
            case topk_label_t::INTERACTED_TIMES: {
                thrust::transform(thrust::omp::par, account_posts_.begin(), account_posts_.end(), 
                    counts.begin(), [this](const std::vector<vid_t>& posts) {
                        int count = 0;
                        for (vid_t post_idx: posts) {
                            count += post_interactions_[post_idx].size();
                        }
                        return count;
                    });
                break;
            }
            case topk_label_t::INTERACTED_ACCOUNTS: {
                thrust::transform(thrust::omp::par, account_graph_.begin(), account_graph_.end(), 
                    counts.begin(), [](const hashmap<vid_t, acc_edata_t>& accounts) {
                        return accounts.size();
                    });
                break;
            }
            case topk_label_t::INTERACTING_TIMES: {
                thrust::for_each(thrust::omp::par, account_graph_.begin(), account_graph_.end(), 
                    [&counts](const hashmap<vid_t, acc_edata_t>& accounts) {
                        for (auto& [uidx, edata]: accounts) {
                            int cnt = edata.size();
                            #pragma omp atomic
                            counts[uidx] += cnt;
                        }
                    });
                break;
            }
            case topk_label_t::INTERACTING_ACCOUNTS: {
                thrust::for_each(thrust::omp::par, account_graph_.begin(), account_graph_.end(), 
                    [&counts](const hashmap<vid_t, acc_edata_t>& accounts) {
                        for (auto& [uidx, edata]: accounts) {
                            #pragma omp atomic
                            ++counts[uidx];
                        }
                    });                
                break;
            }
            case topk_label_t::INTERACTING_POSTS: {
                thrust::for_each(thrust::omp::par, account_graph_.begin(), account_graph_.end(),
                    [&counts](const hashmap<vid_t, acc_edata_t>& accounts) {
                        hashset<vid_t> posts;
                        for (auto& [uidx, edata]: accounts) {
                            posts.clear();
                            for (vid_t post_idx: edata) {
                                posts.insert(post_idx);
                            }
                            int cnt = posts.size();
                            #pragma omp atomic
                            counts[uidx] += cnt;
                        }
                    });
                break;
            }
            default:
                LOG_ERROR("the topk lable is NOT supported: label=", int(label));
        }
        LOG_DEBUG("counts: ", VecToString(counts));

        thrust::sort_by_key(thrust::omp::par, counts.begin(), counts.end(), 
            indices.begin(), thrust::greater<int>());
        
        LOG_DEBUG("indices: ", VecToString(indices));

        std::vector<std::pair<std::string, int>> results(topk);
        for (int i = 0; i < topk; ++i) {
            vid_t uidx = indices[i];
            int count = counts[i];
            results[i] = std::make_pair(idx2account_map_.at(uidx)->userId, count);
        }
        
        return results;
    }

    // GetTopKAccount重载
    std::vector<std::pair<std::string, int>> 
    GetTopkAccounts(vid_t topk, int label) const{
        assert(label < 6 && label >= 0);
        if(label == 0) return GetTopkAccounts(topk, topk_label_t::POSTS);
        if(label == 1) return GetTopkAccounts(topk, topk_label_t::INTERACTED_TIMES);
        if(label == 2) return GetTopkAccounts(topk, topk_label_t::INTERACTED_ACCOUNTS);
        if(label == 3) return GetTopkAccounts(topk, topk_label_t::INTERACTING_TIMES);
        if(label == 4) return GetTopkAccounts(topk, topk_label_t::INTERACTING_POSTS);
        if(label == 5) return GetTopkAccounts(topk, topk_label_t::INTERACTING_ACCOUNTS);
    }

    std::vector<std::string> GetCommonInteractingAccounts(const std::vector<std::string>& userIds) const {
        std::vector<vid_t> uidxs;
        for (const std::string& uid: userIds) {
            auto it = account2idx_map_.find(uid);
            if (it == account2idx_map_.end()) {
                LOG_WARNING("the userId is NOT exist, ignore: userId=", uid);
                continue;
            }
            uidxs.push_back(it->second);
        }
        if (uidxs.empty()) {
            return {};
        }
        
        hashset<vid_t> base, commons;
        for (auto& [uidx, _]: account_graph_.at(uidxs[0])) {
            commons.insert(uidx);
        }
        for (size_t i = 1; i < uidxs.size(); ++i) {
            std::swap(commons, base);
            commons.clear();
            for (auto& [uidx, _]: account_graph_.at(uidxs[i])) {
                if (base.count(uidx)) {
                    commons.insert(uidx);
                }
            }
            if (commons.empty()) {
                break;
            }
        }
        std::vector<std::string> result;
        for (vid_t uidx: commons) {
            result.push_back(idx2account_map_.at(uidx)->userId);
        }
        return result;
    }


    std::array<std::string, 3>
    GetOldestPost(const std::string& start_time, const std::string& end_time) const {
        return GetOldestPost(DateTime(start_time), DateTime(end_time));
    }

    // get the oldest post and its userId from datetime [start_time, end_time] (both including)
    std::array<std::string, 3>
    GetOldestPost(const DateTime& start_time = MIN_DATETIME, const DateTime& end_time = MAX_DATETIME) const {
        std::string oldest_uid;
        std::string oldest_post_id;
        DateTime oldest_datetime = end_time;
        for (auto& [_, post_ptr]: idx2post_map_) {
            const json& data = post_ptr->data;
            if (data.is_null()) {
                continue;
            }
            auto it = data.find("publishTime");
            if (it == data.end()) {
                LOG_WARNING("the `publishTime` is NOT in data of the post, ignore: post_id=",
                            post_ptr->userId);
                continue;
            }
            const std::string& time_str = *it;
            DateTime cur = DateTime(time_str);
            if (start_time <= cur && cur <= oldest_datetime) {
                oldest_datetime = cur;
                oldest_uid = post_ptr->userId;
                oldest_post_id = post_ptr->post_id;
            }
        }
        if (oldest_post_id.empty()) {
            return {};
        }
        return { oldest_post_id, oldest_datetime.ToString(), oldest_uid };
    }

    std::pair<std::vector<std::string>, std::vector<int>>
    PostsByFreq(const TimeRange& interval = TIMERANGE_DAY, 
                const DateTime& start_time = MIN_DATETIME, const DateTime& end_time = MAX_DATETIME,
                bool sorted = true) const {
        auto interval_secs = interval.seconds;
        assert(interval.seconds > 0);
        
        auto base_secs = start_time.UnixSeconds();
        auto end_secs = end_time.UnixSeconds();

        hashmap<uint64_t, int> post_buckets;

        for (auto& [_, post_ptr]: idx2post_map_) {
            const json& data = post_ptr->data;
            if (data.is_null()) {
                continue;
            }
            auto it = data.find("publishTime");
            if (it == data.end()) {
                LOG_WARNING("the `publishTime` is NOT in data of the post, ignore: post_id=",
                            post_ptr->userId);
                continue;
            }
            const std::string& time_str = *it;
            auto cur_secs = DateTime(time_str).UnixSeconds();
            if (cur_secs < base_secs || cur_secs > end_secs) {
                continue;
            }
            int bucket_id = (cur_secs - base_secs) / interval_secs;
            ++post_buckets[bucket_id];
        }

        int sz = post_buckets.size();
        std::vector<int> indices;
        std::vector<int> counts;
        indices.reserve(sz);
        counts.reserve(sz);
        for (auto& [id, cnt]: post_buckets) {
            indices.push_back(id);
            counts.push_back(cnt);
        }

        if (sorted) { 
            thrust::sort_by_key(thrust::omp::par, counts.begin(), counts.end(), 
                indices.begin(), thrust::greater<int>());  
        }

        std::vector<std::string> dates(sz);
        thrust::transform(thrust::omp::par, indices.begin(), indices.end(),
            dates.begin(), [base_secs, interval_secs](const int& offsets) {
                return DateTime::FromUnixSeconds(base_secs + offsets * interval_secs).ToString();
            });
        
        return std::make_pair(dates, counts);
    }

    std::pair<std::vector<std::string>, std::vector<int>>
    PostsByFreq(const TimeRange& interval, 
                const std::string& start_time, const std::string& end_time,
                bool sorted = true) const {
        return PostsByFreq(interval, DateTime(start_time), DateTime(end_time), sorted);
    }

    // 重载第一个参数int/uint64_t型输入 -> 转换为 TimeRange类型
    std::pair<std::vector<std::string>, std::vector<int>>
    PostsByFreq(const uint64_t& interval, 
                const DateTime& start_time = MIN_DATETIME, const DateTime& end_time = MAX_DATETIME,
                bool sorted = true) const{
        return PostsByFreq(TimeRange(interval), start_time, end_time, sorted);
    }

    std::pair<std::vector<std::string>, std::vector<int>>
    PostsByFreq(const uint64_t& interval, 
                const std::string& start_time, const std::string& end_time,
                bool sorted = true) const {
        return PostsByFreq(TimeRange(interval), DateTime(start_time), DateTime(end_time), sorted);
    }


private:

    bool AddAccount_(std::string uid, const json* data, bool replace) {
        bool added = true;
        std::shared_ptr<AccountInfo> info;
        if (auto it = account2idx_map_.find(uid); it != account2idx_map_.end()) {
            added = false;
            if (!replace) {
                return added;
            }
            vid_t uidx = it->second;
            info = idx2account_map_[uidx];
        } else {
            info = std::make_shared<AccountInfo>();
            info->userId = uid;
            
            vid_t uidx = account2idx_map_.size();
            account2idx_map_.emplace(uid, uidx);
            idx2account_map_.emplace(uidx, info);

            account_posts_.push_back({});
            account_graph_.push_back({});
        }
        
        if (data) {
            info->data = *data;
        }
        return added;
    }

    bool AddAccount_(const json& data, bool replace) {
        assert(data.is_object());

        std::string uid = data["userId"];
        return AddAccount_(uid, &data, replace);
    }

    bool AddPost_(std::string post_id, const json* data, bool replace, std::string uid = "") {
        bool added = true;
        std::shared_ptr<PostInfo> info;
        vid_t post_idx;
        if (auto it = post2idx_map_.find(post_id); it != post2idx_map_.end()) {
            added = false;
            if (!replace) {
                return added;
            }
            post_idx = it->second;
            info = idx2post_map_[post_idx];
        } else {
            info = std::make_shared<PostInfo>();
            info->post_id = post_id;
            
            post_idx = post2idx_map_.size();
            post2idx_map_.emplace(post_id, post_idx);
            idx2post_map_.emplace(post_idx, info);

            post_interactions_.push_back({});
        }
        
        if (!uid.empty() || data) {
            if (uid.empty()) {
                uid = (*data)["userId"];
            }
            if (AddAccount_(uid, nullptr, false)) {
                LOG_WARNING("add the account when adding the post: userId=", 
                            uid, ", post_id=", post_id);
            }
            if (info->userId.empty()) {
                info->userId = uid;
                account_posts_[account2idx_map_[uid]].push_back(post_idx);
            } else {    // when replacing the post
                assert(info->userId == uid);
            }

            if (data) {
                info->data = *data;
            }
        }
        return added;
    }

    bool AddPost_(const json& data, bool replace) {
        assert(data.is_object());

        std::string post_id = data["post_id"];
        return AddPost_(post_id, &data, replace);
    }

    bool AddInteraction_(interact_t itype, const std::string& post_id, const json& data, bool replace) {
        assert(data.is_object());
        std::string id;

        if (itype == interact_t::FORWARD) {
            id = data["forward_id"];
        } else if (itype == interact_t::QUOTE) {
            id = data["quote_id"];
        } else {    // itype == interact_t::COMMENT
            id = data["comment_id"];
        }

        bool added = true;
        std::shared_ptr<InteractInfo> info;
        std::string dst_uid = data["user_id"];
        if (auto it = interact_map_.find(id); it != interact_map_.end()) {
            added = false;
            if (!replace) {
                return added;
            }
            info = it->second;
        } else {
            info = std::make_shared<InteractInfo>();
            info->id = id;
            info->itype = itype;
            
            if (AddAccount_(dst_uid, nullptr, false)) {
                LOG_WARNING("add the account when adding the interaction: userId=", 
                            dst_uid, ", interact_id=", id);                
            }
            // forward or quote the post will create a new post
            if (itype == interact_t::FORWARD || itype == interact_t::QUOTE) {
                AddPost_(id, nullptr, false, dst_uid);
            }

            vid_t post_idx = post2idx_map_[post_id];
            post_interactions_[post_idx].push_back(id);
            interact_map_.emplace(id, info);

            vid_t dst_uidx = account2idx_map_[dst_uid];
            std::string src_uid = idx2post_map_[post_idx]->userId;
            // don't know the account's userId of the post if no info about the post
            if (!src_uid.empty()) {
                vid_t src_uidx = account2idx_map_[src_uid];
                auto& dst_list = account_graph_[src_uidx];
                auto iit = dst_list.find(dst_uidx);
                if (iit == dst_list.end()) {
                    iit = dst_list.emplace(dst_uidx, acc_edata_t{}).first;
                }
                // LOG_DEBUG("add interact: src_uid=", src_uid, ", src_uidx=", src_uidx,
                //           "| dst_uid=", dst_uid, ", dst_uidx=", dst_uidx, 
                //           "| post_id=", post_id, ", post_idx=", post_idx);
                iit->second.push_back(post_idx);
            }
        }

        if (info->userId.empty()) {
            info->userId = dst_uid;
        } else {    // when replacing the interaction
            assert(info->userId == dst_uid);
        }
        return added;
    }


    void AddInteractions_(const json& data, bool replace) {
        assert(data.is_object());

        std::string post_id = data["postId"];
        if (AddPost_(post_id, nullptr, false, "")) {    // don't know the account of the post
            LOG_WARNING("add the post when adding the interaction: postId=", 
                        post_id);
        }

        for (json::const_iterator it = data["forward"].begin(); it != data["forward"].end(); ++it) {
            if (!AddInteraction_(interact_t::FORWARD, post_id, *it, replace)) {
                LOG_WARNING("the forward-interaction has been added before: forward_id=", 
                        it->at("forward_id"));
            }
        }
        for (json::const_iterator it = data["quote"].begin(); it != data["quote"].end(); ++it) {
            if (!AddInteraction_(interact_t::QUOTE, post_id, *it, replace)) {
                LOG_WARNING("the quote-interaction has been added before: quote_id=", 
                        it->at("quote_id"));
            }
        }
        for (json::const_iterator it = data["comment"].begin(); it != data["comment"].end(); ++it) {
            if (!AddInteraction_(interact_t::COMMENT, post_id, *it, replace)) {
                LOG_WARNING("the comment-interaction has been added before: comment_id=", 
                        it->at("comment_id"));
            }            
        }
    }


public:

    // account's "userId" to his index
    hashmap<std::string, vid_t> account2idx_map_;
    // account index to his info
    hashmap<vid_t, std::shared_ptr<AccountInfo>> idx2account_map_;

    // post's "postId" to its index
    hashmap<std::string, vid_t> post2idx_map_;
    // posts's index to its info
    hashmap<vid_t, std::shared_ptr<PostInfo>> idx2post_map_;

    // interactions's id(forward_id/quote_id/comment_id) to its info
    hashmap<std::string, std::shared_ptr<InteractInfo>> interact_map_;

    // posts of the account (account-index -> [post-index, ...])
    std::vector<std::vector<vid_t>> account_posts_;
    // interactions(forward/quote/comment) of the post (post-index -> [interact_id, ...])
    std::vector<std::vector<std::string>> post_interactions_;

    // account graph with interactions of posts ( account-index -> (account-index -> acc_edata_t) )
    std::vector<hashmap<vid_t, acc_edata_t>> account_graph_;
};

}   // namespace gf_graph