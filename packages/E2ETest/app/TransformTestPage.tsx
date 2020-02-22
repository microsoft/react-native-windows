/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 */

import React, { useState } from 'react';
import { Text, View, StyleSheet, Button, findNodeHandle } from 'react-native';
import { APPLY_SCALE_TRANSFORM_BUTTON, MEASURE_LAYOUT_BUTTON, TRANSFORM_TEST_RESULT } from './Consts';

const styles = StyleSheet.create({
    text: {
        textAlign: 'center',
        fontWeight: '700',
        height: 30
    },
    childView: {
        width: 60,
        height: 60,
        marginTop: 50,
        backgroundColor: 'lightgreen',
        marginBottom: 20,
    }
});

const rootViewRef = React.createRef<View>();
const childViewRef = React.createRef<View>();

export function TransformTestPage() {
    const [resultTextState, setResultTextState] = useState('');
    const [viewScale, setViewScale] = useState(1);

    const measureLayoutSucceeded = (x: number, y: number, width: number, height: number) => {
        setResultTextState(`x=${x};y=${y};width=${Math.trunc(width)};height=${Math.trunc(height)}`);
    }

    const measureLayoutFailed = () => {
        setResultTextState('MeasureLayout failed');
    }

    return (
        <View ref={rootViewRef}>
            <Text testID={TRANSFORM_TEST_RESULT}>
                {resultTextState}
            </Text>
            <View style={[styles.childView, { transform: [{ scale: viewScale }] }]} ref={childViewRef}></View>
            <Button title='Apply ScaleTransform'
                onPress={() => {
                    setViewScale(viewScale === 1 ? 0.5 : 1);
                }}
                testID={APPLY_SCALE_TRANSFORM_BUTTON} />

            <Button title='MeasureLayout'
                onPress={() => {
                    if (childViewRef.current) {
                        const rootViewHandle = findNodeHandle(rootViewRef.current);
                        if (rootViewHandle) {
                            childViewRef.current.measureLayout(rootViewHandle, measureLayoutSucceeded, measureLayoutFailed);
                        }
                    }
                }}
                testID={MEASURE_LAYOUT_BUTTON} />

        </View >);
}