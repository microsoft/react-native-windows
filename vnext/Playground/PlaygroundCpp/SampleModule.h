// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

using namespace winrt;
using namespace winrt::react::uwp;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Foundation::Collections;

namespace winrt::PlaygroundCpp::implementation
{
	struct SampleModule : winrt::implements<SampleModule, IModule>
	{
	public:
		SampleModule();

		IMapView<hstring, MethodWithCallbackDelegate> MethodsWithCallback();
		IMapView<hstring, MethodDelegate> Methods();
		IMapView<hstring, hstring> Constants();
		hstring Name();

	private:
		IMap<hstring, hstring> m_constants = winrt::single_threaded_map<hstring, hstring>();
		IMap<hstring, MethodDelegate> m_delegates = winrt::single_threaded_map<hstring, MethodDelegate>();
		IMap<hstring, MethodWithCallbackDelegate> m_delegateWithCallback = winrt::single_threaded_map<hstring, MethodWithCallbackDelegate>();
	};
}