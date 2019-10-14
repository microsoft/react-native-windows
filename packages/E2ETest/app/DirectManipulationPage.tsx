/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 */

import React, { useState } from 'react';
import { Text, View, StyleSheet, Button } from 'react-native';
import { MEASURE_IN_WINDOW_BUTTON, MEASURE_IN_WINDOW_RESULT } from './Consts';

const styles = StyleSheet.create({
    container: {
        padding: 20
    },
    text: {
        textAlign: 'center',
        fontWeight: '700',
        height: 30
    }
});

const viewRef = React.createRef<View>();

export function DirectManipulationTestPage() {
    const [measureInWindowState, setMeasureInWindowState] = useState('');

    return (
        <View ref={viewRef} style={styles.container}>
            <Text style={styles.text}>MeasureInWindowResult:
                <Text testID={MEASURE_IN_WINDOW_RESULT}>
                    {measureInWindowState}
                </Text>
            </Text>
            <Button title='Call MeasureInWindow'
                onPress={() => {
                    viewRef.current && viewRef.current.measureInWindow((x, y, width, height) => {
                        setMeasureInWindowState(`x=${x};y=${y};width=${width};height=${height}`);
                    })
                }}
                testID={MEASURE_IN_WINDOW_BUTTON} />

        </View >);
}