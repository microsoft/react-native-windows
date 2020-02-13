# Focus Tracking Across NetUI and React Boundaries

## Focus Tracking is Hard
The asynchronous boundaries between the native host platform (NetUI) and the JS layers of React Native make the tracking of focus a difficult problem. Trying not to rely on perceived event ordering, including the unmounting of React components and the destruction of their native projections, is particularly problematic. Consider tracking focus for styling purposes; it is easy for focus styles on parents to remain post focused components unmounting.

## Focus On vs Focus In
To distinguish between focus being on a particular native element and focus being within one of it's children, we can use focus/blur and focusIn/focusOut to refer to each respectively.

## Proposal
### NetUI Elements and React IDs
Today NetUI elements that correspond 1-1 with a React component set the tag passed down as a property on the NetUI element. Making sure to clear on unmount, but before destruction of the NetUI element makes determining which component should be the target of an event much easier.

### On Unmount
On receiving a request to remove an element from the tree, the following must happen
1. ShadowNode must clear the ReactID from the NetUI element (it is no longer valid)
2. After NetUI makes a best effort to move focus, the ReactRootView can hook into the focus move event by overriding OnFocusMoved. Using the lost and gained focus it can do tree walks (going up to parents) to determine which components now have focusIn and which no longer do.
3. Disatch the focusOut event as a bubbling event starting at the "leafiest" React component (use tags to verify), prior to dispathing the bubbling focusIn event, again starting at the leafiest React component that can be found in the ancestor chain of the element.

Note that we maintain the current bubbling focus and blur events.

### Considerations and Consequences
- If event ordering is not guranteed, reducing to chains that get passed in a single payload in a focusMoved event seems reasonable
- Components like TouchableWin32 should split focus tracking into **FocusOn** and **FocusWithin** states