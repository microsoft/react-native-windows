import { Text } from 'react-native'
import React, { useState } from 'react';

export function AccessibilityTestPage() {
    const [pressedCount, setPressedCount] = useState(0);
    return (
        <Text onPress={() => setPressedCount(pressedCount + 1)} accessible={true} accessibilityLiveRegion="polite">Pressed {pressedCount} times</Text>)
}