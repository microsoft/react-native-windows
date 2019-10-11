import { View, Text } from 'react-native'
import React, { useState } from 'react';

export function AccessibilityTestPage() {
    const [pressedCountNested, setPressedCountNested] = useState(0);
    const [pressedCount, setPressedCount] = useState(0);
    return (
        <View>
            <Text accessible={true} accessibilityLiveRegion="polite" style={{fontWeight: "bold"}}>
                I'm bold
                <Text style={{color: 'red'}} onPress={() => setPressedCountNested(pressedCountNested + 1)} accessible={true} accessibilityLiveRegion="polite">Pressed {pressedCountNested} times</Text>
            </Text>
            <Text style={{color: 'green'}} onPress={() => setPressedCount(pressedCount + 1)} accessible={true} accessibilityLiveRegion="polite">Pressed {pressedCount} times</Text>
        </View>)
}