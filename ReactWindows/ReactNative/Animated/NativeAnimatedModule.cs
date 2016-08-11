using Newtonsoft.Json.Linq;
using ReactNative.Bridge;
using ReactNative.Modules.Core;
using ReactNative.UIManager;
using System;
using System.Collections.Generic;
using Windows.UI.Xaml.Media;

namespace ReactNative.Animated
{
    /// <summary>
    /// Module that exposes interface for creating and managing animated nodes 
    /// on the "native" side.
    /// </summary>
    /// <remarks>
    /// Animated.js library is based on a concept of a graph where nodes are 
    /// values or transform operations (such as interpolation, addition, etc.)
    /// and connection are used to describe how change of the value in one node
    /// can affect other nodes.
    /// 
    /// Few examples of the nodes that can be created on the JS side:
    ///  - Animated.Value is a simplest type of node with a numeric value which
    ///    can be driven by an animation engine (spring, decay, etc) or by 
    ///    calling setValue on it directly from JS
    ///  - Animated.add is a type of node that may have two or more input nodes.
    ///    It outputs the sum of all the input node values
    ///  - interpolate - is actually a method you can call on any node and it 
    ///    creates a new node that takes the parent node as an input and 
    ///    outputs its interpolated value (e.g. if you have value that can
    ///    animate from 0 to 1 you can create interpolated node and set output
    ///    range to be 0 to 100 and when the input node changes the output of
    ///    interpolated node will multiply the values by 100)
    /// 
    /// You can mix and chain nodes however you like and this way create nodes
    /// graph with connections between them.
    /// 
    /// To map animated node values to view properties there is a special type
    /// of a node: AnimatedProps. It is created by AnimatedImplementation 
    /// whenever you render Animated.View and stores a mapping from the view
    /// properties to the corresponding animated values (so it's actually also
    /// a node with connections to the value nodes).
    /// 
    /// Last "special" elements of the the graph are "animation drivers". Those
    /// are objects (represented as a graph nodes too) that based on some
    /// criteria updates attached values every frame (we have few types of
    /// those, e.g., spring, timing, decay). Animation objects can be "started"
    /// and "stopped". Those are like "pulse generators" for the rest of the 
    /// nodes graph. Those pulses then propagate along the graph to the
    /// children nodes up to the special node type: AnimatedProps which then 
    /// can be used to calculate property update map for a view.
    /// 
    /// This class acts as a proxy between the "native" API that can be called
    /// from JS and the main class that coordinates all the action: 
    /// <see cref="NativeAnimatedNodesManager"/>. Since all the methods from
    /// <see cref="NativeAnimatedNodesManager"/> need to be called from the UI
    /// thread, we create a queue of animated graph operations that is then
    /// enqueued to be executed on the UI Thread at the end of the batch of 
    /// JS->native calls (similarily to how it's handled in 
    /// <see cref="UIManagerModule"/>). This isolates us from the problems that
    /// may be caused by concurrent updates of animated graph while UI thread 
    /// is "executing" the animation loop.
    /// </remarks>
    public class NativeAnimatedModule : ReactContextNativeModuleBase, IOnBatchCompleteListener, ILifecycleEventListener
    {
        private readonly object _operationsGate = new object();

        private EventHandler<object> _animatedFrameCallback;
        private List<Action<NativeAnimatedNodesManager>> _operations = 
            new List<Action<NativeAnimatedNodesManager>>();
        private List<Action<NativeAnimatedNodesManager>> _readyOperations;

        /// <summary>
        /// Instantiates the <see cref="NativeAnimatedModule"/>. 
        /// </summary>
        /// <param name="reactContext">The React context.</param>
        public NativeAnimatedModule(ReactContext reactContext)
            : base(reactContext)
        {
        }

        /// <summary>
        /// The name of the module.
        /// </summary>
        public override string Name
        {
            get
            {
                return "NativeAnimatedModule";
            }
        }

        /// <summary>
        /// Called after the creation of a <see cref="IReactInstance"/>, in
        /// order to initialize native modules that require the React or
        /// JavaScript modules.
        /// </summary>
        public override void Initialize()
        {
            var ctx = Context;
            var uiImpl = ctx.GetNativeModule<UIManagerModule>().UIImplementation;
            var nodesManager = new NativeAnimatedNodesManager(uiImpl);
            _animatedFrameCallback = (sender, args) =>
            {
                var renderingArgs = args as RenderingEventArgs;
                if (renderingArgs == null)
                {
                    return;
                }

                var operations = default(List<Action<NativeAnimatedNodesManager>>);
                lock (_operationsGate)
                {
                    operations = _readyOperations;
                    _readyOperations = null;
                }

                if (operations != null)
                {
                    foreach (var operation in operations)
                    {
                        operation(nodesManager);
                    }
                }

                if (nodesManager.HasActiveAnimations)
                {
                    nodesManager.RunUpdates(renderingArgs.RenderingTime);
                }
            };

            ctx.AddLifecycleEventListener(this);
        }

        /// <summary>
        /// Invoked when a batch of JavaScript to native calls has finished.
        /// </summary>
        public void OnBatchComplete()
        {
            var operations = _operations.Count == 0 ? null : _operations;
            _operations = new List<Action<NativeAnimatedNodesManager>>();
            if (operations != null)
            {
                lock (_operationsGate)
                {
                    if (_readyOperations == null)
                    {
                        _readyOperations = operations;
                    }
                    else
                    {
                        _readyOperations.AddRange(operations);
                    }
                }
            }
        }

        /// <summary>
        /// Called when the host is shutting down.
        /// </summary>
        public void OnDestroy()
        {
        }

        /// <summary>
        /// Called when the host receives the resume event.
        /// </summary>
        public void OnResume()
        {
            CompositionTarget.Rendering += _animatedFrameCallback;
        }

        /// <summary>
        /// Called when the host receives the suspend event.
        /// </summary>
        public void OnSuspend()
        {
            CompositionTarget.Rendering -= _animatedFrameCallback;
        }

        /// <summary>
        /// Creates an animated node.
        /// </summary>
        /// <param name="tag">Tag of the animated node.</param>
        /// <param name="config">Animation configuration.</param>
        [ReactMethod]
        public void createAnimatedNode(int tag, JObject config)
        {
            _operations.Add(manager =>
                manager.CreateAnimatedNode(tag, config));
        }

        /// <summary>
        /// Creates listener for animated values on the given node.
        /// </summary>
        /// <param name="tag">Tag of the animated node.</param>
        [ReactMethod]
        public void startListeningToAnimatedNodeValue(int tag)
        {
            _operations.Add(manager =>
                manager.StartListeningToAnimatedNodeValue(tag, value =>
                    Context.GetJavaScriptModule<RCTDeviceEventEmitter>()
                        .emit("onAnimatedValueUpdate", new JObject
                        {
                            { "tag", tag },
                            { "value", value }
                        })));
        }

        /// <summary>
        /// Removes listener for animated values on the given node.
        /// </summary>
        /// <param name="tag">Tag of the animated node.</param>
        [ReactMethod]
        public void stopListeningToAnimatedNodeValue(int tag)
        {
            _operations.Add(manager =>
                manager.StopListeningToAnimatedNodeValue(tag));
        }

        /// <summary>
        /// Drops animated node with the given tag.
        /// </summary>
        /// <param name="tag">Tag of the animated node.</param>
        [ReactMethod]
        public void dropAnimatedNode(int tag)
        {
            _operations.Add(manager =>
                manager.DropAnimatedNode(tag));
        }

        /// <summary>
        /// Sets the value of the animated node.
        /// </summary>
        /// <param name="tag">Tag of the animated node.</param>
        /// <param name="value">Animated node value.</param>
        [ReactMethod]
        public void setAnimatedNodeValue(int tag, double value)
        {
            _operations.Add(manager =>
                manager.SetAnimatedNodeValue(tag, value));
        }

        /// <summary>
        /// Starts an animation on the given node with the given identifier.
        /// </summary>
        /// <param name="animationId">Anmation identifier.</param>
        /// <param name="animatedNodeTag">Animated node tag.</param>
        /// <param name="animationConfig">Animation configuration.</param>
        /// <param name="endCallback">Callback for animation completion.</param>
        [ReactMethod]
        public void startAnimatingNode(
            int animationId,
            int animatedNodeTag,
            JObject animationConfig,
            ICallback endCallback)
        {
            _operations.Add(manager =>
                manager.StartAnimatingNode(
                    animationId,
                    animatedNodeTag,
                    animationConfig,
                    endCallback));
        }

        /// <summary>
        /// Stops the animation with the given identifier.
        /// </summary>
        /// <param name="animationId">Animation identifier.</param>
        [ReactMethod]
        public void stopAnimation(int animationId)
        {
            _operations.Add(manager =>
                manager.StopAnimation(animationId));
        }

        /// <summary>
        /// Connects animated nodes.
        /// </summary>
        /// <param name="parentNodeTag">Parent animated node tag.</param>
        /// <param name="childNodeTag">Child animated node tag.</param>
        [ReactMethod]
        public void connectAnimatedNodes(int parentNodeTag, int childNodeTag)
        {
            _operations.Add(manager =>
                manager.ConnectAnimatedNodes(parentNodeTag, childNodeTag));
        }

        /// <summary>
        /// Disconnects animated nodes.
        /// </summary>
        /// <param name="parentNodeTag">Parent animated node tag.</param>
        /// <param name="childNodeTag">Child animated node tag.</param>
        [ReactMethod]
        public void disconnectAnimatedNodes(int parentNodeTag, int childNodeTag)
        {
            _operations.Add(manager =>
                manager.DisconnectAnimatedNodes(parentNodeTag, childNodeTag));
        }

        /// <summary>
        /// Connects animated node to view.
        /// </summary>
        /// <param name="animatedNodeTag">Animated node tag.</param>
        /// <param name="viewTag">React view tag.s</param>
        [ReactMethod]
        public void connectAnimatedNodeToView(int animatedNodeTag, int viewTag)
        {
            _operations.Add(manager =>
                manager.ConnectAnimatedNodeToView(animatedNodeTag, viewTag));
        }

        /// <summary>
        /// Disconnects animated node from view.
        /// </summary>
        /// <param name="animatedNodeTag">Animated node tag.</param>
        /// <param name="viewTag">React view tag.s</param>
        [ReactMethod]
        public void disconnectAnimatedNodeFromView(int animatedNodeTag, int viewTag)
        {
            _operations.Add(manager =>
                manager.DisconnectAnimatedNodeFromView(animatedNodeTag, viewTag));
        }
    }
}
