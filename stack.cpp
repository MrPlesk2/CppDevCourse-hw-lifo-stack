#include "stack.hpp"

#include <cstddef>
#include <unordered_map>
#include <vector>
#include <stdexcept>
#include <queue>
#include <limits>

namespace stack
{

    struct Node {
        std::vector<char> data;
        Node* next;

        Node(const void* data_ptr, std::size_t data_size) 
            : data(static_cast<const char*>(data_ptr), 
                static_cast<const char*>(data_ptr) + data_size),
            next(nullptr) {
        }
    };

    struct Stack {
        Node* top;
        std::size_t count;
        
        Stack() : top(nullptr), count(0) {
        }
    };

    std::unordered_map<Handle, Stack> stacks;
    std::queue<Handle> available_handles;
    Handle next_new_handle = 0;

    Handle allocate_handle() {
        if (!available_handles.empty()) {
            Handle handle = available_handles.front();
            available_handles.pop();
            return handle;
        }
        
        if (next_new_handle < std::numeric_limits<Handle>::max()) {
            return next_new_handle++;
        }
        
        return -1;
    }
    
    void deallocate_handle(Handle handle) {
        if (handle != -1) {
            available_handles.push(handle);
        }
    }

    Handle create()
    {
        try {
            Handle handle = allocate_handle();
            if (handle == -1) {
                return -1;
            }
            
            stacks.emplace(handle, Stack());
            return handle;
        } catch (...) {
            return -1;
        }
    }

    void destroy(const Handle handle)
    {
        auto it = stacks.find(handle);
        if (it == stacks.end()) {
            return;
        }

        Stack& stack = it->second;
        Node* current = stack.top;
        while (current != nullptr) {
            Node* next = current->next;
            delete current;
            current = next;
        }

        stacks.erase(it);
        deallocate_handle(handle);
    }

    bool valid(const Handle handle)
    {
        return stacks.find(handle) != stacks.end();
    }

    std::size_t count(const Handle handle)
    {
        auto it = stacks.find(handle);
        if (it == stacks.end()) {
            return 0;
        }
        return it->second.count;
    }

    void push(const Handle handle, const void* const data, const std::size_t size)
    {
        if (data == nullptr || size == 0) {
            return ;
        }

        auto it = stacks.find(handle);
        if (it == stacks.end()) {
            return;
        }

        Stack& stack = it->second;

        try {
            Node* new_node = new Node(data, size);

            new_node->next = stack.top;
            stack.top = new_node;
            stack.count++;
 
        } catch (...) {
        }
    }

    std::size_t pop(const Handle handle, void* const data, const std::size_t size)
    {
        if (data == nullptr || size == 0) {
            return 0;
        }

        auto it = stacks.find(handle);
        if (it == stacks.end() || it->second.top == nullptr) {
            return 0;
        }

        Stack& stack = it->second;
        Node* top_node = stack.top;

        if (size < top_node->data.size()) {
            return 0;
        }

        std::size_t copy_size = std::min(size, top_node->data.size());
        const char* source_data = top_node->data.data();
        char* dest_data = static_cast<char*>(data);

        for (std::size_t i = 0; i < copy_size; ++i) {
            dest_data[i] = source_data[i];
        }

        stack.top = top_node->next;
        delete top_node;
        stack.count--;

        return copy_size;
    }
} // namespace stack
