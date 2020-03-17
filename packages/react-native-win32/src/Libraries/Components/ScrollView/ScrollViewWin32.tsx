import * as React from 'react';
import RN = require('react-native');
import { IScrollViewWin32Props } from './ScrollViewWin32.Props';
import { UseFrom } from '../View/ViewWin32.Props';

/**
 * React-native <Text> control with additional Win32-specific functionality.
 */

type InnerViewProps = UseFrom<IScrollViewWin32Props, RN.ScrollViewProps, 'accessibilityRole'> &
    UseFrom<IScrollViewWin32Props, RN.ScrollViewProps, 'accessibilityStates'> &
    UseFrom<IScrollViewWin32Props, RN.ScrollViewProps, 'accessibilityActions'>;

export class ScrollViewWin32 extends React.Component<IScrollViewWin32Props, {}> {
    constructor(props: IScrollViewWin32Props) {
        super(props);
    }

    public render() {
        return <RN.ScrollView {...(this.props as InnerViewProps)} />;
    }
}
