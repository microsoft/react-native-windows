using Microsoft.ReactNative;

namespace <%=ns%>
{
    sealed partial class App : ReactApplication
{
    public App()
    {
        MainComponentName = "<%=name%>";

#if BUNDLE
        JavaScriptBundleFile = "index.windows";
        InstanceSettings.UseWebDebugger = false;
        InstanceSettings.UseFastRefresh = false;
#else
        JavaScriptMainModuleName = "index";
        InstanceSettings.UseWebDebugger = true;
        InstanceSettings.UseFastRefresh = true;
#endif

#if DEBUG
        InstanceSettings.EnableDeveloperMenu = true;
#else
        InstanceSettings.EnableDeveloperMenu = false;
#endif

#if USE_DIRECT_DEBUGGING
        InstanceSettings.UseWebDebugger = false;
        InstanceSettings.UseDirectDebugger = true;
        InstanceSettings.DebuggerBreakOnNextLine = true;
        InstanceSettings.DebuggerPort = 9229;
#endif

        PackageProviders.Add(new Microsoft.ReactNative.Managed.ReactPackageProvider()); // Includes any modules in this project

        InitializeComponent();
    }
}
}
