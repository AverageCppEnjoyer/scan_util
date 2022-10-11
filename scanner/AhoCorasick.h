#ifndef SCAN_UTIL_AHOCORASICK_H
#define SCAN_UTIL_AHOCORASICK_H

#include <iostream>
#include <map>
#include <optional>
#include <vector>
#include <string>
#include <queue>
#include <cstdint>

//https://ru.wikipedia.org/wiki/Алгоритм_Ахо_—_Корасик
class AhoCorasick;
class BorNode;
using LinksMap = std::map<const char, BorNode*>;

class BorNode {
    friend class AhoCorasick;
private:
    LinksMap trie_step_;
    BorNode* suffix_link_;  // Suffix link. Only for root is nullptr.
    BorNode* closest_term_; // The nearest terminal state. If missing - nullptr
    int32_t word_index_;

    BorNode(BorNode* fail_node = nullptr)
            : suffix_link_(fail_node)
            , closest_term_(nullptr)
            , word_index_(-1)
    { }

    BorNode* GetLink(const char c) const
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
        return (word_index_ >= 0);
    }
};

class AhoCorasick
{
private:
    BorNode root;
    std::vector<std::string> words;
    BorNode* current_state;

    //switch the machine to the following state
    void Step(const char c)
    {
        while (current_state) {
            BorNode* candidate = current_state->GetLink(c);
            if (candidate) {
                current_state = candidate;
                return;
            }
            current_state = current_state->suffix_link_;
        }
        current_state = &root;
    }
    
     //return index of word if pattern matches,std::nullopt otherwise
    std::optional<int32_t> CheckCurrentState() const
    {
        std::optional<int32_t> result = std::nullopt;

        if (current_state->IsTerminal()) {
            result.emplace(current_state->word_index_);
        }

        BorNode* temp_node = current_state->closest_term_;
        if(temp_node) {
            result.emplace(current_state->closest_term_->word_index_);
        }

        return result;
    }

public:
    //add string to trie
    void AddString(const std::string& str)
    {
        BorNode* current_node = &root;
        for (const char sym : str) {
            BorNode* child_node = current_node->GetLink(sym);
            if (!child_node) {
                child_node = new BorNode(&root);
                current_node->trie_step_[sym] = child_node;
            }
            current_node = child_node;
        }
        current_node->word_index_ = words.size();
        words.push_back(str);
    }

    //DFS initialization of state machine from trie
    void Init()
    {
        std::queue<BorNode*> q;
        q.push(&root);
        while (!q.empty()) {
            BorNode* current_node = q.front();
            q.pop();
            for (const auto&[symbol,child] : current_node->trie_step_)
            {
                // Defining suffix_link_ for the childnode
                BorNode* temp_node = current_node->suffix_link_;
                while (temp_node) {
                    BorNode* fail_candidate = temp_node->GetLink(symbol);
                    if (fail_candidate) {
                        child->suffix_link_ = fail_candidate;
                        break;
                    }
                    temp_node = temp_node->suffix_link_;
                }

                // Defining closest terminal node for the childnode using .term of current node
                if (child->suffix_link_->IsTerminal()) {
                    child->closest_term_ = child->suffix_link_;
                }
                else {
                    child->closest_term_ = child->suffix_link_->closest_term_;
                }
                q.push(child);
            }
        }
    }
    
    //search before first match with pattern or text end
    std::optional<int32_t> Search(const std::string& text)
    {
        current_state = &root;
        for (const char sym : text) {
            Step(sym);
            if(auto maybe_match_idx = CheckCurrentState();maybe_match_idx.has_value()) {
                return maybe_match_idx;
            }
        }
        return std::nullopt;
    }
};

#endif //SCAN_UTIL_AHOCORASICK_H
