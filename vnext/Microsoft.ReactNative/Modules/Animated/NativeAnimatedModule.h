// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "codegen/NativeAnimatedModuleSpec.g.h"

#include "NativeAnimatedNodeManager.h"

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
/// criteria updates attached values via a composition expression animation
/// (we have few types of those, e.g., spring, timing, decay). Animation objects
/// can be "started" and "stopped". Those are like "pulse generators" for the
/// rest of the nodes graph. Those pulses then propagate along the graph to the
/// children nodes up to the special node type: AnimatedProps which then
/// can be used to calculate prop update map for a view.
///
/// This class acts as a proxy between the "native" API that can be called
/// from JS and the main class that coordinates all the action:
/// <see cref="NativeAnimatedNodeManager"/>.
/// </remarks>
namespace Microsoft::ReactNative {

REACT_MODULE(NativeAnimatedModule)
struct NativeAnimatedModule : std::enable_shared_from_this<NativeAnimatedModule> {
  // Commented out due to missing implementation of updateAnimatedNodeConfig
  // using ModuleSpec = ReactNativeSpecs::AnimatedModuleSpec;

  REACT_INIT(Initialize)
  void Initialize(winrt::Microsoft::ReactNative::ReactContext const &reactContext) noexcept;

  REACT_METHOD(startOperationBatch)
  void startOperationBatch() noexcept;

  REACT_METHOD(finishOperationBatch)
  void finishOperationBatch() noexcept;

  REACT_METHOD(createAnimatedNode)
  void createAnimatedNode(double tag, ::React::JSValue &&config) noexcept;

  // REACT_METHOD(updateAnimatedNodeConfig)
  // void updateAnimatedNodeConfig(double tag, ::React::JSValue && config) noexcept;

  REACT_METHOD(getValue)
  void getValue(double tag, std::function<void(double)> const &saveValueCallback) noexcept;

  REACT_METHOD(startListeningToAnimatedNodeValue)
  void startListeningToAnimatedNodeValue(double tag) noexcept;

  REACT_METHOD(stopListeningToAnimatedNodeValue)
  void stopListeningToAnimatedNodeValue(double tag) noexcept;

  REACT_METHOD(connectAnimatedNodes)
  void connectAnimatedNodes(double parentTag, double childTag) noexcept;

  REACT_METHOD(disconnectAnimatedNodes)
  void disconnectAnimatedNodes(double parentTag, double childTag) noexcept;

  REACT_METHOD(startAnimatingNode)
  void startAnimatingNode(
      double animationId,
      double nodeTag,
      ::React::JSValue &&config,
      std::function<void(ReactNativeSpecs::AnimatedModuleSpec_EndResult const &)> const &endCallback) noexcept;

  REACT_METHOD(stopAnimation)
  void stopAnimation(double animationId) noexcept;

  REACT_METHOD(setAnimatedNodeValue)
  void setAnimatedNodeValue(double nodeTag, double value) noexcept;

  REACT_METHOD(setAnimatedNodeOffset)
  void setAnimatedNodeOffset(double nodeTag, double offset) noexcept;

  REACT_METHOD(flattenAnimatedNodeOffset)
  void flattenAnimatedNodeOffset(double nodeTag) noexcept;

  REACT_METHOD(extractAnimatedNodeOffset)
  void extractAnimatedNodeOffset(double nodeTag) noexcept;

  REACT_METHOD(connectAnimatedNodeToView)
  void connectAnimatedNodeToView(double nodeTag, double viewTag) noexcept;

  REACT_METHOD(disconnectAnimatedNodeFromView)
  void disconnectAnimatedNodeFromView(double nodeTag, double viewTag) noexcept;

  REACT_METHOD(restoreDefaultValues)
  void restoreDefaultValues(double nodeTag) noexcept;

  REACT_METHOD(dropAnimatedNode)
  void dropAnimatedNode(double tag) noexcept;

  REACT_METHOD(addAnimatedEventToView)
  void addAnimatedEventToView(
      double viewTag,
      std::string eventName,
      ReactNativeSpecs::AnimatedModuleSpec_EventMapping &&eventMapping) noexcept;

  REACT_METHOD(removeAnimatedEventFromView)
  void removeAnimatedEventFromView(double viewTag, std::string eventName, double animatedNodeTag) noexcept;

  REACT_METHOD(addListener)
  void addListener(std::string eventName) noexcept;

  REACT_METHOD(removeListeners)
  void removeListeners(double count) noexcept;

  REACT_METHOD(queueAndExecuteBatchedOperations)
  void queueAndExecuteBatchedOperations(::React::JSValueArray &&operationsAndArgs) noexcept;

 private:
  std::shared_ptr<NativeAnimatedNodeManager> m_nodesManager;
  winrt::Microsoft::ReactNative::ReactContext m_context;
};
} // namespace Microsoft::ReactNative
