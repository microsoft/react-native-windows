/**
 * Copyright: Microsoft 2015
 *
 * RN-specific implementation of the cross-platform View abstraction.
 */

import * as React from 'react';
import {useCallback, useRef, useLayoutEffect} from 'react';
import type {ViewProps} from './ViewPropTypes';

import View from './View';
import {findNodeHandle} from '../../ReactNative/RendererProxy';
import UIManager from '../../ReactNative/UIManager';
import useMergeRefs from '../../Utilities/useMergeRefs';
import warnOnce from '../../Utilities/warnOnce';

/**
 * Basic View component with additional Win32 specific functionality
 */


const ViewWin32: React.AbstractComponent<
  ViewProps,
  React.ElementRef<typeof View>,
> = React.forwardRef(
  (
    {
      accessibilityLabeledBy,
      accessibilityDescribedBy, 
      accessibilityControls, 
      ...rest
    }: ViewProps,
    forwardedRef,
  ) => {


    /**
     * Process accessibility refs into node handles after initial DOM render, before sent across the bridge.
     * useLayoutEffect will invalidate the render to assess the ref-based accessibility properties.
     * 
     * RN core moved these properties to use NativeID instead of Refs, so this is only kept for back compat
     */

    const [labeledByTarget, setLabeledByTarget] = React.useState<number|null>(null);
    const [describedByTarget, setDescribedByTarget] = React.useState<number|null>(null);
    const [controlsTarget, setControlsTarget] = React.useState<number|null>(null);

    useLayoutEffect(() => {
      if (accessibilityLabeledBy !== undefined && accessibilityLabeledBy.current !== null)
      {
        warnOnce('accessibilityLabeledByAsRef', 'accessibilityLabeledBy has been deprecated.  Instead use accessibilityLabelledBy (double L), and specify it as a string matching the nativeID of the target component.  Support of specifying accessibilityLabeledBy will be removed in a future release');
        setLabeledByTarget(findNodeHandle(accessibilityLabeledBy.current));
      }
    }, [accessibilityLabeledBy]);

    useLayoutEffect(() => {
      if ((typeof accessibilityDescribedBy !== "string") && accessibilityDescribedBy !== undefined && accessibilityDescribedBy.current !== null)
      {
        warnOnce('accessibilityDescribedByRef', 'accessibilityDescribedBy should be specfied as a string matching the nativeID of the target component.  Support of specifying accessibilityDescribedBy using a ref is deprecated and will be removed in a future release');
        setDescribedByTarget(findNodeHandle(accessibilityDescribedBy.current));
      }
    }, [accessibilityDescribedBy]);
    
    useLayoutEffect(() => {
      if((typeof accessibilityControls !== "string") && accessibilityControls !== undefined && accessibilityControls.current !== null)
      {
        warnOnce('accessibilityControlsRef', 'accessibilityControls should be specfied as a string matching the nativeID of the target component.  Support of specifying accessibilityControls using a ref is deprecated and will be removed in a future release');
        setControlsTarget(findNodeHandle(accessibilityControls.current));
      }
    }, [accessibilityControls]);

    /**
     * Set up the forwarding ref to enable adding the focus method.
     */
    const focusRef = useRef<React.ElementRef<typeof View | null>(null);

    const setLocalRef = useCallback(
      (instance : View | null) => {
        focusRef.current = instance;  
        /**
         * Add focus() as a callable function to the forwarded reference.
         */
        if (instance)
        {
          instance.focus = () => {
            UIManager.dispatchViewManagerCommand(
              findNodeHandle(instance),
              UIManager.getViewManagerConfig('RCTView').Commands.focus,
              null
              );
          };
        }
    },
    [focusRef],
    );

    const setNativeRef = useMergeRefs<View | ({
      setLocalRef,
      forwardedRef,
    })<null;

    return <View ref={setNativeRef}
    {...rest}
    {...((controlsTarget !== null) ? {accessibilityControls:controlsTarget} : {})}
    {...((labeledByTarget !== null) ? {accessibilityLabeledBy:labeledByTarget} : {})}
    {...((describedByTarget !== null) ? {accessibilityDescribedBy:describedByTarget} : {})}
    {...((typeof accessibilityDescribedBy === "string") ? {accessibilityDescribedBy} : {})}
    {...((typeof accessibilityControls === "string") ? {accessibilityControls} : {})}
    />;
  });
  
ViewWin32.displayName = 'ViewWin32';

module.exports = ViewWin32;
