/**
 * Copyright: Microsoft 2015
 *
 * RN-specific implementation of the cross-platform View abstraction.
 */
import React from 'react';
import { View } from 'react-native';
import { IViewWin32Props } from './ViewWin32.Props';
declare type ViewWin32Type = React.ForwardRefExoticComponent<IViewWin32Props & React.RefAttributes<View>> & View;
export declare const ViewWin32: React.ForwardRefExoticComponent<IViewWin32Props & React.RefAttributes<any>>;
export declare type ViewWin32 = ViewWin32Type;
export {};
