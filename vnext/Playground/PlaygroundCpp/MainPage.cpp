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
		instance.RegisterModule(module.as<IModule>());
		instance.Start(settings);

		RootElement().Instance(instance);

		// Setup sample properties for initializing UI thread
		hstring initialProps = L"{ " \
			"\"one\":\"1\"" \
			", \"two\":\"2\"" \
			"}";

		RootElement().InitialProps(initialProps);

		RootElement().JsComponentName(JSCOMPONENTNAME);

		RootElement().StartRender();
	}
}
