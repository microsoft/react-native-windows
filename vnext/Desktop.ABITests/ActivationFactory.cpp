
#include <Windows.h>
#include <winrt/base.h>

using namespace winrt;

extern "C"
{
	int32_t WINRT_CALL OS_RoGetActivationFactory(void* classId, guid const& iid, void** factory) noexcept;
}

#ifdef _M_IX86
#pragma comment(linker, "/alternatename:_OS_RoGetActivationFactory@12=_RoGetActivationFactory@12")
#else
#pragma comment(linker, "/alternatename:OS_RoGetActivationFactory=RoGetActivationFactory")
#endif

bool starts_with(std::wstring_view value, std::wstring_view match) noexcept
{
	return 0 == value.compare(0, match.size(), match);
}

int32_t WINRT_CALL WINRT_RoGetActivationFactory(void* classId, guid const& iid, void** factory) noexcept
{
	*factory = nullptr;
	std::wstring_view name{ WINRT_WindowsGetStringRawBuffer(classId, nullptr) };
	HMODULE library{ nullptr };

	if (starts_with(name, L"facebook.react."))
	{
		library = LoadLibraryExW(RNDLLPATH, NULL, LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR | LOAD_LIBRARY_SEARCH_DEFAULT_DIRS);
		auto le = GetLastError();
		le = le;
	}
	else
	{
		return OS_RoGetActivationFactory(classId, iid, factory);
	}

	if (!library)
	{
		return HRESULT_FROM_WIN32(GetLastError());
	}

	using DllGetActivationFactory = HRESULT __stdcall(void* classId, void** factory);
	auto call = reinterpret_cast<DllGetActivationFactory*>(GetProcAddress(library, "DllGetActivationFactory"));

	if (!call)
	{
		HRESULT const hr = HRESULT_FROM_WIN32(GetLastError());
		WINRT_VERIFY(FreeLibrary(library));
		return hr;
	}

	winrt::com_ptr<winrt::Windows::Foundation::IActivationFactory> activation_factory;
	HRESULT const hr = call(classId, activation_factory.put_void());

	if (FAILED(hr))
	{
		WINRT_VERIFY(FreeLibrary(library));
		return hr;
	}

	if (iid != winrt::guid_of<winrt::Windows::Foundation::IActivationFactory>())
	{
		return activation_factory->QueryInterface(iid, factory);
	}

	*factory = activation_factory.detach();
	return S_OK;
}
