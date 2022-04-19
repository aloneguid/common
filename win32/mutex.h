#pragma once
#include <string>

namespace win32
{
    class mutex
    {
    public:
        mutex(std::string name);
        ~mutex();

        bool is_owner;

    private:
        void* hMutex;
    };
}

