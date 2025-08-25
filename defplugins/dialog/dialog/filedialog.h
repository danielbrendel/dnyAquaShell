#pragma once

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <Windows.h>
#include <string>
#include <shobjidl.h> 

namespace FileDialog {
    std::wstring OpenDialog(DWORD options)
    {
        //Show a FileOpenDialog

        std::wstring wszResult = L"";

        //Initialize COM
        HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
        if (SUCCEEDED(hr)) {
            IFileOpenDialog* pFileOpen;

            //Create FileOpenDialog object
            hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_IFileOpenDialog, (LPVOID*)&pFileOpen);

            if (SUCCEEDED(hr)) {
                //Adjust options
                DWORD dwOptions;
                pFileOpen->GetOptions(&dwOptions);
                pFileOpen->SetOptions(dwOptions | options);

                //Show the dialog box
                hr = pFileOpen->Show(NULL);

                //Get the file name from the dialog box
                if (SUCCEEDED(hr)) {
                    IShellItem* pItem;

                    hr = pFileOpen->GetResult(&pItem);
                    if (SUCCEEDED(hr)) {
                        PWSTR pwszFilePath;
                        hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pwszFilePath);

                        //Save as result
                        if (SUCCEEDED(hr)) {
                            wszResult = pwszFilePath;

                            CoTaskMemFree(pwszFilePath);
                        }

                        pItem->Release();
                    }
                }

                pFileOpen->Release();
            }

            CoUninitialize();
        }

        return wszResult;
    }

    std::wstring SaveDialog(DWORD options, const std::wstring& wszDefaultFileName, const std::vector<std::wstring>& vFileSpec)
    {
        //Show a FileSaveDialog

        std::wstring wszResult = L"";

        //Initialize COM
        HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
        if (SUCCEEDED(hr)) {
            IFileSaveDialog* pFileSave;

            //Create FileSaveDialog object
            hr = CoCreateInstance(CLSID_FileSaveDialog, NULL, CLSCTX_ALL, IID_IFileSaveDialog, (LPVOID*)&pFileSave);

            if (SUCCEEDED(hr)) {
                //Adjust options
                DWORD dwOptions;
                pFileSave->GetOptions(&dwOptions);
                pFileSave->SetOptions(dwOptions | options);

                //Suggest a default file name
                pFileSave->SetFileName(wszDefaultFileName.c_str());

                //Define allowed file types

                COMDLG_FILTERSPEC* rgSpec = new COMDLG_FILTERSPEC[vFileSpec.size()];

                for (size_t i = 0; i < vFileSpec.size(); i++) {
                    rgSpec[i].pszName = vFileSpec[i].c_str();
                    rgSpec[i].pszSpec = vFileSpec[i].c_str();
                }

                pFileSave->SetFileTypes(vFileSpec.size(), rgSpec);

                //Show the dialog
                hr = pFileSave->Show(NULL);
                if (SUCCEEDED(hr)) {
                    IShellItem* pItem;

                    hr = pFileSave->GetResult(&pItem);
                    if (SUCCEEDED(hr)) {
                        PWSTR pwszFilePath = nullptr;

                        hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pwszFilePath);
                        if (SUCCEEDED(hr)) {
                            wszResult = pwszFilePath;

                            CoTaskMemFree(pwszFilePath);
                        }

                        pItem->Release();
                    }
                }

                pFileSave->Release();

                delete[] rgSpec;
            }

            CoUninitialize();
        }

        return wszResult;
    }
}