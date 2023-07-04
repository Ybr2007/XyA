#pragma once
#include <stddef.h>
#include <utility>
#include <unordered_set>
#include <Runtime/Object.h>
#include <Config.h>

#ifdef Debug_Display_Memory_Leaks
#include <cstdio>
#include <string>
#include <vector>
#include <map>
#endif


namespace XyA
{
    namespace Runtime
    {
        #ifndef Debug_Display_Memory_Leaks
        #define XyA_Allocate_(T) MemoryPool::get_instance()->allocate<T>()
        #define XyA_Allocate(T, args) MemoryPool::get_instance()->allocate<T>(args)
        #define XyA_Deallocate(object) MemoryPool::get_instance()->deallocate(object)
        #endif

        #ifdef Debug_Display_Memory_Leaks
        #define STRINGIFY(x) #x
        #define TOSTRING(x) STRINGIFY(x)
        #define FILE_LINE std::string(__FILE__) + "(" + std::string(TOSTRING(__LINE__)) + ")"
        #define XyA_Allocate_(T) MemoryPool::get_instance()->allocate<T>(FILE_LINE)
        #define XyA_Allocate(T, args) MemoryPool::get_instance()->allocate<T>(FILE_LINE, args)
        #define XyA_Deallocate(object) MemoryPool::get_instance()->deallocate(FILE_LINE, object)
        #endif        

        struct Block
        {
            Block* next_block;
        };

        class Chunk
        {
        public:
            static constexpr size_t chunk_batch_allocate_size = 4096;  // 每次分配新的block的总大小

            Block* free_block_head = nullptr;  // 未分配的Block链表的链表头
            size_t size;  // 该Chunk中Block的大小
            std::unordered_set<char*> allocated_memory_heads;  // 使用malloc分配的地址, 记录以供后续free

            Chunk(size_t size)
            {
                this->size = size;
            }

            ~Chunk()
            {
                this->clear();
            }

            void new_blocks()
            {
                size_t new_block_num = chunk_batch_allocate_size / size;
                char* allocated_memory_head = (char*)malloc(new_block_num * size);
                this->allocated_memory_heads.insert(allocated_memory_head);
                for (size_t i = 0; i < new_block_num; i ++)
                {
                    Block* new_block = reinterpret_cast<Block*>(allocated_memory_head + size * i);
                    this->push_block(new_block);
                }
            }

            Block* pop_free_block()
            {
                if (this->free_block_head == nullptr)
                {
                    this->new_blocks();
                }

                Block* block = this->free_block_head;
                this->free_block_head = block->next_block;
                return block;
            }

            void push_block(Block* block)
            {
                if (block == nullptr) 
                {
                    return;
                }

                block->next_block = this->free_block_head;
                this->free_block_head = block;
            }

            void clear()
            {
                for (auto iter = this->allocated_memory_heads.begin(); 
                    iter != this->allocated_memory_heads.end(); iter ++)
                {
                    free(*iter);
                }
            }
        };

        class MemoryPool
        {
        public:
            static constexpr size_t cpp_new_and_del_threshold_size = 512;  // 当对象的大小超过该阈值时, 直接使用new/delete进行分配/释放
            static constexpr size_t alignment = 8;  // Block大小与alignment对齐

            #ifdef Debug_Display_Memory_Leaks
            std::unordered_set<void*> allocated_objects;
            std::map<void*, std::string> allocation_locations;
            std::map<void*, std::string> deallocation_locations;
            #endif

            static MemoryPool* get_instance()
            {
                static MemoryPool instance;
                return &instance;
            }

            #ifndef Debug_Display_Memory_Leaks
            template <typename T, typename... Args>
            T* allocate(Args&&... args)
            {
                size_t aligned_size = this->__get_aligned_size(sizeof(T));

                if (aligned_size > cpp_new_and_del_threshold_size)
                {
                    return new T(std::forward<Args>(args)...);
                }

                size_t chunk_index = aligned_size / alignment;
                Block* block = this->__chunks[chunk_index]->pop_free_block();
                return new(block) T(std::forward<Args>(args)...);
            }
            #endif

            #ifdef Debug_Display_Memory_Leaks
            template <typename T, typename... Args>
            T* allocate(std::string location, Args&&... args)
            {
                size_t aligned_size = this->__get_aligned_size(sizeof(T));

                if (aligned_size > cpp_new_and_del_threshold_size)
                {
                    T* object = new T(std::forward<Args>(args)...);
                    this->allocated_objects.insert(object);
                    this->allocation_locations[object] = location;
                }

                size_t chunk_index = aligned_size / alignment;
                Block* block = this->__chunks[chunk_index]->pop_free_block();
                T* object = new(block) T(std::forward<Args>(args)...);
                
                this->allocated_objects.insert(object);
                this->allocation_locations[object] = location;

                return object;
            }
            #endif

            #ifndef Debug_Display_Memory_Leaks
            template <typename T>
            void deallocate(T* object)
            {
                size_t aligned_size = this->__get_aligned_size(sizeof(T));

                if (aligned_size > cpp_new_and_del_threshold_size)
                {
                    delete object;
                    return;
                }

                object->~T();
                
                size_t chunk_index = aligned_size / alignment;
                Block* block = reinterpret_cast<Block*>(object);
                
                this->__chunks[chunk_index]->push_block(block);
            }
            #endif

            #ifdef Debug_Display_Memory_Leaks
            template <typename T>
            void deallocate(std::string location, T* object)
            {
                bool ok = false;
                for (auto iter = this->allocated_objects.begin(); iter != this->allocated_objects.end(); iter ++)
                {
                    if (*iter == object)
                    {
                        this->allocated_objects.erase(iter);
                        ok = true;
                        break;
                    }
                }

                if (!ok)
                {
                    auto iter = this->deallocation_locations.find(object);
                    if (iter == this->deallocation_locations.end())
                    {
                        printf("\nERROR: The object was not allocated by the memory pool\n");
                        printf("ERROR Location: %s\n", location.c_str());
                        exit(-1);
                    }
                    else
                    {
                        printf("\nERROR: The object was already deallocated at %s\n", iter->second.c_str());
                        printf("ERROR Location: %s\n", location.c_str());
                        exit(-1);
                    }
                }
                this->deallocation_locations[object] = location;

                size_t aligned_size = this->__get_aligned_size(sizeof(T));

                if (aligned_size > cpp_new_and_del_threshold_size)
                {
                    delete object;
                    return;
                }

                object->~T();
                
                size_t chunk_index = aligned_size / alignment;
                Block* block = reinterpret_cast<Block*>(object);
                
                this->__chunks[chunk_index]->push_block(block);
            }
            #endif

            ~MemoryPool()
            {
                #ifdef Debug_Display_Memory_Leaks
                printf("\nLEAKS: %d\n", this->allocated_objects.size());

                for (auto iter = this->allocated_objects.begin(); iter != this->allocated_objects.end(); iter ++)
                {
                    printf(
                        "%s %d Allocation Location: %s\n", 
                        ((Runtime::Object*)*iter)->to_string().c_str(),
                        (size_t)(*iter),
                        this->allocation_locations[*iter].c_str()
                    );
                }
                #endif

                for (size_t i = 0; i < cpp_new_and_del_threshold_size / alignment; i ++)
                {
                    delete this->__chunks[i];
                }
            }

        private:
            Chunk* __chunks[cpp_new_and_del_threshold_size / alignment];

            MemoryPool()
            {
                for (size_t i = 0; i < cpp_new_and_del_threshold_size / alignment; i ++)
                {
                    this->__chunks[i] = new Chunk(i * alignment);
                }
            }

            static size_t __get_aligned_size(size_t size)
            {
                return size % alignment == 0 ? size : (size / alignment + 1) * alignment;
            }
        };
    }
}