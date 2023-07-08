#pragma once
#include <stddef.h>
#include <string>
#include <functional>
#include <Runtime/MemoryManager.hpp>


namespace XyA
{
    namespace Runtime
    {
        template <typename T, size_t N = 15>
        class StrKeyDict
        {
        private:
            struct Node
            {
                std::string key;
                T value;
                Node* next = nullptr;
            };

            class Iterator
            {
            public:
                Iterator(Node** node_list_heads ,Node* node, size_t index)
                {
                    this->__node_list_heads = node_list_heads;
                    this->__node = node;
                    this->__index = index;
                }

                Iterator& operator++()
                {
                    this->__node = this->__node->next;
                    if (this->__node == nullptr)
                    {
                        for (size_t i = this->__index + 1; i < N; i ++)
                        {
                            if (this->__node_list_heads[i] != nullptr)
                            {
                                this->__node = this->__node_list_heads[i];
                                this->__index = i;
                                break;
                            }
                        }
                    }
                    return *this;
                }

                bool operator!=(const Iterator& other) const
                {
                    return this->__node != other.__node;
                }

                bool operator==(const Iterator& other) const
                {
                    return this->__node == other.__node;
                }

                Node* operator*()
                {
                    return this->__node;
                }

            private:
                Node** __node_list_heads;
                Node* __node;
                size_t __index;
            };

        public:
            StrKeyDict()
            {
                using node_ptr_t = Node*;
                this->__node_list_heads = XyA_Allocate_Array(node_ptr_t, N, nullptr);
            }

            ~StrKeyDict()
            {
                for (size_t i = 0; i < N; i ++)
                {
                    while (this->__node_list_heads[i] != nullptr)
                    {
                        Node* node = this->__node_list_heads[i];
                        this->__node_list_heads[i] = this->__node_list_heads[i]->next;
                        XyA_Deallocate(node);
                    }
                }
                XyA_Deallocate_Array(this->__node_list_heads);
            }

            T& operator[](const std::string& key)
            {
                size_t index = this->__hash(key);
                
                Node* node = this->__node_list_heads[index];
                while (node != nullptr)
                {
                    if (node->key == key)
                    {
                        return node->value;
                    }
                    node = node->next;
                }

                node = XyA_Allocate_(Node);
                node->key = key;
                node->next = this->__node_list_heads[index];
                this->__node_list_heads[index] = node;
                return node->value;
            }

            bool try_get(const std::string& key, T& result_value) const
            {
                size_t index = this->__hash(key);
                
                Node* node = this->__node_list_heads[index];
                while (node != nullptr)
                {
                    if (node->key == key)
                    {
                        result_value = node->value;
                        return true;
                    }
                    node = node->next;
                }
                return false;
            }

            void remove(const std::string& key)
            {
                size_t index = this->__hash(key);
                
                Node* prev = nullptr;
                Node* node = this->__node_list_heads[index];
                while (node != nullptr)
                {
                    if (node->key == key)
                    {
                        if (prev == nullptr)
                        {
                            this->__node_list_heads[index] = node->next;
                        }
                        else
                        {
                            prev->next = node->next;
                        }
                        XyA_Deallocate(node);
                        return;
                    }
                    prev = node;
                    node = node->next;
                }
            }

            Iterator begin() const
            {
                for (size_t i = 0; i < N; i ++)
                {
                    if (this->__node_list_heads[i] != nullptr)
                    {
                        return Iterator(this->__node_list_heads, this->__node_list_heads[i], i);
                    }
                }
                return Iterator(this->__node_list_heads, nullptr, N);
            }

            Iterator end() const
            {
                return Iterator(this->__node_list_heads, nullptr, N);
            }

        private:
            size_t __hash(const std::string& key) const
            {
                return std::hash<std::string>()(key) % N;
            }

            Node** __node_list_heads;
        };
    }
}