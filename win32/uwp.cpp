#include "uwp.h"
#include <windows.h>
#include <wrl.h>
#include <windows.foundation.h>
#include <windows.management.deployment.h>
#include <shobjidl.h>  // For IApplicationActivationManager

#pragma comment(lib, "WindowsApp.lib")

using namespace Microsoft::WRL;
using namespace ABI::Windows::Foundation;
using namespace ABI::Windows::Management::Deployment;

namespace win32 {


    uwp::uwp() {
        // Initialize COM
        HRESULT hr = ::CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
        initialised = SUCCEEDED(hr);
    }

    uwp::~uwp() {
        if(initialised) {
            ::CoUninitialize();
        }
    }

    bool uwp::start_app(const std::wstring& app_user_mode_id, const std::wstring& arg) {

        // Create an instance of IApplicationActivationManager
        IApplicationActivationManager* appActivationManager = nullptr;
        HRESULT hr = CoCreateInstance(CLSID_ApplicationActivationManager, nullptr, CLSCTX_LOCAL_SERVER, IID_PPV_ARGS(&appActivationManager));
        if(FAILED(hr)) {
            return false;
        }

        // Activate the UWP application
        DWORD processId;
        hr = appActivationManager->ActivateApplication(app_user_mode_id.c_str(), nullptr, AO_NONE, &processId);
        appActivationManager->Release();

        return SUCCEEDED(hr);
    }

}
