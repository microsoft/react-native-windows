/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 */

import { View, Text, ViewProps, NativeMethodsMixin, Constructor } from 'react-native'
import React, { useState } from 'react';

// MyView is a workaround. Currently in typescript, accessibilityAction doesn't allow name to be any string.
declare class MyViewComponent extends React.Component<ViewProps & { accessibilityActions: any, onAccessibilityAction: any }> { }
declare const MyViewBase: Constructor<NativeMethodsMixin> & typeof MyViewComponent;
export class MyView extends MyViewBase {
}

export function AccessibilityTestPage() {
    const [pressedCountNested, setPressedCountNested] = useState(0);
    const [pressedCount, setPressedCount] = useState(0);
    const [accessibilityAction, setAccessibilityAction] = useState('');

    return (
        <View>
            <View>
                <Text accessible={true} accessibilityLiveRegion="polite" style={{ fontWeight: "bold" }}>
                    I'm bold
                <Text style={{ color: 'red' }} onPress={() => setPressedCountNested(pressedCountNested + 1)} accessible={true} accessibilityLiveRegion="polite">Pressed {pressedCountNested} times</Text>
                </Text>
                <Text style={{ color: 'green' }} onPress={() => setPressedCount(pressedCount + 1)} accessible={true} accessibilityLiveRegion="polite">Pressed {pressedCount} times</Text>
            </View>
            <MyView
                accessible={true}
                accessibilityActions={[
                    { name: 'toggle', label: 'toggle' },
                    { name: 'invoke', label: 'invoke' },
                    { name: 'expand', label: 'expand' },
                    { name: 'collapse', label: 'collapseIt' },
                ]}
                onAccessibilityAction={(event: { nativeEvent: { actionName: any; }; }) => {
                    switch (event.nativeEvent.actionName) {
                        case 'toggle':
                            setAccessibilityAction('toggle action success');
                            break;
                        case 'invoke':
                            setAccessibilityAction('invoke action success');
                            break;
                        case 'expand':
                            setAccessibilityAction('expand action success');
                            break;
                        case 'collapseIt':
                            setAccessibilityAction('collapseIt action success');
                            break;
                    }
                }}
            >
                <Text>accessibilityAction:{accessibilityAction}</Text>
            </MyView>
        </View>)
}