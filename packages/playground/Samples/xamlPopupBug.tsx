/**
 * XAML Popup Positioning Bug Repro - Issue #15557
 * 
 * HOW TO REPRO:
 * 1. Run this sample in Playground
 * 2. SCROLL DOWN in the ScrollView
 * 3. Click on the ComboBox to open the dropdown popup
 * 4. BUG: The popup appears at the WRONG position!
 * 
 * The popup offset = how much you scrolled
 */

import React from 'react';
import {AppRegistry, ScrollView, View, Text, StyleSheet} from 'react-native';
import {ComboBox} from 'sample-custom-component';

const XamlPopupBugRepro = () => {
  const [selectedValue, setSelectedValue] = React.useState('(click to select)');

  return (
    <View style={styles.container}>
      {/* Header - Fixed at top */}
      <View style={styles.header}>
        <Text style={styles.title}>XAML Popup Bug Repro #15557</Text>
        <Text style={styles.subtitle}>Selected: {selectedValue}</Text>
      </View>

      {/* Instructions */}
      <View style={styles.instructions}>
        <Text style={styles.step}>1. SCROLL DOWN in the box below</Text>
        <Text style={styles.step}>2. Click a ComboBox to open dropdown</Text>
        <Text style={styles.step}>3. See the popup at WRONG position!</Text>
      </View>

      {/* Scrollable area with ComboBoxes */}
      <ScrollView style={styles.scrollView}>
        <View style={[styles.spacer, {backgroundColor: '#e74c3c'}]}>
          <Text style={styles.spacerText}>SCROLL DOWN</Text>
        </View>

        <View style={[styles.spacer, {backgroundColor: '#e67e22'}]}>
          <Text style={styles.spacerText}>Keep scrolling...</Text>
        </View>

        <View style={[styles.spacer, {backgroundColor: '#f1c40f'}]}>
          <Text style={styles.spacerText}>Almost there...</Text>
        </View>

        {/* First ComboBox */}
        <View style={styles.comboBoxContainer}>
          <Text style={styles.comboLabel}>ComboBox 1 - Click me!</Text>
          <ComboBox
            style={styles.comboBox}
            onSelectionChanged={(e) => {
              setSelectedValue(`CB1: ${e.nativeEvent.selectedValue}`);
            }}
          />
        </View>

        <View style={[styles.spacer, {backgroundColor: '#2ecc71'}]}>
          <Text style={styles.spacerText}>More space...</Text>
        </View>

        {/* Second ComboBox */}
        <View style={styles.comboBoxContainer}>
          <Text style={styles.comboLabel}>ComboBox 2 - Click me!</Text>
          <ComboBox
            style={styles.comboBox}
            onSelectionChanged={(e) => {
              setSelectedValue(`CB2: ${e.nativeEvent.selectedValue}`);
            }}
          />
        </View>

        <View style={[styles.spacer, {backgroundColor: '#3498db'}]}>
          <Text style={styles.spacerText}>End of content</Text>
        </View>
      </ScrollView>
    </View>
  );
};

const styles = StyleSheet.create({
  container: {
    flex: 1,
    backgroundColor: '#1a1a2e',
  },
  header: {
    padding: 20,
    backgroundColor: '#16213e',
    alignItems: 'center',
  },
  title: {
    fontSize: 24,
    fontWeight: 'bold',
    color: '#fff',
  },
  subtitle: {
    fontSize: 16,
    color: '#0f0',
    marginTop: 5,
  },
  instructions: {
    padding: 15,
    backgroundColor: '#0f3460',
  },
  step: {
    fontSize: 18,
    color: '#fff',
    marginVertical: 3,
  },
  scrollView: {
    flex: 1,
    margin: 10,
    borderWidth: 3,
    borderColor: '#e94560',
    borderRadius: 10,
  },
  spacer: {
    height: 200,
    justifyContent: 'center',
    alignItems: 'center',
    margin: 10,
    borderRadius: 10,
  },
  spacerText: {
    fontSize: 24,
    fontWeight: 'bold',
    color: '#fff',
  },
  comboBoxContainer: {
    margin: 10,
    padding: 15,
    backgroundColor: '#fff',
    borderRadius: 10,
    borderWidth: 3,
    borderColor: '#e94560',
  },
  comboLabel: {
    fontSize: 20,
    fontWeight: 'bold',
    marginBottom: 10,
    color: '#1a1a2e',
  },
  comboBox: {
    width: 350,
    height: 60,
  },
});

AppRegistry.registerComponent('Bootstrap', () => XamlPopupBugRepro);
export default XamlPopupBugRepro;
