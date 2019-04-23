// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <mutex>

namespace WindowsSampleApp
{
	// A custom event that fires whenever the secondary view is ready to be closed. You should
	// clean up any state (including deregistering for events) then close the window in this handler
	public delegate void ViewReleasedHandler(Platform::Object^ sender, Platform::Object^ e);

	[Windows::UI::Xaml::Data::Bindable]
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class ViewLifetimeControl sealed : Windows::UI::Xaml::Data::INotifyPropertyChanged
	{
	public:
		static ViewLifetimeControl^ CreateForCurrentView();
		property Windows::UI::Core::CoreDispatcher^ Dispatcher
		{
			Windows::UI::Core::CoreDispatcher^ get();
		};

		property int Id
		{
			int get();
		};

		property Platform::String^ Title
		{
			Platform::String^ get();
			void set(Platform::String^ value);
		};

		int StartViewInUse();
		int StopViewInUse();

		event ViewReleasedHandler^ Released
		{
			Windows::Foundation::EventRegistrationToken add(ViewReleasedHandler^ handler);
			void remove(Windows::Foundation::EventRegistrationToken token);
		}
		virtual event Windows::UI::Xaml::Data::PropertyChangedEventHandler^ PropertyChanged;

	private:
		ViewLifetimeControl(Windows::UI::Core::CoreWindow^ newWindow);
		void RegisterForEvents();
		void UnregisterForEvents();
		void FinalizeRelease();
		void ViewConsolidated(Windows::UI::ViewManagement::ApplicationView^ sender, Windows::UI::ViewManagement::ApplicationViewConsolidatedEventArgs^ e);
		void VisibilityChanged(Windows::UI::Core::CoreWindow^ sender, Windows::UI::Core::VisibilityChangedEventArgs^ e);
		event ViewReleasedHandler^ InternalReleased;

		// Keeps track of if the consolidated event has fired yet. A view is consolidated with other views
		// when there's no way for the user to get to it (it's not in the list of recently used apps, cannot be
		// launched from Start, etc.) A view stops being consolidated when it's visible--at that point
		// the user can interact with it, move it on or off screen, etc.
		property bool Consolidated
		{
			bool get();
			void set(bool value);
		}
		Windows::Foundation::EventRegistrationToken consolidatedToken, visibilityToken;

		// Dispatcher for this view. Kept here for sending messages between this view and the main view.
		Windows::UI::Core::CoreDispatcher^ dispatcher;

		// Window for this particular view. Used to register and unregister for events
		Platform::Agile<Windows::UI::Core::CoreWindow^> window;

		// The title for the view shown in the list of recently used apps (by setting the title on
		// ApplicationView)
		Platform::String^ title;

		// This class uses reference counts to make sure the secondary views isn't closed prematurely.
		// Whenever the main view is about to interact with the secondary view, it should take a reference
		// by calling "StartViewInUse" on this object. When finished interacting, it should release the reference
		// by calling "StopViewInUse". You can see examples of this throughout the sample, especially in
		// scenario 1.
		int refCount;

		// Each view has a unique Id, found using the ApplicationView.Id property or
		// ApplicationView.GetApplicationViewIdForCoreWindow method. This id is used in all of the ApplicationViewSwitcher
		// and ProjectionManager APIs.
		int viewId;

		// Tracks if this ViewLifetimeControl object is still valid. If this is true, then the view is in the process
		// of closing itself down
		bool consolidated, released;
		std::mutex globalMutex;
	};
}