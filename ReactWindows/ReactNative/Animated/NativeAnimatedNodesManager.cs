using Newtonsoft.Json.Linq;
using ReactNative.Bridge;
using ReactNative.UIManager;
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
    /// responsible for calculating property map which can be sent to native
    /// view hierarchy to update the view.
    /// </summary>
    /// <remarks>
    /// This class should be accessed only from the dispatcher thread.
    /// </remarks>
    class NativeAnimatedNodesManager
    {
        private readonly Dictionary<int, AnimatedNode> _animatedNodes = new Dictionary<int, AnimatedNode>();
        private readonly List<AnimationDriver> _activeAnimations = new List<AnimationDriver>();
        private readonly List<AnimatedNode> _updatedNodes = new List<AnimatedNode>();
        private readonly UIImplementation _uiImplementation;

        private int _animatedGraphBFSColor = 0;

        public NativeAnimatedNodesManager(UIImplementation uiImplementation)
        {
            this._uiImplementation = uiImplementation;
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
                    node = new PropsAnimatedNode(tag, config, this);
                    break;
                case "interpolation":
                    node = new InterpolationAnimatedNode(tag, config);
                    break;
                case "addition":
                    node = new AdditionAnimatedNode(tag, config, this);
                    break;
                case "multiplication":
                    node = new MultiplicationAnimatedNode(tag, config, this);
                    break;
                case "transform":
                    node = new TransformAnimatedNode(tag, config, this);
                    break;
                default:
                    throw new InvalidOperationException(Invariant($"Unsupported node type: '{type}'"));
            }

            _animatedNodes.Add(tag, node);
        }

        public void DropAnimatedNode(int tag)
        {
            _animatedNodes.Remove(tag);
        }

        public void StartListeningToAnimatedNodeValue(int tag, Action<double> callback)
        {
            var node = default(AnimatedNode);
            if (!_animatedNodes.TryGetValue(tag, out node))
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

            valueNode.Value = value;
            _updatedNodes.Add(node);
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

            _activeAnimations.Add(animation);
        }

        public void StopAnimation(int animationId)
        {
            // In most cases, there should never be more than a few active
            // animations running at the same time. Therefore it does not make
            // much sense to create an animationId -> animation map that would
            // require additional memory just to support the use case of
            // stopping the animation.

            for (var i = 0; i < _activeAnimations.Count; ++i)
            {
                var animation = _activeAnimations[i];
                if (animation.Id == animationId)
                {
                    animation.EndCallback.Invoke(new JObject
                    {
                        { "finished", false },
                    });

                    _activeAnimations.RemoveAt(i);
                    return;
                }
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
        }

        public void DisconnectAnimatedNodes(int parentNodeTag, int childNodeTag)
        {
            var parentNode = GetNode(parentNodeTag);
            var childNode = GetNode(childNodeTag);
            parentNode.RemoveChild(childNode);
        }

        public void ConnectAnimatedNodeToView(int animatedNodeTag, int viewTag)
        {
            var node = GetNode(animatedNodeTag);

            var propsAnimatedNode = node as PropsAnimatedNode;
            if (propsAnimatedNode == null)
            {
                throw new InvalidOperationException("Animated node connected to view should be props node.");
            }

            if (propsAnimatedNode.ConnectedViewTag != -1)
            {
                throw new InvalidOperationException(
                    Invariant($"Animated node '{animatedNodeTag}' is already attached to a view."));
            }

            propsAnimatedNode.ConnectedViewTag = viewTag;
        }

        public void DisconnectAnimatedNodeFromView(int animatedNodeTag, int viewTag)
        {
            var node = GetNode(animatedNodeTag);

            var propsAnimatedNode = node as PropsAnimatedNode;
            if (propsAnimatedNode == null)
            {
                throw new InvalidOperationException("Animated node connected to view should be props node.");
            }

            if (propsAnimatedNode.ConnectedViewTag != viewTag)
            {
                throw new InvalidOperationException(
                    "Attempting to disconnect view that has not been connected with the given animated node.");
            }

            propsAnimatedNode.ConnectedViewTag = -1;
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
            var activeNodesCount = 0;
            var updatedNodesCount = 0;
            var hasFinishedAnimations = false;

            // STEP 1.
            // BFS over graph of nodes starting from ones from `_updatedNodes` and ones that are attached to
            // active animations (from `mActiveAnimations)`. Update `ActiveIncomingNodes` property for each node
            // during that BFS. Store number of visited nodes in `activeNodesCount`. We "execute" active
            // animations as a part of this step.

            _animatedGraphBFSColor++; /* use new color */

            var nodesQueue = new Queue<AnimatedNode>();
            foreach (var node in _updatedNodes)
            {
                if (node.BfsColor != _animatedGraphBFSColor)
                {
                    node.BfsColor = _animatedGraphBFSColor;
                    activeNodesCount++;
                    nodesQueue.Enqueue(node);
                }
            }

            foreach (var animation in _activeAnimations)
            {
                animation.RunAnimationStep(renderingTime);
                var valueNode = animation.AnimatedValue;
                if (valueNode.BfsColor != _animatedGraphBFSColor)
                {
                    valueNode.BfsColor = _animatedGraphBFSColor;
                    activeNodesCount++;
                    nodesQueue.Enqueue(valueNode);
                }

                if (animation.HasFinished)
                {
                    hasFinishedAnimations = true;
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

            // find nodes with zero "incoming nodes", those can be either nodes from `mUpdatedNodes` or
            // ones connected to active animations
            foreach (var node in _updatedNodes)
            {
                if (node.ActiveIncomingNodes == 0 && node.BfsColor != _animatedGraphBFSColor)
                {
                    node.BfsColor = _animatedGraphBFSColor;
                    updatedNodesCount++;
                    nodesQueue.Enqueue(node);
                }
            }

            foreach (var animation in _activeAnimations)
            {
                var valueNode = animation.AnimatedValue;
                if (valueNode.ActiveIncomingNodes == 0 && valueNode.BfsColor != _animatedGraphBFSColor)
                {
                    valueNode.BfsColor = _animatedGraphBFSColor;
                    updatedNodesCount++;
                    nodesQueue.Enqueue(valueNode);
                }
            }

            // Run main "update" loop
            while (nodesQueue.Count > 0)
            {
                var nextNode = nodesQueue.Dequeue();
                nextNode.Update();

                var propsNode = nextNode as PropsAnimatedNode;
                var valueNode = default(ValueAnimatedNode);
                if (propsNode != null)
                {
                    propsNode.UpdateView(_uiImplementation);
                }
                else if ((valueNode = nextNode as ValueAnimatedNode) != null)
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
            // visited in the step above so that all the nodes properties `mActiveIncomingNodes` are set to
            // zero
            if (activeNodesCount != updatedNodesCount)
            {
                throw new InvalidOperationException(
                    Invariant($"Looks like animated nodes graph has cycles, there are {activeNodesCount} but visited only {updatedNodesCount}."));
            }

            // Clean _updatedNodes queue
            _updatedNodes.Clear();

            // Cleanup finished animations. Iterate over the array of animations and override ones that has
            // finished, then resize `_activeAnimations`.
            if (hasFinishedAnimations)
            {
                int dest = 0;
                for (var i = 0; i < _activeAnimations.Count; ++i)
                {
                    var animation = _activeAnimations[i];
                    if (!animation.HasFinished)
                    {
                        _activeAnimations[dest++] = animation;
                    }
                    else
                    {
                        animation.EndCallback.Invoke(new JObject
                        {
                            { "finished", true },
                        });
                    }
                }

                for (var i = _activeAnimations.Count - 1; i >= dest; --i)
                {
                    _activeAnimations.RemoveAt(i);
                }
            }
        }

        internal AnimatedNode GetNodeById(int tag)
        {
            return GetNode(tag);
        }

        private AnimatedNode GetNode(int tag)
        {
            var node = default(AnimatedNode);
            if (!_animatedNodes.TryGetValue(tag, out node))
            {
                throw new InvalidOperationException(
                    Invariant($"Animated node with tag '{tag}' does not exist."));
            }

            return node;
        }
    }
}
