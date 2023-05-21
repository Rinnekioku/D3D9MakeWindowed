#include <windows.h>
#include <d3d9.h>
#include <detours.h>

#pragma comment(lib, "d3d9.lib")

HRESULT __stdcall hookedEndScene(IDirect3DDevice9 *d3DDevicePtr) {
    D3DPRESENT_PARAMETERS d3DPresentParams;
    d3DPresentParams.Windowed = true;
    d3DDevicePtr->Reset(&d3DPresentParams);

    return S_OK;
}

void hookEndScene() {
    IDirect3D9 *d3DPtr = Direct3DCreate9(D3D_SDK_VERSION);

    if (!d3DPtr)
        return;

    D3DPRESENT_PARAMETERS d3DPresentParams;
    d3DPresentParams.hDeviceWindow = GetForegroundWindow();
    d3DPresentParams.Windowed = false;

    IDirect3DDevice9 *d3DDevicePtr = nullptr;

    if (FAILED(d3DPtr->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, d3DPresentParams.hDeviceWindow,
                                    D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3DPresentParams, &d3DDevicePtr)
        ) || !d3DDevicePtr) {
        d3DPtr->Release();
        return;
    }

    void **vTable = *reinterpret_cast<void ***>(d3DDevicePtr);

    DetourAttach((PVOID *) vTable[42], (PVOID) hookedEndScene);
    d3DDevicePtr->Release();
    d3DPtr->Release();

}

DWORD WINAPI MakeWindowed(LPVOID lpParam) {
    Sleep(100);
    hookEndScene();

    return 0;
}

BOOL WINAPI DllMain(
        HINSTANCE hinstDLL,
        DWORD fdwReason,
        LPVOID lpvReserved) {
    HANDLE makeWindowedThread = CreateThread(
            nullptr,
            0,
            MakeWindowed,
            nullptr,
            0,
            nullptr);
    if (!makeWindowedThread) {
        return FALSE;
    }

    switch (fdwReason) {
        case DLL_PROCESS_ATTACH: {
            WaitForSingleObject(makeWindowedThread, INFINITE);

            break;
        }

        case DLL_THREAD_ATTACH: {
            break;
        }

        case DLL_THREAD_DETACH: {
            break;
        }

        case DLL_PROCESS_DETACH: {
            if (lpvReserved != nullptr) {
                break;
            }
            CloseHandle(makeWindowedThread);

            break;
        }
    }

    return TRUE;
}
