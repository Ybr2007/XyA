#pragma once
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <list>
#include <unordered_set>
#include <utility>
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
        #define FILE_LINE std::string(__FILE__) + ": " + std::string(TOSTRING(__LINE__))
        #define XyA_Allocate_(T) MemoryPool::get_instance()->allocate_<T>(FILE_LINE)
        #define XyA_Allocate(T, args) MemoryPool::get_instance()->allocate_<T>(FILE_LINE, args)
        #define XyA_Deallocate(object) MemoryPool::get_instance()->deallocate_(FILE_LINE, object)
        #endif

        class MemoryPool
        {
        public:
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
                static_assert(std::constructible_from<T, Args...>, "Can not construct object from given arguments");

                unsigned short aligned_size = this->__get_aligned_size(sizeof(T));

                if (aligned_size > 512)
                {
                    return new T(std::forward<Args>(args)...);
                }

                unsigned char chunk_index = aligned_size / 8 - 1;
                Chunk* chunk = this->__chunks[chunk_index];

                if (chunk->free_block_head == nullptr)
                {
                    chunk->new_blocks();
                }

                Block* block = chunk->free_block_head;
                chunk->free_block_head = block->next_free;

                T* object = new(block)T(std::forward<Args>(args)...);
                return object;
            }
            #endif

            #ifdef Debug_Display_Memory_Leaks
            template <typename T, typename... Args>
            T* allocate_(std::string location, Args&&... args)
            {
                static_assert(std::constructible_from<T, Args...>, "Can not construct object from given arguments");

                unsigned short aligned_size = this->__get_aligned_size(sizeof(T));

                if (aligned_size > 512)
                {
                    return new T(std::forward<Args>(args)...);
                }

                unsigned char chunk_index = aligned_size / 8 - 1;
                Chunk* chunk = this->__chunks[chunk_index];

                if (chunk->free_block_head == nullptr)
                {
                    chunk->new_blocks();
                }

                Block* block = chunk->free_block_head;
                chunk->free_block_head = block->next_free;

                T* object = new(block)T(std::forward<Args>(args)...);
                this->allocated_objects.insert(object);
                this->allocation_locations[object] = location;
                return object;
            }
            #endif

            #ifndef Debug_Display_Memory_Leaks
            template <typename T>
            void deallocate(T* object)
            {
                object->~T();

                unsigned short aligned_size = this->__get_aligned_size(sizeof(T));

                if (aligned_size > 512)
                {
                    return delete object;
                }

                unsigned char chunk_index = aligned_size / 8 - 1;
                Chunk* chunk = this->__chunks[chunk_index];

                Block* block = reinterpret_cast<Block*>(object);
                block->next_free = chunk->free_block_head;
                chunk->free_block_head = block;
            }
            #endif

            #ifdef Debug_Display_Memory_Leaks
            template <typename T>
            void deallocate_(std::string location, T* object)
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
                    printf("!OK\n");
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

                object->~T();

                unsigned short aligned_size = this->__get_aligned_size(sizeof(T));

                if (aligned_size > 512)
                {
                    return delete object;
                }
                
                unsigned char chunk_index = aligned_size / 8 - 1;
                Chunk* chunk = this->__chunks[chunk_index];

                Block* block = reinterpret_cast<Block*>(object);
                block->next_free = chunk->free_block_head;
                chunk->free_block_head = block;

                this->deallocation_locations[object] = location;
            }
            #endif
        
        
        private:
            struct Block
            {
                Block* next_free;
            };

            class Chunk
            {
            public:
                Block* free_block_head = nullptr;
                std::unordered_set<char*> allocated_memories;  
                unsigned short size;

                Chunk(unsigned short size)
                {
                    this->size = size;
                }

                ~Chunk()
                {     
                    for (const auto& memory : this->allocated_memories)
                    {
                        free(memory);
                    }
                }

                void new_blocks()
                {
                    char* data = (char*)malloc(this->size * (4096 / this->size));
                    this->allocated_memories.insert(data);
                    for (unsigned int i = 0; i < 4096 / this->size; i ++)
                    {
                        Block* block = (Block*)(data + i * this->size);
                        block->next_free = this->free_block_head;
                        this->free_block_head = block;
                    }
                }
            };

            Chunk* __chunks[64];

            MemoryPool()
            {
                for (unsigned char i = 1; i <= 64; i ++)
                {
                    this->__chunks[i - 1] = new Chunk(i * 8);
                }   
            }

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

                for (unsigned char i = 1; i <= 64; i ++)
                {
                    delete this->__chunks[i - 1];
                } 
            }

            unsigned short __get_aligned_size(unsigned short object_size) const
            {
                return object_size % 8 == 0 ? object_size : (object_size / 8 + 1) * 8;
            }
        };
    }
}