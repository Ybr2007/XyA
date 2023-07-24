#pragma once
#include <stddef.h>
#include <functional>
#include <TypeDefs.h>
#include <Runtime/MemoryManager.hpp>


namespace XyA
{
    namespace Runtime
    {
        template <typename T, Id N = 15>
        class IdKeyDict
        {
        private:
            struct Node
            {
                Id key;
                T value;
                Node* next = nullptr;
            };

            class Iterator
            {
            public:
                Iterator(Node** node_list_heads ,Node* node, Id index)
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
                        for (Index i = this->__index + 1; i < N; i ++)
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
                Id __index;
            };

        public:
            IdKeyDict()
            {
                using node_ptr_t = Node*;
                this->__node_list_heads = XyA_Allocate_Array(node_ptr_t, N, nullptr);
            }

            ~IdKeyDict()
            {
                for (Index i = 0; i < N; i ++)
                {
                    while (this->__node_list_heads[i] != nullptr)
                    {
                        Node* node = this->__node_list_heads[i];
                        this->__node_list_heads[i] = this->__node_list_heads[i]->next;
                        XyA_Deallocate(node);
                    }
                }
                XyA_Deallocate_Array(this->__node_list_heads, N);
            }

            T& operator[](Id key)
            {
                Index index = this->__hash(key);
                
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

            bool try_get(Id key, T& result_value) const
            {
                Index index = this->__hash(key);
                
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

            void remove(Id key)
            {
                Index index = this->__hash(key);
                
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
                for (Index i = 0; i < N; i ++)
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
            Index __hash(Id key) const
            {
                return key % N;
            }

            Node** __node_list_heads;
        };
    }
}