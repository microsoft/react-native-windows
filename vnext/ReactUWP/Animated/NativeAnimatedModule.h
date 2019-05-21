// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
#include <ReactContextNativeModuleBase>

namespace react {
  namespace uwp {
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
  /// To map animated node values to view props there is a special type
  /// of a node: AnimatedProps. It is created by AnimatedImplementation 
  /// whenever you render Animated.View and stores a mapping from the view
  /// props to the corresponding animated values (so it's actually also
  /// a node with connections to the value nodes).
  /// 
  /// Last "special" elements of the the graph are "animation drivers". Those
  /// are objects (represented as a graph nodes too) that based on some
  /// criteria updates attached values every frame (we have few types of
  /// those, e.g., spring, timing, decay). Animation objects can be "started"
  /// and "stopped". Those are like "pulse generators" for the rest of the 
  /// nodes graph. Those pulses then propagate along the graph to the
  /// children nodes up to the special node type: AnimatedProps which then 
  /// can be used to calculate prop update map for a view.
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
  class NativeAnimatedModule : ReactContextNativeModuleBase, ILifecycleEventListener
  {
      private readonly object _operationsGate = new object();

      private EventHandler<FrameEventArgs> _animatedFrameCallback;

      private List<Action<NativeAnimatedNodesManager>> _operations =
          new List<Action<NativeAnimatedNodesManager>>();
      private List<Action<NativeAnimatedNodesManager>> _preOperations =
          new List<Action<NativeAnimatedNodesManager>>();

      private NativeAnimatedNodesManager _nodesManager;

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

      private NativeAnimatedNodesManager NodesManager
      {
          get
          {
              if (_nodesManager == null)
              {
                  var uiManager = Context.GetNativeModule<UIManagerModule>();
                  _nodesManager = new NativeAnimatedNodesManager(uiManager);
              }

              return _nodesManager;
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
                var uiManager = ctx.GetNativeModule<UIManagerModule>();
                uiManager.DispatchingViewUpdates += OnDispatchingViewUpdates;
                _animatedFrameCallback = (sender, args) = >
                {
                    try
                    {
                        var nodesManager = NodesManager;
                        if (nodesManager.HasActiveAnimations)
                        {
                            nodesManager.RunUpdates(args.RenderingTime);
                        }
                        else
                        {
                            ReactChoreographer.Instance.DeactivateCallback(nameof(NativeAnimatedModule));
                        }
                    }
                    catch (Exception ex)
                    {
                        Context.HandleException(ex);
                    }
                };

                ctx.AddLifecycleEventListener(this);
            }

            private void OnDispatchingViewUpdates(object sender, EventArgs e)
            {
                if (_operations.Count == 0 && _preOperations.Count == 0)
                {
                    return;
                }

                var uiManager = (UIManagerModule)sender;
                List<Action<NativeAnimatedNodesManager>> preOperations;
                List<Action<NativeAnimatedNodesManager>> operations;
                lock(_operationsGate)
                {
                    preOperations = _preOperations;
                    operations = _operations;
                    _preOperations = new List<Action<NativeAnimatedNodesManager>>();
                    _operations = new List<Action<NativeAnimatedNodesManager>>();
                }

                uiManager.PrependUIBlock(new UIBlock(() = >
                {
                    var nodesManager = NodesManager;
                    foreach(var operation in preOperations)
                    {
                        operation(nodesManager);
                    }
                }));

                uiManager.AddUIBlock(new UIBlock(() = >
                {
                    var nodesManager = NodesManager;
                    foreach(var operation in operations)
                    {
                        operation(nodesManager);
                    }
                }));

                ReactChoreographer.Instance.ActivateCallback(nameof(NativeAnimatedModule));
            }

            /// <summary>
            /// Called when the host is shutting down.
            /// </summary>
            public void OnDestroy()
            {
                ReactChoreographer.Instance.NativeAnimatedCallback -= _animatedFrameCallback;
            }

            /// <summary>
            /// Called when the host receives the resume event.
            /// </summary>
            public void OnResume()
            {
                ReactChoreographer.Instance.NativeAnimatedCallback += _animatedFrameCallback;
            }

            /// <summary>
            /// Called when the host receives the suspend event.
            /// </summary>
            public void OnSuspend()
            {
                ReactChoreographer.Instance.NativeAnimatedCallback -= _animatedFrameCallback;
            }

            /// <summary>
            /// Creates an animated node.
            /// </summary>
            /// <param name="tag">Tag of the animated node.</param>
            /// <param name="config">Animation configuration.</param>
            [ReactMethod]
            public void createAnimatedNode(int tag, JObject config)
            {
                AddOperation(manager = >
                    manager.CreateAnimatedNode(tag, config));
            }

            /// <summary>
            /// Creates listener for animated values on the given node.
            /// </summary>
            /// <param name="tag">Tag of the animated node.</param>
            [ReactMethod]
            public void startListeningToAnimatedNodeValue(int tag)
            {
                AddOperation(manager = >
                    manager.StartListeningToAnimatedNodeValue(tag, value = >
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
                AddOperation(manager = >
                    manager.StopListeningToAnimatedNodeValue(tag));
            }

            /// <summary>
            /// Drops animated node with the given tag.
            /// </summary>
            /// <param name="tag">Tag of the animated node.</param>
            [ReactMethod]
            public void dropAnimatedNode(int tag)
            {
                AddOperation(manager = >
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
                AddOperation(manager = >
                    manager.SetAnimatedNodeValue(tag, value));
            }

            /// <summary>
            /// Sets the offset of the animated node.
            /// </summary>
            /// <param name="tag">Tag of the animated node.</param>
            /// <param name="value">Animated node offset.</param>
            [ReactMethod]
            public void setAnimatedNodeOffset(int tag, double value)
            {
                AddOperation(manager = >
                    manager.SetAnimatedNodeOffset(tag, value));
            }

            /// <summary>
            /// Flattens the animated node offset.
            /// </summary>
            /// <param name="tag">Tag of the animated node.</param>
            [ReactMethod]
            public void flattenAnimatedNodeOffset(int tag)
            {
                AddOperation(manager = >
                    manager.FlattenAnimatedNodeOffset(tag));
            }

            /// <summary>
            /// Extracts the animated node offset.
            /// </summary>
            /// <param name="tag">Tag of the animated node.</param>
            [ReactMethod]
            public void extractAnimatedNodeOffset(int tag)
            {
                AddOperation(manager = >
                    manager.ExtractAnimatedNodeOffset(tag));
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
                AddOperation(manager = >
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
                AddOperation(manager = >
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
                AddOperation(manager = >
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
                AddOperation(manager = >
                    manager.DisconnectAnimatedNodes(parentNodeTag, childNodeTag));
            }

            /// <summary>
            /// Connects animated node to view.
            /// </summary>
            /// <param name="animatedNodeTag">Animated node tag.</param>
            /// <param name="viewTag">React view tag.</param>
            [ReactMethod]
            public void connectAnimatedNodeToView(int animatedNodeTag, int viewTag)
            {
                AddOperation(manager = >
                    manager.ConnectAnimatedNodeToView(animatedNodeTag, viewTag));
            }

            /// <summary>
            /// Disconnects animated node from view.
            /// </summary>
            /// <param name="animatedNodeTag">Animated node tag.</param>
            /// <param name="viewTag">React view tag.</param>
            [ReactMethod]
            public void disconnectAnimatedNodeFromView(int animatedNodeTag, int viewTag)
            {
                lock(_operationsGate)
                {
                    _preOperations.Add(manager = >
                        manager.RestoreDefaultValues(animatedNodeTag, viewTag));
                    _operations.Add(manager = >
                        manager.DisconnectAnimatedNodeFromView(animatedNodeTag, viewTag));
                }
            }

            /// <summary>
            /// Adds an animated event to view.
            /// </summary>
            /// <param name="viewTag">The view tag.</param>
            /// <param name="eventName">The event name.</param>
            /// <param name="eventMapping">The event mapping.</param>
            [ReactMethod]
            public void addAnimatedEventToView(int viewTag, string eventName, JObject eventMapping)
            {
                AddOperation(manager = >
                    manager.AddAnimatedEventToView(viewTag, eventName, eventMapping));
            }

            /// <summary>
            /// Removes an animated event from view.
            /// </summary>
            /// <param name="viewTag">The view tag.</param>
            /// <param name="eventName">The event name.</param>
            /// <param name="animatedValueTag">The value tag.</param>
            [ReactMethod]
            public void removeAnimatedEventFromView(int viewTag, string eventName, int animatedValueTag)
            {
                AddOperation(manager = >
                    manager.RemoveAnimatedEventFromView(viewTag, eventName, animatedValueTag));
            }

            private void AddOperation(Action<NativeAnimatedNodesManager> action)
            {
                lock(_operationsGate)
                {
                    _operations.Add(action);
                }
            }

            private void AddPreOperation(Action<NativeAnimatedNodesManager> action)
            {
                lock(_operationsGate)
                {
                    _preOperations.Add(action);
                }
            }

            class UIBlock : IUIBlock
            {
                private readonly Action _action;

                public UIBlock(Action action)
                {
                    _action = action;
                }

                public void Execute(NativeViewHierarchyManager nativeViewHierarchyManager)
                {
                    _action();
                }
            }
        }
}
