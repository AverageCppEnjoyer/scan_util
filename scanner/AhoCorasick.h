#ifndef SCAN_UTIL_AHOCORASICK_H
#define SCAN_UTIL_AHOCORASICK_H

#include <iostream>
#include <map>
#include <optional>
#include <vector>
#include <string>
#include <queue>
#include <cstdint>
#include <memory>

//https://ru.wikipedia.org/wiki/Алгоритм_Ахо_—_Корасик
class AhoCorasick;
class BorNode;
using LinksMap = std::map<const char, std::shared_ptr<BorNode>>;

struct BorNode {
    LinksMap trie_step_;
    std::shared_ptr<BorNode> suffix_link_;  // Suffix link. Only for root is nullptr.
    std::shared_ptr<BorNode> closest_term_; // The nearest terminal state. If missing - nullptr
    int32_t pattern_index_;

    BorNode()
        : suffix_link_(nullptr)
        , closest_term_(nullptr)
        , pattern_index_(-1)
    { }

    //trie transition by symbol c
    std::shared_ptr<BorNode> GetLink(const char c) const
    {
        auto iter = trie_step_.find(c);
        if (iter != trie_step_.cend()) {
            return iter->second;
        }
        else {
            return nullptr;
        }
    }

    bool IsTerminal() const
    {
        return (pattern_index_ >= 0);
    }
};

class AhoCorasick
{
private:
    std::shared_ptr<BorNode> root_ = nullptr;
    std::vector<std::string> patterns_;
    std::shared_ptr<BorNode> current_state_ = nullptr;

    //switch the machine to the following state
    void Step(const char c)
    {
        while (current_state_) {
            auto candidate = current_state_->GetLink(c);
            if (candidate) {
                current_state_ = candidate;
                return;
            }
            current_state_ = current_state_->suffix_link_;
        }
        current_state_ = root_;
    }

    //return index of word if pattern matches,std::nullopt otherwise
    std::optional<int32_t> CheckCurrentState() const
    {
        std::optional<int32_t> result{ std::nullopt };

        if (current_state_->IsTerminal()) {
            result.emplace(current_state_->pattern_index_);
        }

        if (auto temp_node = current_state_->closest_term_; temp_node) {
            result.emplace(temp_node->pattern_index_);
        }

        return result;
    }
public:
    AhoCorasick() : root_(std::make_shared<BorNode>()) {}

    //add string to trie
    void AddString(const std::string& str)
    {
        auto current_node = root_;
        for (const char sym : str) {
            auto child_node = current_node->GetLink(sym);
            if (!child_node) {
                child_node = std::make_shared<BorNode>();
                child_node->suffix_link_ = root_;
                current_node->trie_step_[sym] = child_node;
            }
            current_node = child_node;
        }
        current_node->pattern_index_ = patterns_.size();
        patterns_.push_back(str);
    }

    //DFS initialization of state machine
    void Init()
    {
        std::queue<std::shared_ptr<BorNode>> q;
        q.push(root_);

        while (!q.empty()) {
            auto current_node = q.front();
            q.pop();
            for (auto& [symbol, child] : current_node->trie_step_)
            {
                // Defining suffix_link_ for the childnode
                auto temp_node = current_node->suffix_link_;
                while (temp_node) {
                    auto fail_candidate = temp_node->GetLink(symbol);
                    if (fail_candidate) {
                        child->suffix_link_ = fail_candidate;
                        break;
                    }
                    temp_node = temp_node->suffix_link_;
                }

                // Defining closest terminal node for the childnode using .term of current node
                if (child->suffix_link_ && child->suffix_link_->IsTerminal()) {
                    child->closest_term_ = child->suffix_link_;
                }
                else {
                    child->closest_term_ = child->suffix_link_->closest_term_;
                }
                q.push(child);
            }
        }
    }

    //search before first match with pattern or text ends
    std::optional<int32_t> Search(const std::string& text)
    {
        std::optional<int32_t> result{ std::nullopt };

        current_state_ = root_;
        for (const char sym : text) {
            Step(sym);
            if (auto maybe_match_idx = CheckCurrentState(); maybe_match_idx.has_value()) {
                result.emplace(maybe_match_idx.value());
            }
        }
        return result;
    }
};

#endif //SCAN_UTIL_AHOCORASICK_H
