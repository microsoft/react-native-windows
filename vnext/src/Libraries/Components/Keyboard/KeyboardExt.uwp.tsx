// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
'use strict';

import * as React from 'react';
import { IKeyboardProps } from './KeyboardExtProps';

export const supportKeyboard = <P extends object>(WrappedComponent: React.ComponentType<P>) => {
  interface IForwardRefProps {
    // tslint:disable-next-line:no-any
    forwardedRef?: React.Ref<any>;
  }

  // children is used to avoid error: Property 'children' does not exist on type 'IntrinsicAttributes & ViewProps &
  // IKeyboardProps & RefAttributes<any>
  // tslint:disable-next-line:no-any
  type PropsWithoutForwardedRef = P & IKeyboardProps & {children?: any};
  type PropsWithForwardedRef = PropsWithoutForwardedRef & IForwardRefProps;

  class SupportKeyboard extends React.Component<PropsWithForwardedRef> {
    public render(): JSX.Element {
      const { forwardedRef, ...rest } = this.props;
      return <WrappedComponent ref={forwardedRef} {...rest as P} />;
    }
  }

  // tslint:disable-next-line:no-any
  return React.forwardRef((props: PropsWithoutForwardedRef, ref: React.Ref<any>) => {
    return <SupportKeyboard {...props} forwardedRef={ref} />;
  });
};
