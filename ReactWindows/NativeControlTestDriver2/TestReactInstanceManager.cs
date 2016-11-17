using ReactNative;
using ReactNative.Bridge;
using ReactNative.UIManager;
using ReactNative.Views.Text;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Threading;

namespace NativeControlTestDriver
{
    //public class TestReactInstanceManager
    //{
    //    private readonly UIImplementationProvider _uiImplementationProvider;
    //    private readonly Action<Exception> _nativeModuleCallExceptionHandler;

    //    private LifecycleState _lifecycleState = LifecycleState.BeforeResume;
    //    private ReactContext _currentReactContext;

    //    public TestReactInstanceManager()
    //    {
    //        this._uiImplementationProvider = new UIImplementationProvider();

    //        this._nativeModuleCallExceptionHandler = exception => { throw exception; };
    //    }

    //    public async Task InitializeReactContextAsync()
    //    {
    //        var currentReactContext = _currentReactContext;
    //        if (currentReactContext != null)
    //        {
    //            await TearDownReactContextAsync(currentReactContext);
    //            _currentReactContext = null;
    //        }

    //        try
    //        {
    //            var reactContext = await CreateReactContextAsync();
    //            SetupReactContext(reactContext);
    //        }
    //        catch (Exception ex)
    //        {
    //            throw;
    //        }
    //    }

    //    private async Task<ReactContext> CreateReactContextAsync()
    //    {
    //        var nativeRegistryBuilder = new NativeModuleRegistry.Builder();

    //        var reactContext = new ReactContext();

    //        var nativeModuleRegistry = nativeRegistryBuilder.Build();

    //        var viewManagers = new List<IViewManager>
    //        {
    //            new ReactTextViewManager()
    //        };

    //        var uiManagerModule =
    //    }

    //    private void SetupReactContext(ReactContext reactContext)
    //    {
    //        //DispatcherHelpers.AssertOnDispatcher();
    //        if (_currentReactContext != null)
    //        {
    //            throw new InvalidOperationException("React context has already been setup and has not been destroyed.");
    //        }

    //        _currentReactContext = reactContext;

    //        if (_lifecycleState == LifecycleState.Resumed)
    //        {
    //            _currentReactContext.OnResume();
    //        }
    //    }

    //    private async Task TearDownReactContextAsync(ReactContext reactContext)
    //    {
    //        if (!Dispatcher.CurrentDispatcher.CheckAccess())
    //        {
    //            throw new InvalidOperationException("Not on dispatcher");
    //        }

    //        if (_lifecycleState == LifecycleState.Resumed)
    //        {
    //            reactContext.OnSuspend();
    //        }

    //        //foreach (var rootView in _attachedRootViews)
    //        //{
    //        //    DetachViewFromInstance(rootView, reactContext.ReactInstance);
    //        //}

    //        await reactContext.DisposeAsync();
    //    }
    //}
}
