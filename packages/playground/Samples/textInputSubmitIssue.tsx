/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

import React from 'react';
import {
  StyleSheet,
  Text,
  TextInput,
  View,
  ScrollView,
  Alert,
  AppRegistry,
} from 'react-native';

interface ChatEntryProps {
  submit: (value: string) => void;
}

function ChatEntry({submit}: ChatEntryProps) {
  const [value, setValue] = React.useState('');

  const submitValue = () => {
    // If the user hits submit but the text is empty, don't carry that forward
    if (value !== '') {
      submit(value);
      // Reset to a blank prompt - THIS IS THE BUG: setValue('') doesn't work in Fabric during onSubmitEditing
      setValue('');
    }
  };

  return (
    <View style={styles.containerRow}>
      <Text style={styles.label}>
        Manual setValue on submit (broken in Fabric):
      </Text>
      <TextInput
        style={styles.textInput}
        accessibilityLabel="Prompt input"
        multiline={true}
        placeholder="Type and press Enter - text should clear but doesn't in Fabric"
        onChangeText={newValue => setValue(newValue)}
        onSubmitEditing={submitValue}
        value={value}
        blurOnSubmit={false}
      />
    </View>
  );
}

function WorkingChatEntry({submit}: ChatEntryProps) {
  const [value, setValue] = React.useState('');

  const submitValue = () => {
    if (value !== '') {
      submit(value);
      // Manual clear - this is the workaround that should work
      setValue('');
    }
  };

  return (
    <View style={styles.containerRow}>
      <Text style={styles.label}>Workaround - manual clear after submit:</Text>
      <TextInput
        style={styles.textInput}
        accessibilityLabel="Working prompt input"
        multiline={true}
        placeholder="Type and press Enter - manual clear workaround"
        onChangeText={newValue => setValue(newValue)}
        onSubmitEditing={submitValue}
        value={value}
        blurOnSubmit={false}
      />
    </View>
  );
}

export default function TextInputSubmitIssue() {
  const [submissions, setSubmissions] = React.useState<string[]>([]);

  const handleSubmit = (text: string) => {
    setSubmissions(prev => [...prev, text]);
    Alert.alert('Submitted', `Text: "${text}"`);
  };

  return (
    <ScrollView
      contentInsetAdjustmentBehavior="automatic"
      style={styles.container}>
      <View style={styles.header}>
        <Text style={styles.title}>TextInput Submit Issue Reproduction</Text>
        <Text style={styles.description}>
          This demonstrates the bug in GitHub issue #15168 where setValue('')
          doesn't work during onSubmitEditing in Fabric architecture.
          {'\n\n'}
          Note: In the actual Windows implementation, this would use
          clearTextOnSubmit and submitKeyEvents props. This simplified version
          shows the core issue with programmatic value updates during submit.
        </Text>
      </View>

      <ChatEntry submit={handleSubmit} />
      <WorkingChatEntry submit={handleSubmit} />

      <View style={styles.submissionsList}>
        <Text style={styles.subtitle}>Submitted texts:</Text>
        {submissions.map((text, index) => (
          <Text key={index} style={styles.submissionItem}>
            {index + 1}. "{text}"
          </Text>
        ))}
      </View>
    </ScrollView>
  );
}

const styles = StyleSheet.create({
  container: {
    flex: 1,
    backgroundColor: '#f5f5f5',
  },
  header: {
    padding: 20,
    backgroundColor: '#ffffff',
    borderBottomWidth: 1,
    borderBottomColor: '#e0e0e0',
  },
  title: {
    fontSize: 24,
    fontWeight: 'bold',
    color: '#333333',
    marginBottom: 10,
  },
  description: {
    fontSize: 16,
    color: '#666666',
    lineHeight: 22,
  },
  containerRow: {
    margin: 20,
    padding: 15,
    backgroundColor: '#ffffff',
    borderRadius: 8,
    shadowColor: '#000',
    shadowOffset: {
      width: 0,
      height: 2,
    },
    shadowOpacity: 0.1,
    shadowRadius: 3.84,
    elevation: 5,
  },
  label: {
    fontSize: 16,
    fontWeight: '600',
    color: '#333333',
    marginBottom: 10,
  },
  textInput: {
    borderWidth: 1,
    borderColor: '#cccccc',
    borderRadius: 6,
    padding: 12,
    fontSize: 16,
    backgroundColor: '#ffffff',
    minHeight: 80,
    textAlignVertical: 'top',
  },
  submissionsList: {
    margin: 20,
    padding: 15,
    backgroundColor: '#ffffff',
    borderRadius: 8,
  },
  subtitle: {
    fontSize: 18,
    fontWeight: '600',
    color: '#333333',
    marginBottom: 10,
  },
  submissionItem: {
    fontSize: 14,
    color: '#666666',
    marginBottom: 5,
    paddingLeft: 10,
  },
});

AppRegistry.registerComponent('Bootstrap', () => TextInputSubmitIssue);
