/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 * @flow
 */

'use strict';

// Temporary test example for UseExperimentalWinUI3=true
// Remove when we get react-native-xaml working well for Fabric

import React, {useState} from 'react';
import {Alert, Button, Text, View, ScrollView} from 'react-native';
import CalendarView from './FabricXamlCalendarViewNativeComponent';

const XamlContentExample = () => {
  const [selectedDate, setSelectedDate] = useState(true);

  return (
    <ScrollView>
      <View
        style={{
          margin: 20,
          flexDirection: 'column',
          gap: 5,
        }}>
        <Button
          title="Before Button"
          onPress={() => Alert.alert('Before button pressed')}
        />
        <Text>Xaml CalendarView control. Selected date: {selectedDate}</Text>
        <CalendarView
          style={{
            width: 400,
            height: 400,
            minWidth: 400,
            minHeight: 400,
          }}
          onSelectedDatesChanged={e => {
            setSelectedDate(e.nativeEvent.startDate);
          }}
        />
        <Button
          title="After Button"
          onPress={() => Alert.alert('After button pressed')}
        />
      </View>
    </ScrollView>
  );
};

exports.displayName = 'XamlFabricExample';
exports.framework = 'React';
exports.category = 'UI';
exports.title = 'Xaml WinUI3 (Experimental, for Fabric)';
exports.description =
  'Requires UseExperimentalWinUI3 - Xaml content that works in Fabric';

exports.examples = [
  {
    title: 'Xaml WinUI3 (Experimental, for Fabric)',
    render: function (): React.Node {
      return <XamlContentExample />;
    },
  },
];
