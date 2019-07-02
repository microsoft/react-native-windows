// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// The objects defined here demonstrate how to make sure each of the views
// created remains alive as long as the app needs them, but only when they're
// being used by the app or the user. Many of the scenarios contained in this
// sample use these functions to keep track of the views available and ensure
// that the view is not closed while the scenario is attempting to show it.
//
// As you can see in scenario 1, the
// ApplicationViewSwitcher.TryShowAsStandaloneAsync and
// ProjectionManager.StartProjectingAsync methods let you show one view next to
// another. The Consolidated event is fired when a view stops being visible
// separately from other views. Common cases where this will occur is when the
// view falls out of the list of recently used apps, or when the user performs
// the close gesture on the view. This is a good time to close the view,
// provided the app isn't trying to show the view at the same time. This event
// is fired on the thread of the view that becomes consolidated.
//
// Each view lives on its own thread, so concurrency control is necessary. Also,
// as you'll see in the sample, certain objects may be bound to UI on given
// threads. Properties of those objects should only be updated on that UI
// thread.

#include "pch.h"
#include "ViewLifetimeControl.h"

using namespace Platform;
using namespace std;
using namespace Windows::Foundation;
using namespace Windows::UI::Core;
using namespace Windows::UI::ViewManagement;
using namespace Windows::UI::Xaml::Data;

namespace WindowsSampleApp {
// A ViewLifetimeControl is instantiated for every secondary view.
// ViewLifetimeControl's reference count keeps track of when the secondary view
// thinks it's in use and when the main view is interacting with the secondary
// view (about to show it to the user, etc.) When the reference count drops to
// zero, the secondary view is closed. Instantiate views using
// "CreateForCurrentView".
ViewLifetimeControl::ViewLifetimeControl(CoreWindow ^ newWindow)
    : consolidated(true) {
  dispatcher = newWindow->Dispatcher;
  window = newWindow;
  viewId = ApplicationView::GetApplicationViewIdForWindow(window.Get());

  // This class will automatically tell the view when its time to close
  // or stay alive in a few cases
  RegisterForEvents();
}

// Register for events on the current view
void ViewLifetimeControl::RegisterForEvents() {
  // A view is consolidated with other views when there's no way for the user to
  // get to it (it's not in the list of recently used apps, cannot be launched
  // from Start, etc.) A view stops being consolidated when it's visible--at
  // that point the user can interact with it, move it on or off screen, etc.
  // It's generally a good idea to close a view after it has been consolidated,
  // but keep it open while it's visible.
  consolidatedToken = ApplicationView::GetForCurrentView()->Consolidated +=
      ref new TypedEventHandler<
          ApplicationView ^
          , ApplicationViewConsolidatedEventArgs ^>(
          this, &ViewLifetimeControl::ViewConsolidated);
  visibilityToken = window->VisibilityChanged +=
      ref new TypedEventHandler<CoreWindow ^, VisibilityChangedEventArgs ^>(
          this, &ViewLifetimeControl::VisibilityChanged);
}

// Unregister for events. Call this method before closing the view to prevent
// leaks.
void ViewLifetimeControl::UnregisterForEvents() {
  ApplicationView::GetForCurrentView()->Consolidated -= consolidatedToken;
  window->VisibilityChanged -= visibilityToken;
}

void ViewLifetimeControl::VisibilityChanged(
    CoreWindow ^ sender,
    VisibilityChangedEventArgs ^ e) {
  if (e->Visible) {
    // A view is consolidated with other views when there's no way for the user
    // to get to it (it's not in the list of recently used apps, cannot be
    // launched from Start, etc.) A view stops being consolidated when it's
    // visible--at that point the user can interact with it, move it on or off
    // screen, etc. It's generally a good idea to close a view after it has been
    // consolidated, but keep it open while it's visible.
    Consolidated = false;
  }
}

void ViewLifetimeControl::ViewConsolidated(
    ApplicationView ^ sender,
    ApplicationViewConsolidatedEventArgs ^ e) {
  // A view is consolidated with other views hen there's no way for the user to
  // get to it (it's not in the list of recently used apps, cannot be launched
  // from Start, etc.) A view stops being consolidated when it's visible--at
  // that point the user can interact with it, move it on or off screen, etc.
  // It's generally a good idea to close a view after it has been consolidated,
  // but keep it open while it's visible.
  Consolidated = true;
}

// Called when a view has been "consolidated" (no longer accessible to the user)
// and no other view is trying to interact with it. This should only be closed
// after the reference count goes to 0 (including being consolidated). At the
// end of this, the view is closed.
void ViewLifetimeControl::FinalizeRelease() {
  bool justReleased = false;
  globalMutex.lock();
  if (!released && refCount == 0) {
    released = true;
    justReleased = true;
  }
  globalMutex.unlock();

  // This assumes that released will never be made false after it
  // it has been set to true
  if (justReleased) {
    UnregisterForEvents();
    InternalReleased(this, nullptr);
  }
}

// Creates ViewLifetimeControl for the particular view.
// Only do this once per view.
ViewLifetimeControl ^ ViewLifetimeControl::CreateForCurrentView() {
  return ref new ViewLifetimeControl(CoreWindow::GetForCurrentThread());
}

// For purposes of this sample, the collection of views
// is bound to a UI collection. This property is available for binding
String ^ ViewLifetimeControl::Title::get() {
  // This is expected to only be updated on the thread that this object is bound
  // to Hence, it's not necessary to lock
  return title;
}

void ViewLifetimeControl::Title::set(String ^ value) {
  // This is expected to only be updated on the thread that this object is bound
  // to Hence, it's not necessary to lock
  if (title != value) {
    title = value;
    PropertyChanged(this, ref new PropertyChangedEventArgs("Title"));
  }
}

// Dispatcher for this view. Kept here for sending messages between this view
// and the main view.
CoreDispatcher ^ ViewLifetimeControl::Dispatcher::get() {
  // This property never changes, so there's no need to lock
  return dispatcher;
}

// Each view has a unique Id, found using the ApplicationView.Id property or
// ApplicationView.GetApplicationViewIdForCoreWindow method. This id is used in
// all of the ApplicationViewSwitcher and ProjectionManager APIs.
int ViewLifetimeControl::Id::get() {
  // This property never changes, so there's no need to lock
  return viewId;
}

// Keeps track of if the consolidated event has fired yet. A view is
// consolidated with other views when there's no way for the user to get to it
// (it's not in the list of recently used apps, cannot be launched from Start,
// etc.) A view stops being consolidated when it's visible--at that point the
// user can interact with it, move it on or off screen, etc.
bool ViewLifetimeControl::Consolidated::get() {
  return consolidated;
}

void ViewLifetimeControl::Consolidated::set(bool value) {
  // This property should only be accessed by the thread on which the view
  // lives, so there's no need to lock
  if (consolidated != value) {
    consolidated = value;
    if (consolidated) {
      // The view isn't accessible to the user, so it's OK to close it.
      StopViewInUse();
    } else {
      // The view has become visible, so do not close it until it's consolidated
      StartViewInUse();
    }
  }
}

// Signals that the view is being interacted with by another view,
// so it shouldn't be closed even if it becomes "consolidated"
int ViewLifetimeControl::StartViewInUse() {
  bool releasedCopy = false;
  int refCountCopy = 0;

  globalMutex.lock();
  releasedCopy = released;
  if (!released) {
    refCountCopy = ++refCount;
  }
  globalMutex.unlock();

  if (releasedCopy) {
    throw ref new ObjectDisposedException("The view is being disposed");
  }

  return refCountCopy;
}

// Should come after any call to StartViewInUse
// Signals that the another view has finished interacting with the view tracked
// by this object
int ViewLifetimeControl::StopViewInUse() {
  bool releasedCopy = false;
  int refCountCopy = -1;

  globalMutex.lock();
  releasedCopy = released;
  if (!released && refCount > 0) {
    refCountCopy = --refCount;
    if (refCountCopy == 0) {
      // If no other view is interacting with this view, and
      // the view isn't accessible to the user, it's appropriate
      // to close it
      //
      // Before actually closing the view, make sure there are no
      // other important events waiting in the queue (this low-priority item
      // will run after other events)
      dispatcher->RunAsync(
          CoreDispatcherPriority::Low,
          ref new DispatchedHandler(
              this, &ViewLifetimeControl::FinalizeRelease));
    }
  }
  globalMutex.unlock();

  if (releasedCopy) {
    throw ref new ObjectDisposedException("The view is being disposed");
  }

  if (refCountCopy < 0) {
    throw ref new Exception(
        HRESULT_FROM_WIN32(ERROR_INVALID_STATE),
        "Object was released too many times");
  }

  return refCountCopy;
}

// Signals to consumers that its time to close the view so that
// they can clean up (including calling Window.Close() when finished)
EventRegistrationToken ViewLifetimeControl::Released::add(
    ViewReleasedHandler ^ handler) {
  bool releasedCopy = false;
  EventRegistrationToken eventToken;
  globalMutex.lock();
  releasedCopy = released;
  if (!released) {
    eventToken = InternalReleased += handler;
  }
  globalMutex.unlock();

  if (releasedCopy) {
    throw ref new ObjectDisposedException("The view is being disposed");
  }

  return eventToken;
}

void ViewLifetimeControl::Released::remove(EventRegistrationToken token) {
  lock_guard<mutex> lock(globalMutex);
  return InternalReleased -= token;
}
} // namespace WindowsSampleApp