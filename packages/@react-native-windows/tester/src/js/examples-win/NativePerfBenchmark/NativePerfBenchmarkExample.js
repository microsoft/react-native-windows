/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

'use strict';

const React = require('react');
const {
  View,
  Text,
  TextInput,
  Image,
  ScrollView,
  FlatList,
  SectionList,
  Switch,
  ActivityIndicator,
  Button,
  Modal,
  Pressable,
  TouchableHighlight,
  TouchableOpacity,
  StyleSheet,
} = require('react-native');

const {useState, useRef, useCallback, useEffect} = React;

const PHASE_IDLE = 'idle';
const PHASE_CLEARING = 'clearing';
const PHASE_MOUNTING = 'mounting';
const PHASE_DONE = 'done';

const COMPONENT_REGISTRY = {
  View: () => <View style={styles.target} />,
  Text: () => <Text style={styles.target}>Benchmark Text</Text>,
  TextInput: () => (
    <TextInput style={styles.targetInput} placeholder="Benchmark" />
  ),
  Image: () => (
    <Image
      style={styles.targetImage}
      source={{uri: 'https://reactnative.dev/img/tiny_logo.png'}}
    />
  ),
  ScrollView: () => (
    <ScrollView style={styles.target}>
      {Array.from({length: 20}, (_, i) => (
        <View key={i} style={styles.scrollItem} />
      ))}
    </ScrollView>
  ),
  FlatList: () => (
    <FlatList
      style={styles.target}
      data={Array.from({length: 50}, (_, i) => ({key: String(i)}))}
      renderItem={({item}) => <Text>{item.key}</Text>}
    />
  ),
  SectionList: () => (
    <SectionList
      style={styles.target}
      sections={[
        {title: 'A', data: ['A1', 'A2', 'A3']},
        {title: 'B', data: ['B1', 'B2', 'B3']},
      ]}
      renderItem={({item}) => <Text>{item}</Text>}
      renderSectionHeader={({section}) => <Text>{section.title}</Text>}
    />
  ),
  Switch: () => <Switch value={false} />,
  ActivityIndicator: () => <ActivityIndicator size="large" />,
  Button: () => <Button title="Benchmark" onPress={() => {}} />,
  Modal: () => (
    <Modal visible={false} transparent>
      <View style={styles.target} />
    </Modal>
  ),
  Pressable: () => (
    <Pressable style={styles.target}>
      <Text>Press</Text>
    </Pressable>
  ),
  TouchableHighlight: () => (
    <TouchableHighlight style={styles.target} onPress={() => {}}>
      <Text>Highlight</Text>
    </TouchableHighlight>
  ),
  TouchableOpacity: () => (
    <TouchableOpacity style={styles.target} onPress={() => {}}>
      <Text>Opacity</Text>
    </TouchableOpacity>
  ),
};

function BenchmarkRunner() {
  const [componentName, setComponentName] = useState('View');
  const [runsInput, setRunsInput] = useState('15');
  const [phase, setPhase] = useState(PHASE_IDLE);
  const [showTarget, setShowTarget] = useState(false);
  const [resultsJson, setResultsJson] = useState('');

  const durationsRef = useRef([]);
  const runIndexRef = useRef(0);
  const totalRunsRef = useRef(15);
  const markNameRef = useRef('');

  const finishRun = useCallback(() => {
    const markEnd = `perf-end-${runIndexRef.current}`;
    performance.mark(markEnd);
    try {
      const measure = performance.measure(
        `perf-run-${runIndexRef.current}`,
        markNameRef.current,
        markEnd,
      );
      durationsRef.current.push(measure.duration);
    } catch (_) {}
    performance.clearMarks(markNameRef.current);
    performance.clearMarks(markEnd);
    performance.clearMeasures(`perf-run-${runIndexRef.current}`);

    runIndexRef.current++;
    if (runIndexRef.current < totalRunsRef.current) {
      setPhase(PHASE_CLEARING);
    } else {
      setShowTarget(false);
      setResultsJson(
        JSON.stringify({
          componentName,
          runs: durationsRef.current.length,
          durations: durationsRef.current,
        }),
      );
      setPhase(PHASE_DONE);
    }
  }, [componentName]);

  useEffect(() => {
    if (phase === PHASE_CLEARING) {
      setShowTarget(false);
      requestAnimationFrame(() => {
        setPhase(PHASE_MOUNTING);
      });
    }
  }, [phase]);

  useEffect(() => {
    if (phase === PHASE_MOUNTING) {
      const markStart = `perf-start-${runIndexRef.current}`;
      markNameRef.current = markStart;
      performance.mark(markStart);
      setShowTarget(true);
    }
  }, [phase]);

  useEffect(() => {
    if (phase === PHASE_MOUNTING && showTarget) {
      requestAnimationFrame(() => {
        finishRun();
      });
    }
  }, [phase, showTarget, finishRun]);

  const handleRun = useCallback(() => {
    const runs = parseInt(runsInput, 10) || 15;
    totalRunsRef.current = runs;
    runIndexRef.current = 0;
    durationsRef.current = [];
    setResultsJson('');
    setPhase(PHASE_CLEARING);
  }, [runsInput]);

  const ComponentFactory = COMPONENT_REGISTRY[componentName];

  return (
    <View style={styles.container}>
      <View style={styles.controls}>
        <TextInput
          testID="perf-component-input"
          style={styles.input}
          value={componentName}
          onChangeText={setComponentName}
          placeholder="Component name"
        />
        <TextInput
          testID="perf-runs-input"
          style={styles.input}
          value={runsInput}
          onChangeText={setRunsInput}
          keyboardType="numeric"
          placeholder="Runs"
        />
        <Pressable
          testID="perf-run-btn"
          style={styles.button}
          onPress={handleRun}
          disabled={phase !== PHASE_IDLE && phase !== PHASE_DONE}>
          <Text style={styles.buttonText}>Run Benchmark</Text>
        </Pressable>
      </View>

      <Text testID="perf-status" style={styles.status}>
        {phase}
      </Text>

      <View style={styles.targetContainer}>
        {showTarget && ComponentFactory ? <ComponentFactory /> : null}
      </View>

      <Text testID="perf-results" style={styles.results}>
        {resultsJson}
      </Text>
    </View>
  );
}

const styles = StyleSheet.create({
  container: {flex: 1, padding: 8},
  controls: {flexDirection: 'row', gap: 8, marginBottom: 8},
  input: {
    borderWidth: 1,
    borderColor: '#ccc',
    padding: 6,
    minWidth: 100,
    fontSize: 14,
  },
  button: {
    backgroundColor: '#0078D4',
    paddingHorizontal: 16,
    paddingVertical: 8,
    borderRadius: 4,
    justifyContent: 'center',
  },
  buttonText: {color: 'white', fontWeight: 'bold'},
  status: {fontSize: 12, color: '#666', marginBottom: 4},
  targetContainer: {
    minHeight: 100,
    borderWidth: 1,
    borderColor: '#eee',
    marginBottom: 8,
  },
  target: {width: 80, height: 80, backgroundColor: '#f0f0f0'},
  targetInput: {width: 200, height: 40, borderWidth: 1, borderColor: '#999'},
  targetImage: {width: 80, height: 80},
  scrollItem: {height: 20, backgroundColor: '#ddd', marginBottom: 2},
  results: {fontSize: 10, fontFamily: 'monospace', color: '#333'},
});

exports.displayName = 'NativePerfBenchmarkExample';
exports.framework = 'React';
exports.category = 'Basic';
exports.title = 'Native Perf Benchmark';
exports.description =
  'Measures native rendering pipeline via performance.mark/measure.';

exports.examples = [
  {
    title: 'Benchmark Runner',
    render: function () {
      return <BenchmarkRunner />;
    },
  },
];
