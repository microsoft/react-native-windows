using Newtonsoft.Json.Linq;
using ReactNative;
using ReactNative.Bridge;
using ReactNative.Touch;
using ReactNative.UIManager;
using ReactNative.Views.Text;
using System.Collections.Generic;
using System.Threading;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media;
using ReactNative.UIManager.Events;
using System;
using System.Threading.Tasks;

namespace NativeControlTestDriver
{
    public partial class MainWindow : Window, IEventDispatcherListener
    {
        #region Properties

        //private MockReactPage SourcePage { get; } = new MockReactPage();
        //private ReactPage SourcePage { get; } = new MockReactPage();

        #endregion

        //private UIManagerModule uiManagerModule;
        //private ReactRootView rootView;

        //private ReactContext reactContext = new ReactContext();

        //private UIImplementationProvider uiImplementationProvider = new UIImplementationProvider();
        //private UIImplementation uiImplementation;

        private static int nextTag = 0;
        //private int rootViewTag;

        public MainWindow()
        {
            InitializeComponent();

            DataContext = this;

            this.Loaded += MainWindow_Loaded;
        }

        private void MainWindow_Loaded(object sender, RoutedEventArgs e)
        {
            // this.SourcePage.Initialize();

            //var context = new ReactContext();
            //var viewManagers = new List<IViewManager>();


            //var ggg = (ViewParentManager<TextBlock, ReactTextShadowNode>) textViewManager;
            //var shadowNodeInstance = textViewManager.CreateShadowNodeInstance();

            //textViewManager = new ReactTextViewManager();

            //textBlock = textViewManager.CreateView(null, null);
            //textBlock.Background = new SolidColorBrush(Colors.Aqua);
            //viewManagers.Add(textViewManager);

            //var uiImplementation = new UIImplementation(context, viewManagers);

            //uiManagerModule = new UIManagerModule(context, viewManagers, uiImplementation, this);

            //var textBlock = new TextBlock();
            //textBlock.Width = 150;
            //textBlock.Height = 32;
            //textBlock.Text = "Hello";


            //rootView = new ReactRootView();
            //rootView.Children.Add(textBlock);
            //ReactContent.Content = rootView;

            //int tag = uiManagerModule.AddMeasuredRootView(rootView);

            //JObject props = new JObject();
            //props.Add("fontSize", 20);
            //props.Add("textAlign", "center");

            //uiManagerModule.createView(tag, @"RCTText", tag, props);

            //textViewManager = new ReactTextViewManager();

            //var viewManagers = new List<IViewManager>
            //{
            //    textViewManager
            //};

            //uiImplementation = uiImplementationProvider.Create(reactContext, viewManagers);
            //uiImplementation.OnResume();

            //rootViewTag = GetNextTag();

            //uiImplementation.RegisterRootView(this.RootView, rootViewTag, this.RootView.Width, this.RootView.Height, themedReactContext);

            this.SetupViewManagerTest();

        }

        private void OnRunTestButtonClicked(object sender, RoutedEventArgs e)
        {


            JObject props = new JObject();
            props.Add("fontSize", 20);
            props.Add("width", 150);
            //props.Add("height", 32);
            props.Add("textAlign", "center");

            ReactStylesDiffMap diffMap = new ReactStylesDiffMap(props);

            ReactTextShadowNode shadowNode = this.textViewManager.CreateShadowNodeInstance();

            shadowNode.UpdateProperties(diffMap);

            this.textViewManager.UpdateExtraData(textBlock, shadowNode);

            //this.textViewManager.UpdateProperties(textBlock, diffMap);

            //shadowNode.UpdateProperties(diffMap);

            ////shadowNode.SetWidth(550);
            ////shadowNode.SetHeight(35);
            ////shadowNode.SetTextAlign("right");
            //textViewManager.UpdateExtraData(textBlock, shadowNode);

            ////

            ////ReactTextShadowNode CreateShadowNode(JObject props)

        }



        private void OnRunTest2ButtonClicked(object sender, RoutedEventArgs e)
        {
            //JObject props = new JObject();
            //props.Add("fontSize", 20);
            ////jo.Add("color", "#FF00FF00");
            //props.Add("width", 150);
            //props.Add("height", 32);
            //props.Add("textAlign", "center");

            //int childTag = GetNextTag();
            //uiImplementation.CreateView(childTag, @"RCTText", rootViewTag, props);

            //int[] childrenTags = {childTag};

            ////uiImplementation.SetChildren(rootViewTag, childrenTags);
            //uiImplementation.ManageChildren(rootViewTag, null, null, childrenTags, null, null);
        }

        private int GetNextTag()
        {
            return Interlocked.Increment(ref nextTag);
        }

        #region ViewManager Tests

        private ReactContext textViewManagerReactContext;
        private IEventDispatcher textViewManagerEventDispatcher;
        private ReactTextViewManager textViewManager;
        private JavaScriptResponderHandler textViewManagerResponderHandler;
        private TextBlock textBlock;


        private void SetupViewManagerTest()
        {
            this.textViewManagerReactContext = new ReactContext
            {
                NativeModuleCallExceptionHandler = exception => { throw exception; }
            };




            this.textViewManagerEventDispatcher = new EventDispatcher(this.textViewManagerReactContext);

            this.textViewManagerEventDispatcher.AddListener(this);

            this.textViewManager = new ReactTextViewManager(this.textViewManagerEventDispatcher);




            this.textViewManagerResponderHandler = new JavaScriptResponderHandler();

            this.textBlock = this.textViewManager.CreateView(this.textViewManagerResponderHandler);

            if ( this.textBlock != null )
            {
                this.textBlock.Text = "Hello World";
                this.textBlock.Background = new SolidColorBrush(Colors.Fuchsia);
                this.textBlock.Width = 300;
                //this.textBlock.Tag = GetNextTag(); //Doesn't work

                Canvas.SetTop(this.textBlock, 100);
                Canvas.SetLeft(this.textBlock, 100);

                this.ReactCanvas.Children.Add(this.textBlock);

                //var ggg = (ViewParentManager<TextBlock, ReactTextShadowNode>) textViewManager;
                //var shadowNodeInstance = textViewManager.CreateShadowNodeInstance();
            }
        }

        public bool OnEventDispatch(Event @event)
        {
            //var eventArgs = @event as ReactTextViewSizeChangedEvent;

            //if (eventArgs != null)
            //{
            //    var sizeParams = new JObject
            //    {
            //        { "newWidth", eventArgs.NewSize.Width },
            //        { "newHeight", eventArgs.NewSize.Height },
            //        { "prevWidth", eventArgs.PrevSize.Width },
            //        { "prevHeight", eventArgs.PrevSize.Height },
            //        { "heightChanged", eventArgs.HeightChanged },
            //        { "widthChanged", eventArgs.WidthChanged },
            //    };

            //    var eventData = new JObject()
            //    {
            //        {"eventName", eventArgs.EventName},
            //        {"target", eventArgs.ViewTag},
            //        {"sizeParams", sizeParams},
            //    };

            //    this.EventDepot.Text = eventData.ToString();
            //}

            return true;
        }

        #endregion
    }
}
