#pragma once
#include <unordered_set>
#include <stddef.h>
#include <type_traits>
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
        #define XyA_Allocate_(T) MemoryPool::get_instance().allocate<T>()
        #define XyA_Allocate(T, args) MemoryPool::get_instance().allocate<T>(args)
        #define XyA_Deallocate(object) MemoryPool::get_instance().deallocate(object)
        #endif

        #ifdef Debug_Display_Memory_Leaks
        #define STRINGIFY(x) #x
        #define TOSTRING(x) STRINGIFY(x)
        #define FILE_LINE std::string(__FILE__) + ": " + std::string(TOSTRING(__LINE__))
        #define XyA_Allocate_(T) MemoryPool::get_instance().allocate_<T>(FILE_LINE)
        #define XyA_Allocate(T, args) MemoryPool::get_instance().allocate_<T>(FILE_LINE, args)
        #define XyA_Deallocate(object) MemoryPool::get_instance().deallocate_(FILE_LINE, object)
        #endif

        #define XyA_Check_If_Deallocated(object) MemoryPool::get_instance().deallocated(object)

        // TODO: 实现内存池
        class MemoryPool
        {
        public:
            std::unordered_set<void*> allocated_objects;

            #ifdef Debug_Display_Memory_Leaks
            // std::vector<void*> allocated_objects;
            std::map<void*, std::string> allocation_locations;
            std::map<void*, std::string> deallocation_locations;
            #endif

            static MemoryPool& get_instance()
            {
                static MemoryPool instance;
                return instance; 
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
            }

            #ifndef Debug_Display_Memory_Leaks
            template <typename T, typename... Args>
            T *allocate(Args &&...args)
            {
                static_assert(std::is_base_of<Object, T>::value);

                return new T(std::forward<Args>(args)...);
            }
            #endif

            #ifdef Debug_Display_Memory_Leaks
            template <typename T, typename... Args>
            T *allocate_(std::string location, Args &&...args)
            {
                static_assert(std::is_base_of<Object, T>::value);

                T* obj = new T(std::forward<Args>(args)...);

                this->allocated_objects.insert(obj);
                this->allocation_locations[obj] = location;

                return obj;
            }
            #endif

            #ifndef Debug_Display_Memory_Leaks
            template <typename T>
            void deallocate(T *object)
            {
                delete object;
            }
            #endif

            #ifdef Debug_Display_Memory_Leaks
            template <typename T>
            void deallocate_(std::string location, T *object)
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

                delete object;
                this->deallocation_locations[object] = location;
            }
            #endif

            bool deallocated(void* object) const
            {
                auto iter = this->allocated_objects.find(object);
                return iter == this->allocated_objects.end();
            }
        };
    }
}