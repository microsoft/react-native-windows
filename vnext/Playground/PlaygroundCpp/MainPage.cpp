// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include "MainPage.h"
#include "MainPage.g.cpp"
#include "winrt\react.uwp.h"
#include "SampleModule.h"

using namespace winrt;
using namespace react::uwp;
using namespace Windows::UI;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Media;

namespace winrt::PlaygroundCpp::implementation
{
	static constexpr auto JSFILENAME = L"App.windows";
	static constexpr auto JSCOMPONENTNAME = L"Playground";

    MainPage::MainPage()
    {
        InitializeComponent();

		LoadReact();
    }

	void MainPage::LoadReact()
	{
		InstanceSettings settings;
		settings.UseWebDebugger = true;
		settings.UseLiveReload = true;

		auto instance = Instance::Create(JSFILENAME);

		// instantiate sample module for registering callbacks for live reload, JS error handling etc.
		auto module = winrt::make<SampleModule>();
		instance.RegisterModule(module);
		instance.Start(settings);

		auto rootElement = RootElement();
		rootElement.Instance(instance);

		// Setup sample properties for initializing UI thread
		hstring initialProps = L"{ " \
			"\"one\":\"1\"" \
			", \"two\":\"2\"" \
			"}";

		rootElement.InitialProps(initialProps);

		rootElement.JsComponentName(JSCOMPONENTNAME);

		rootElement.StartRender();
	}
}
