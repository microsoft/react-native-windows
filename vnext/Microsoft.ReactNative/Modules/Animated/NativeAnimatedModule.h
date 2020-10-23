// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <cxxreact/CxxModule.h>
#include <folly/dynamic.h>
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
namespace react::uwp {
class NativeAnimatedModule final : public facebook::xplat::module::CxxModule {
 public:
  NativeAnimatedModule(
      const std::weak_ptr<IReactInstance> &reactInstance,
      std::shared_ptr<facebook::react::MessageQueueThread> uiMessageQueue);
  virtual ~NativeAnimatedModule();

  // CxxModule
  std::string getName() override {
    return name;
  };
  std::map<std::string, folly::dynamic> getConstants() override {
    return {};
  };
  auto getMethods() -> std::vector<Method> override;

  void CreateAnimatedNode(int64_t tag, const folly::dynamic &config);
  void ConnectAnimatedNodeToView(int64_t animatedNodeTag, int64_t viewTag);
  void DisconnectAnimatedNodeFromView(int64_t animatedNodeTag, int64_t viewTag);
  void ConnectAnimatedNodes(int64_t parentNodeTag, int64_t childNodeTag);
  void DisconnectAnimatedNodes(int64_t parentNodeTag, int64_t childNodeTag);
  void StartAnimatingNode(
      int64_t animationId,
      int64_t animatedNodeTag,
      const folly::dynamic &animationConfig,
      const Callback &endCallback);
  void StopAnimation(int64_t animationId);
  void DropAnimatedNode(int64_t tag);
  void SetAnimatedNodeValue(int64_t tag, double value);
  void SetAnimatedNodeOffset(int64_t tag, double offset);
  void FlattenAnimatedNodeOffset(int64_t tag);
  void ExtractAnimatedNodeOffset(int64_t tag);
  void AddAnimatedEventToView(int64_t tag, const std::string &eventName, const folly::dynamic &eventMapping);
  void RemoveAnimatedEventFromView(int64_t tag, const std::string &eventName, int64_t animatedValueTag);
  void StartListeningToAnimatedNodeValue(int64_t tag);
  void StopListeningToAnimatedNodeValue(int64_t tag);

  static const char *name;

 private:
  std::shared_ptr<NativeAnimatedNodeManager> m_nodesManager{};
  std::weak_ptr<IReactInstance> m_wkReactInstance;
  std::weak_ptr<facebook::react::MessageQueueThread> m_uiMessageQueue;

  static const char *s_createAnimatedNodeName;
  static const char *s_connectAnimatedNodeToViewName;
  static const char *s_disconnectAnimatedNodeFromViewName;
  static const char *s_connectAnimatedNodesName;
  static const char *s_disconnectAnimatedNodesName;
  static const char *s_stopAnimationName;
  static const char *s_startAnimatingNodeName;
  static const char *s_dropAnimatedNodeName;
  static const char *s_setAnimatedNodeValueName;
  static const char *s_setAnimatedNodeOffsetName;
  static const char *s_flattenAnimatedNodeOffsetName;
  static const char *s_extractAnimatedNodeOffsetName;
  static const char *s_addAnimatedEventToViewName;
  static const char *s_removeAnimatedEventFromViewName;
  static const char *s_startListeningToAnimatedNodeValueName;
  static const char *s_stopListeningToAnimatedNodeValueName;
};
} // namespace react::uwp
