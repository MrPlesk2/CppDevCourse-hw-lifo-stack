#include "stack.hpp"

#include <cstddef>
#include <unordered_map>
#include <vector>
#include <stdexcept>

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
    Handle next_handle = 0;

    Handle create()
    {
        try {
            Handle handle = next_handle++;
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
        auto it = stacks.find(handle);
        if (it == stacks.end() || it->second.top == nullptr) {
            return 0;
        }

        Stack& stack = it->second;
        Node* top_node = stack.top;

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
}
