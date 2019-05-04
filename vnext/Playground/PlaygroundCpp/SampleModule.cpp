#include "pch.h"
#include "SampleModule.h"

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::PlaygroundCpp::implementation
{
	SampleModule::SampleModule()
	{
		_constants.Insert(L"a", L"\"b\"");

		m_delegates.Insert(
			L"method1",
			MethodDelegate([](hstring para) {

				})
		);

		m_delegateWithCallback.Insert(
			L"method2",
			MethodWithCallbackDelegate([](hstring para, MethodCallback callback)
				{
					std::vector<hstring> results = { L"{ \"result\":true }" };
					callback(results);
				})
		);
	}

	IMapView<hstring, MethodWithCallbackDelegate> SampleModule::MethodsWithCallback()
	{
		return m_delegateWithCallback.GetView();
	}

	IMapView<hstring, MethodDelegate> SampleModule::Methods()
	{
		return m_delegates.GetView();
	}


	IMapView<hstring, hstring> SampleModule::Constants()
	{
		return _constants.GetView();
	}

	hstring SampleModule::Name()
	{
		return L"SampleModule";
	}
}
