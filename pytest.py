import pyGFGraph

# 测试 InteractGraph
g = pyGFGraph.InteractGraph()
g.AddAccounts("/mnt/sevenT/changpuc/graph_processing/GFGraph/datasets/accountInfo.json")
g.AddPosts("/mnt/sevenT/changpuc/graph_processing/GFGraph/datasets/postInfo.json")
g.AddInteractions("/mnt/sevenT/changpuc/graph_processing/GFGraph/datasets/interactInfo.json")

topk_accounts = g.GetTopkAccounts(10, 0)
common_accounts = g.GetCommonInteractingAccounts(["user1", "user2"])
oldest_post = g.GetOldestPost("2023-01-01 00:00:00", "2023-12-31 00:00:00")
posts_freq = g.PostsByFreq(3600, "2023-01-01 00:00:00", "2023-12-31 00:00:00", True)


# 测试 LouvainCommunity
communities = pyGFGraph.LouvainCommunity(g, sorted=True)
print("="*50)

print("type(topk_accounts): ", type(topk_accounts))
print("type(common_accounts): ", type(common_accounts))
print("type(oldest_post): ", type(oldest_post))
print("type(posts_freq): ", type(posts_freq))

print("type(communities): ", type(communities))
