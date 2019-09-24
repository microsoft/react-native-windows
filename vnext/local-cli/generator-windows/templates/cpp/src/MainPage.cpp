#include "pch.h"
#include "MainPage.h"
#include "MainPage.g.cpp"

using namespace winrt;
using namespace Windows::UI::Xaml;
using namespace react::uwp;

namespace winrt::templatecpp::implementation
{
	const wchar_t* JSFILENAME = L"index.windows";
	const wchar_t* JSCOMPONENTNAME = L"templatecpp";

    MainPage::MainPage()
    {
        InitializeComponent();
		LoadReact();
    }

	void MainPage::LoadReact()
	{
		InstanceSettings settings;

		settings.UseLiveReload = true;
		settings.UseWebDebugger = true;

		auto instance = Instance::Create(winrt::hstring(JSFILENAME));
		instance.Start(settings);

		RootElement().Instance(instance);

		const wchar_t* initialProps = L"{ "
			L"\"one\":\"1\""
			L", \"two\":\"2\""
			L"}";

		RootElement().InitialProps(winrt::hstring(initialProps));

		RootElement().JsComponentName(JSCOMPONENTNAME);
		RootElement().StartRender();
	}
}
