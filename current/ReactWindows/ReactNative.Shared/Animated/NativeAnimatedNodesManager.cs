// Copyright (c) Microsoft Corporation. All rights reserved.
// Portions derived from React Native:
// Copyright (c) 2015-present, Facebook, Inc.
// Licensed under the MIT License.

using Newtonsoft.Json.Linq;
using ReactNative.Bridge;
using ReactNative.UIManager;
using ReactNative.UIManager.Events;
using System;
using System.Collections.Generic;
using static System.FormattableString;

namespace ReactNative.Animated
{
    /// <summary>
    /// This is the main class that coordinates how native animated JS
    /// implementation drives UI changes.
    /// 
    /// It implements a management interface for animated nodes graph as well
    /// as implements a graph traversal algorithm that is run for each
    /// animation frame.
    /// 
    /// For each animation frame we visit animated nodes that might've been 
    /// updated as well as their children that may use parent's values to
    /// update themselves. At the end of the traversal algorithm we expect to
    /// reach a special type of the node: PropsAnimatedNode that is then 
    /// responsible for calculating prop map which can be sent to native
    /// view hierarchy to update the view.
    /// </summary>
    /// <remarks>
    /// This class should be accessed only from the dispatcher thread.
    /// </remarks>
    class NativeAnimatedNodesManager : IEventDispatcherListener
    {
        private readonly IDictionary<int, AnimatedNode> _animatedNodes = new Dictionary<int, AnimatedNode>();
        private readonly IDictionary<int, AnimationDriver> _activeAnimations = new Dictionary<int, AnimationDriver>();
        private readonly IDictionary<int, AnimatedNode> _updatedNodes = new Dictionary<int, AnimatedNode>();
        // Mapping of a view tag and an event name to a list of event animation drivers. 99% of the time
        // there will be only one driver per mapping so all code should be optimized around that.
        private readonly IDictionary<Tuple<int, string>, IList<EventAnimationDriver>> _eventDrivers =
            new Dictionary<Tuple<int, string>, IList<EventAnimationDriver>>();
        private readonly Func<string, string> _customEventNamesResolver;
        private readonly UIImplementation _uiImplementation;
        // Used to avoid allocating a new array on every frame in `RunUpdates` and `OnEventDispatch`
        private readonly List<AnimatedNode> _runUpdateNodeList = new List<AnimatedNode>();
        // Used to avoid allocating a new array on every frame in `RunUpdates` and `StopAnimationsForNode`
        private int[] _activeAnimationIds;

        private int _animatedGraphBFSColor = 0;

        public NativeAnimatedNodesManager(UIManagerModule uiManager)
        {
            _uiImplementation = uiManager.UIImplementation;
            uiManager.EventDispatcher.AddListener(this);
            _customEventNamesResolver = uiManager.ResolveCustomEventName;
        }

        public bool HasActiveAnimations
        {
            get
            {
                return _activeAnimations.Count > 0 || _updatedNodes.Count > 0;
            }
        }

        public void CreateAnimatedNode(int tag, JObject config)
        {
            if (_animatedNodes.ContainsKey(tag))
            {
                throw new InvalidOperationException(
                    Invariant($"Animated node with tag '{tag}' already exists."));
            }

            var type = config.Value<string>("type");
            var node = default(AnimatedNode);
            switch (type)
            {
                case "style":
                    node = new StyleAnimatedNode(tag, config, this);
                    break;
                case "value":
                    node = new ValueAnimatedNode(tag, config);
                    break;
                case "props":
                    node = new PropsAnimatedNode(tag, config, this, _uiImplementation);
                    break;
                case "interpolation":
                    node = new InterpolationAnimatedNode(tag, config);
                    break;
                case "addition":
                    node = new AdditionAnimatedNode(tag, config, this);
                    break;
                case "subtraction":
                    node = new SubtractionAnimatedNode(tag, config, this);
                    break;
                case "division":
                    node = new DivisionAnimatedNode(tag, config, this);
                    break;
                case "multiplication":
                    node = new MultiplicationAnimatedNode(tag, config, this);
                    break;
                case "modulus":
                    node = new ModulusAnimatedNode(tag, config, this);
                    break;
                case "diffclamp":
                    node = new DiffClampAnimatedNode(tag, config, this);
                    break;
                case "transform":
                    node = new TransformAnimatedNode(tag, config, this);
                    break;
                case "tracking":
                    node = new TrackingAnimatedNode(tag, config, this);
                    break;
                default:
                    throw new InvalidOperationException(Invariant($"Unsupported node type: '{type}'"));
            }

            _animatedNodes.Add(tag, node);
            _updatedNodes[tag] = node;
        }

        public void DropAnimatedNode(int tag)
        {
            _animatedNodes.Remove(tag);
            _updatedNodes.Remove(tag);
        }

        public void StartListeningToAnimatedNodeValue(int tag, Action<double> callback)
        {
            if (!_animatedNodes.TryGetValue(tag, out var node))
            {
                throw new InvalidOperationException(
                    Invariant($"Animated node with tag '{tag}' does not exist."));
            }

            var valueNode = node as ValueAnimatedNode;
            if (valueNode == null)
            {
                throw new InvalidOperationException(
                    Invariant($"Animated node with tag '{tag}' is not a value node."));
            }

            valueNode.SetValueListener(callback);
        }

        public void StopListeningToAnimatedNodeValue(int tag)
        {
            var node = GetNode(tag);

            var valueNode = node as ValueAnimatedNode;
            if (valueNode == null)
            {
                throw new InvalidOperationException(
                    Invariant($"Animated node with tag '{tag}' is not a value node."));
            }

            valueNode.SetValueListener(null);
        }

        public void SetAnimatedNodeValue(int tag, double value)
        {
            var node = GetNode(tag);

            var valueNode = node as ValueAnimatedNode;
            if (valueNode == null)
            {
                throw new InvalidOperationException(
                    Invariant($"Animated node with tag '{tag}' is not a value node."));
            }

            valueNode.RawValue = value;
            StopAnimationsForNode(node);
            _updatedNodes[tag] = node;
        }

        public void SetAnimatedNodeOffset(int tag, double offset)
        {
            var node = GetNode(tag);
            var valueNode = node as ValueAnimatedNode;
            if (valueNode == null)
            {
                throw new InvalidOperationException(
                    Invariant($"Animated node with tag '{tag}' is not a value node"));
            }

            valueNode.Offset = offset;
            _updatedNodes[tag] = node;
        }

        public void FlattenAnimatedNodeOffset(int tag)
        {
            var node = GetNode(tag);
            var valueNode = node as ValueAnimatedNode;
            if (valueNode == null)
            {
                throw new InvalidOperationException(
                    Invariant($"Animated node with tag '{tag}' is not a value node"));
            }

            valueNode.FlattenOffset();
        }

        public void ExtractAnimatedNodeOffset(int tag)
        {
            var node = GetNode(tag);
            var valueNode = node as ValueAnimatedNode;
            if (valueNode == null)
            {
                throw new InvalidOperationException(
                    Invariant($"Animated node with tag '{tag}' is not a value node"));
            }

            valueNode.ExtractOffset();
        }

        public void StartAnimatingNode(int animationId, int animatedNodeTag, JObject animationConfig, ICallback endCallback)
        {
            var node = GetNode(animatedNodeTag);

            var valueNode = node as ValueAnimatedNode;
            if (valueNode == null)
            {
                throw new InvalidOperationException(
                    Invariant($"Animated node with tag '{animatedNodeTag}' is not a value node."));
            }

            var type = animationConfig.Value<string>("type");
            var animation = default(AnimationDriver);
            switch (type)
            {
                case "decay":
                    animation = new DecayAnimationDriver(animationId, valueNode, endCallback, animationConfig);
                    break;
                case "frames":
                    animation = new FrameBasedAnimationDriver(animationId, valueNode, endCallback, animationConfig);
                    break;
                case "spring":
                    animation = new SpringAnimationDriver(animationId, valueNode, endCallback, animationConfig);
                    break;
                default:
                    throw new InvalidOperationException(Invariant($"Unsupported animation type: '{type}'"));
            }

            _activeAnimations[animationId] = animation;
        }

        public void StopAnimation(int animationId)
        {
            if (_activeAnimations.TryGetValue(animationId, out var animation))
            {
                animation.EndCallback?.Invoke(new JObject
                {
                    { "finished", false },
                });

                _activeAnimations.Remove(animationId);
            }

            // Do not throw an error in the case the animation was not found.
            // We only keep active animations in the registry and there is a
            // chance that Animated.js will enqueue a stopAnimation call after
            // the animation has ended or the call will reach the native thread
            // only when the animation is already over. 
        }

        public void ConnectAnimatedNodes(int parentNodeTag, int childNodeTag)
        {
            var parentNode = GetNode(parentNodeTag);
            var childNode = GetNode(childNodeTag);
            parentNode.AddChild(childNode);
            _updatedNodes[childNodeTag] = childNode;
        }

        public void DisconnectAnimatedNodes(int parentNodeTag, int childNodeTag)
        {
            var parentNode = GetNode(parentNodeTag);
            var childNode = GetNode(childNodeTag);
            parentNode.RemoveChild(childNode);
            _updatedNodes[childNodeTag] = childNode;
        }

        public void ConnectAnimatedNodeToView(int animatedNodeTag, int viewTag)
        {
            var node = GetNode(animatedNodeTag);

            var propsAnimatedNode = node as PropsAnimatedNode;
            if (propsAnimatedNode == null)
            {
                throw new InvalidOperationException("Animated node connected to view should be props node.");
            }

            propsAnimatedNode.ConnectToView(viewTag);
            _updatedNodes[animatedNodeTag] = node;
        }

        public void DisconnectAnimatedNodeFromView(int animatedNodeTag, int viewTag)
        {
            var node = GetNode(animatedNodeTag);

            var propsAnimatedNode = node as PropsAnimatedNode;
            if (propsAnimatedNode == null)
            {
                throw new InvalidOperationException("Animated node connected to view should be props node.");
            }

            propsAnimatedNode.DisconnectFromView(viewTag);
        }

        public void RestoreDefaultValues(int animatedNodeTag, int viewTag)
        {
            if (!_animatedNodes.TryGetValue(animatedNodeTag, out var node))
            {
                // Restoring default values needs to happen before UIManager
                // operations so it is possible the node hasn't been created yet if
                // it is being connected and disconnected in the same batch. In
                // that case we don't need to restore default values since it will
                // never actually update the view.
                return;
            }

            var propsAnimatedNode = node as PropsAnimatedNode;
            if (propsAnimatedNode == null)
            {
                throw new InvalidOperationException("Animated node connected to view should be props node.");
            }

            propsAnimatedNode.RestoreDefaultValues();
        }

        public void AddAnimatedEventToView(int viewTag, string eventName, JObject eventMapping)
        {
            var nodeTag = eventMapping.Value<int>("animatedValueTag");
            if (!_animatedNodes.TryGetValue(nodeTag, out var node))
            {
                 throw new InvalidOperationException(
                     Invariant($"Animated node with tag '{nodeTag}' does not exist."));
            }

            var valueNode = node as ValueAnimatedNode;
            if (valueNode == null)
            {
                 throw new InvalidOperationException(
                     Invariant($"Animated node connected to event should be of type '{nameof(ValueAnimatedNode)}'."));
            }
            
            var pathList = eventMapping["nativeEventPath"].ToObject<string[]>();
            var @event = new EventAnimationDriver(pathList, valueNode);
            var key = Tuple.Create(viewTag, eventName);
            if (_eventDrivers.ContainsKey(key))
            {
                _eventDrivers[key].Add(@event);
            }
            else
            {
                var drivers = new List<EventAnimationDriver>(1)
                {
                    @event,
                };

                _eventDrivers.Add(key, drivers);
            }
        }

        public void RemoveAnimatedEventFromView(int viewTag, string eventName, int animatedValueTag)
        {
            var key = Tuple.Create(viewTag, eventName);
            if (_eventDrivers.ContainsKey(key))
            {
                var driversForKey = _eventDrivers[key];
                if (driversForKey.Count == 1)
                {
                    _eventDrivers.Remove(key);
                }
                else
                {
                    for (var i = 0; i < driversForKey.Count; ++i)
                    {
                        var driver = driversForKey[i];
                        if (driver.ValueNode.Tag == animatedValueTag)
                        {
                            driversForKey.RemoveAt(i);
                            break;
                        }
                    }
                }
            }
        }

        public void OnEventDispatch(Event @event)
        {
            if (DispatcherHelpers.IsOnDispatcher())
            {
                HandleEvent(@event);
            }
            else
            {
                DispatcherHelpers.RunOnDispatcher(() =>
                {
                    HandleEvent(@event);
                });
            }
        }


        private void HandleEvent(Event @event)
        {
            if (_eventDrivers.Count > 0)
            {
                var eventName = _customEventNamesResolver(@event.EventName);
                if (_eventDrivers.TryGetValue(Tuple.Create(@event.ViewTag, eventName), out var driversForKey))
                {
                    foreach (var driver in driversForKey)
                    {
                        StopAnimationsForNode(driver.ValueNode);
                        @event.Dispatch(driver);
                        _runUpdateNodeList.Add(driver.ValueNode);
                    }

                    UpdateNodes(_runUpdateNodeList);
                    _runUpdateNodeList.Clear();
                }
            }
        }

        /// <summary>
        /// Animation loop performs BFS over the graph of animated nodes.
        /// </summary>
        /// <remarks>
        /// We use incremented <see cref="_animatedGraphBFSColor"/> to mark
        /// nodes as visited in each of the BFS passes, which saves additional
        /// loops for clearing "visited" states.
        /// 
        /// First BFS starts with nodes that are in <see cref="_updatedNodes" />
        /// (this is, their value has been modified from JS in the last batch
        /// of JS operations) or directly attached to an active animation
        /// (thus linked to objects from <see cref="_activeAnimations"/>). In
        /// that step we calculate an attribute <see cref="AnimatedNode.ActiveIncomingNodes"/>.
        /// The second BFS runs in topological order over the sub-graph of
        /// *active* nodes. This is done by adding nodes to the BFS queue only
        /// if all its "predecessors" have already been visited.
        /// </remarks>
        /// <param name="renderingTime">Frame rendering time.</param>
        public void RunUpdates(TimeSpan renderingTime)
        {
            DispatcherHelpers.AssertOnDispatcher();
            var hasFinishedAnimations = false;

            _runUpdateNodeList.AddRange(_updatedNodes.Values);

            // Clean _updatedNodes queue
            _updatedNodes.Clear();

            var length = _activeAnimations.Count;
            UpdateActiveAnimationIds();
            for (var i = 0; i < length; ++i)
            {
                var animationId = _activeAnimationIds[i];
                var animation = _activeAnimations[animationId];
                animation.RunAnimationStep(renderingTime);
                var valueNode = animation.AnimatedValue;
                _runUpdateNodeList.Add(valueNode);
                if (animation.HasFinished)
                {
                    hasFinishedAnimations = true;
                }
            }

            UpdateNodes(_runUpdateNodeList);
            _runUpdateNodeList.Clear();

            // Cleanup finished animations.
            if (hasFinishedAnimations)
            {
                for (var i = 0; i < length; ++i)
                {
                    var animationId = _activeAnimationIds[i];
                    var animation = _activeAnimations[animationId];
                    if (animation.HasFinished)
                    {
                        animation.EndCallback?.Invoke(new JObject
                        {
                            { "finished", true },
                        });

                        _activeAnimations.Remove(animationId);
                    }
                }
            }
        }

        private void StopAnimationsForNode(AnimatedNode animatedNode)
        {
            var length = _activeAnimations.Count;
            UpdateActiveAnimationIds();
            for (var i = 0; i < length; ++i)
            {
                var animationId = _activeAnimationIds[i];
                var animation = _activeAnimations[animationId];
                if (animatedNode == animation.AnimatedValue)
                {
                    // Invoke animation end callback with {finished: false}
                    animation.EndCallback?.Invoke(new JObject
                    {
                        { "finished", false }, 
                    });

                    _activeAnimations.Remove(animationId);
                }
            }
        }

        private void UpdateNodes(IList<AnimatedNode> nodes)
        {
            var activeNodesCount = 0;
            var updatedNodesCount = 0;

            // STEP 1.
            // BFS over graph of nodes starting from ones from `_updatedNodes` and ones that are attached to
            // active animations (from `mActiveAnimations)`. Update `ActiveIncomingNodes` property for each node
            // during that BFS. Store number of visited nodes in `activeNodesCount`. We "execute" active
            // animations as a part of this step.

            _animatedGraphBFSColor++; /* use new color */
            if (_animatedGraphBFSColor == AnimatedNode.InitialBfsColor)
            {
                // value "0" is used as an initial color for a new node, using it in BFS may cause some nodes to be skipped.
                _animatedGraphBFSColor++;
            }

            var nodesQueue = new Queue<AnimatedNode>();
            foreach (var node in nodes)
            {
                if (node.BfsColor != _animatedGraphBFSColor)
                {
                    node.BfsColor = _animatedGraphBFSColor;
                    activeNodesCount++;
                    nodesQueue.Enqueue(node);
                }
            }

            while (nodesQueue.Count > 0)
            {
                var nextNode = nodesQueue.Dequeue();
                if (nextNode.Children != null)
                {
                    foreach (var child in nextNode.Children)
                    {
                        child.ActiveIncomingNodes++;
                        if (child.BfsColor != _animatedGraphBFSColor)
                        {
                            child.BfsColor = _animatedGraphBFSColor;
                            activeNodesCount++;
                            nodesQueue.Enqueue(child);
                        }
                    }
                }
            }

            // STEP 2
            // BFS over the graph of active nodes in topological order -> visit node only when all its
            // "predecessors" in the graph have already been visited. It is important to visit nodes in that
            // order as they may often use values of their predecessors in order to calculate "next state"
            // of their own. We start by determining the starting set of nodes by looking for nodes with
            // `ActiveIncomingNodes = 0` (those can only be the ones that we start BFS in the previous
            // step). We store number of visited nodes in this step in `updatedNodesCount`

            _animatedGraphBFSColor++;
            if (_animatedGraphBFSColor == AnimatedNode.InitialBfsColor)
            {
                // see reasoning for this check a few lines above
                _animatedGraphBFSColor++;
            }


            // find nodes with zero "incoming nodes", those can be either nodes from `mUpdatedNodes` or
            // ones connected to active animations
            foreach (var node in nodes)
            {
                if (node.ActiveIncomingNodes == 0 && node.BfsColor != _animatedGraphBFSColor)
                {
                    node.BfsColor = _animatedGraphBFSColor;
                    updatedNodesCount++;
                    nodesQueue.Enqueue(node);
                }
            }

            // Run main "update" loop
            while (nodesQueue.Count > 0)
            {
                var nextNode = nodesQueue.Dequeue();
                nextNode.Update();

                if (nextNode is PropsAnimatedNode propsNode)
                {
                    propsNode.UpdateView();
                }
                else if (nextNode is ValueAnimatedNode valueNode)
                {
                    // Potentially send events to JS when the node's value is updated
                    valueNode.OnValueUpdate();
                }

                if (nextNode.Children != null)
                {
                    foreach (var child in nextNode.Children)
                    {
                        child.ActiveIncomingNodes--;
                        if (child.BfsColor != _animatedGraphBFSColor && child.ActiveIncomingNodes == 0)
                        {
                            child.BfsColor = _animatedGraphBFSColor;
                            updatedNodesCount++;
                            nodesQueue.Enqueue(child);
                        }
                    }
                }
            }

            // Verify that we've visited *all* active nodes. Throw otherwise as this would mean there is a
            // cycle in animated node graph. We also take advantage of the fact that all active nodes are
            // visited in the step above so that all the nodes properties `ActiveIncomingNodes` are set to
            // zero
            if (activeNodesCount != updatedNodesCount)
            {
                throw new InvalidOperationException(
                    Invariant($"Looks like animated nodes graph has cycles, there are {activeNodesCount} but visited only {updatedNodesCount}."));
            }
        }

        internal AnimatedNode GetNodeById(int tag)
        {
            return GetNode(tag);
        }

        private AnimatedNode GetNode(int tag)
        {
            if (!_animatedNodes.TryGetValue(tag, out var node))
            {
                throw new InvalidOperationException(
                    Invariant($"Animated node with tag '{tag}' does not exist."));
            }

            return node;
        }

        private void UpdateActiveAnimationIds()
        {
            if (_activeAnimationIds == null || _activeAnimationIds.Length < _activeAnimations.Count)
            {
                _activeAnimationIds = new int[_activeAnimations.Count];
            }

            _activeAnimations.Keys.CopyTo(_activeAnimationIds, 0);
        }

        private static JObject GetEventTypes(UIManagerModule uiManager)
        {
            return (JObject)uiManager.ModuleConstants["customDirectEventTypes"];
        }
    }
}
