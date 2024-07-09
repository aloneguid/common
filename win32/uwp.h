#pragma once
#include <string>

namespace win32 {
    class uwp {
    public:
        uwp();
        ~uwp();

        bool start_app(const std::wstring& app_user_mode_id, const std::wstring& arg);

    private:
        bool initialised;
    };
}