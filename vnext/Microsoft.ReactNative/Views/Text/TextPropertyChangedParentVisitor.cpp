// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "TextPropertyChangedParentVisitor.h"
#include <UI.Xaml.Automation.Peers.h>
#include <UI.Xaml.Automation.h>
#include <UI.Xaml.Controls.h>
#include <UI.Xaml.Documents.h>
#include <Utils/ShadowNodeTypeUtils.h>
#include <Views/TextViewManager.h>
#include <Views/VirtualTextViewManager.h>

namespace winrt {
using namespace xaml::Automation;
using namespace xaml::Automation::Peers;
using namespace xaml::Documents;
} // namespace winrt

namespace Microsoft::ReactNative {
void TextPropertyChangedParentVisitor::VisitCore(ShadowNodeBase *node) {
  // Update nested flag fast text updates
  m_isNested = !IsRawTextShadowNode(node);
  Super::VisitCore(node);
}

void TextPropertyChangedParentVisitor::VisitText(ShadowNodeBase *node) {
  // Raise LiveRegionChanged event
  const auto isTextUpdate = HasPropertyChangeType(PropertyChangeType::Text);
  if (isTextUpdate) {
    const auto element = node->GetView().as<xaml::Controls::TextBlock>();

    // If name is set, it's controlled by accessibilityLabel, and it's already
    // handled in FrameworkElementViewManager. Here it only handles when name is
    // not set.
    if (xaml::Automation::AutomationProperties::GetLiveSetting(element) != winrt::AutomationLiveSetting::Off &&
        xaml::Automation::AutomationProperties::GetName(element).empty() &&
        xaml::Automation::AutomationProperties::GetAccessibilityView(element) != winrt::AccessibilityView::Raw) {
      if (auto peer = xaml::Automation::Peers::FrameworkElementAutomationPeer::FromElement(element)) {
        peer.RaiseAutomationEvent(winrt::AutomationEvents::LiveRegionChanged);
      }
    }

    TextViewManager::UpdateOptimizedText(node);
  }

  // Refresh text highlighters
  const auto isHighlightAdded = HasPropertyChangeType(PropertyChangeType::AddHighlight);
  const auto isHighlightRemoved = HasPropertyChangeType(PropertyChangeType::RemoveHighlight);
  if (isTextUpdate || isHighlightAdded || isHighlightRemoved) {
    TextViewManager::UpdateTextHighlighters(node, isHighlightAdded);
  }

  // Set pressable descendant
  if (HasPropertyChangeType(PropertyChangeType::AddPressable)) {
    TextViewManager::SetDescendantPressable(node);
  }
}

void TextPropertyChangedParentVisitor::VisitVirtualText(ShadowNodeBase *node) {
  const auto textNode = static_cast<VirtualTextShadowNode *>(node);
  // Update descendant text highlight flag
  if (HasPropertyChangeType(PropertyChangeType::AddHighlight)) {
    textNode->hasDescendantTextHighlighter = true;
  }

  // Update pressable descendant flag
  if (HasPropertyChangeType(PropertyChangeType::AddPressable)) {
    textNode->hasDescendantPressable = true;
  }

  Super::VisitVirtualText(node);
}

} // namespace Microsoft::ReactNative
