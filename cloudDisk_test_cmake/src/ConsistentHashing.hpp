#include <iostream>
#include <unordered_map>
#include <string>
#include <cmath>
#include <vector>
#include <algorithm>
#include <random>
// 假定Node为存储节点，这里简化为一个整数标识符
class Node 
{
public:
    int id;
    explicit Node(int id) : id(id) {}
};

// 哈希环
class ConsistentHashing 
{
public:
    void addNode(Node* node) 
    {
        for (int i = 0; i < VIRTUAL_NODE_NUM; ++i) 
        {
            std::string virtual_node_key = std::to_string(node->id) + "_" + std::to_string(i);
            int hash_value = hash(virtual_node_key);
            virtual_nodes[hash_value] = node;
            ring.push_back(hash_value);
        }
        std::sort(ring.begin(), ring.end());
    }

    void removeNode(Node* node) {
        for (int i = 0; i < VIRTUAL_NODE_NUM; ++i) 
        {
            std::string virtual_node_key = std::to_string(node->id) + "_" + std::to_string(i);
            int hash_value = hash(virtual_node_key);
            virtual_nodes.erase(hash_value);
            auto it = std::find(ring.begin(), ring.end(), hash_value);
            if (it != ring.end()) 
            {
                ring.erase(it);
            }
        }
    }

    Node* getNode(const std::string& key) 
    {
        int hash_value = hash(key);
        // 使用二分查找找到第一个大于等于hash_value的位置
        auto it = std::lower_bound(ring.begin(), ring.end(), hash_value);
        if (it == ring.end()) 
        {
            it = ring.begin(); // 如果key的哈希值大于环上所有节点的哈希值，则返回第一个节点
        }
        return virtual_nodes[*it];
    }
private:
    // 计算哈希值的函数，这里使用简单的哈希函数
    int hash(const std::string& key) const 
    {
        int hash = 0;
        for (char c : key) 
        {
            hash = (hash * 31 + c) % 4096; // 4096为哈希环大小，可根据需要调整
        }
        return hash;
    }
private:
    std::unordered_map<int, Node*> virtual_nodes; // 虚拟节点到真实节点的映射
    std::vector<int> ring; // 哈希环，存储虚拟节点的哈希值
    static const int VIRTUAL_NODE_NUM = 100; // 每个真实节点对应的虚拟节点数量
};

class LoadBalancer 
{
public:
    void initServers(const std::vector<Node*>& servers) 
    {
        for (Node* server : servers) 
        {
            hashing.addNode(server);
        }
    }

    Node* distributeRequest(const std::string& requestKey) 
    {
        return hashing.getNode(requestKey);
    }

    // 模拟生成并分配请求
    int simulateRequests(const std::vector<Node*>& servers,std::string&filesha1) 
    {
        initServers(servers); // 初始化服务器
        std::string requestKey = "request_username" + filesha1;
        Node* targetNode = distributeRequest(requestKey);
        return  targetNode->id; 
    }

private:
    ConsistentHashing hashing;
};
#if 0
int main() 
{   
    Node n1(1), n2(2), n3(3);
    std::vector<Node*> servers = {&n1, &n2, &n3};
    LoadBalancer balancer;
    balancer.simulateRequests(servers);
    return 0;
}
#endif
