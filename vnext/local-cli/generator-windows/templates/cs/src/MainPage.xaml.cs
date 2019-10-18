using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;
using react.uwp;

// The Blank Page item template is documented at https://go.microsoft.com/fwlink/?LinkId=402352&clcid=0x409

namespace <%=ns%>
{
  /// <summary>
  /// An empty page that can be used on its own or navigated to within a Frame.
  /// </summary>
  public sealed partial class MainPage : Page
  {
#if BUNDLE    
    const string JSFILENAME = "index.windows";
#else    
    const string JSFILENAME = "index";
#endif

    const string JSCOMPONENTNAME = "<%=name%>";

    public MainPage()
    {
      this.InitializeComponent();
      LoadReact();
    }

    private void LoadReact()
    {
      InstanceSettings settings;

#if BUNDLE      
      settings.UseWebDebugger = false;
      settings.UseLiveReload = false;
#else      
      settings.UseWebDebugger = true;
      settings.UseLiveReload = true;
#endif      

#if NDEBUG
      settings.EnableDeveloperMenu = false;
#else
      settings.EnableDeveloperMenu = true;
#endif        

      var instance = Instance.Create(JSFILENAME);

      //instantiate sample module for registering callbacks for live reload, JS error handling etc.,
      instance.RegisterModule(new SampleModule());
      instance.Start(settings);

      RootElement.Instance = instance;

      //Setup sample initial properties for initializing UI thread
      string initialProps = "{ "
          + "\"one\":\"1\""
          + ", \"two\":\"2\""
          + "}";

      RootElement.InitialProps = initialProps;

      RootElement.JsComponentName = JSCOMPONENTNAME;

      RootElement.StartRender();
    }
  }

  public class SampleModule : react.uwp.IModule
  {
    Dictionary<string, string> _constants;
    Dictionary<string, MethodDelegate> m_delegates = new Dictionary<string, MethodDelegate>();
    Dictionary<String, MethodWithCallbackDelegate> m_delegateWithCallback = new Dictionary<string, MethodWithCallbackDelegate>();

    public SampleModule()
    {
      _constants = new Dictionary<string, string>();
      _constants.Add("a", "\"b\"");

      m_delegates.Add("method1", new MethodDelegate(
        (string para) =>
        {

        }));

      m_delegateWithCallback.Add("method2", new MethodWithCallbackDelegate(
        (string para, MethodCallback callback) =>
        {
          string[] results = new string[] { "{ \"result\":true }" };
          callback(results);
        }));
    }

    public IReadOnlyDictionary<string, string> Constants
    {
      get
      {
        return _constants;
      }
    }

    public IReadOnlyDictionary<string, MethodDelegate> Methods
    {
      get
      {
        return m_delegates;
      }
    }

    public IReadOnlyDictionary<string, MethodWithCallbackDelegate> MethodsWithCallback
    {
      get
      {
        return m_delegateWithCallback;
      }
    }

    public string Name
    {
      get
      {
        return "SampleModule";
      }
    }
  }
}

