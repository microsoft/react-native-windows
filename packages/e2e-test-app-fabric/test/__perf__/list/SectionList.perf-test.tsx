/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * Performance tests for SectionList component.
 *
 * @format
 */

import * as React from 'react';
import {SectionList, Text, View, StyleSheet} from 'react-native';
import {
  ComponentPerfTestBase,
  measurePerf,
} from '@react-native-windows/perf-testing';
import type {IScenario, PerfMetrics} from '@react-native-windows/perf-testing';

type ItemType = {id: string; title: string};
type SectionType = {title: string; data: ItemType[]};

function generateSections(
  sectionCount: number,
  itemsPerSection: number,
): SectionType[] {
  return Array.from({length: sectionCount}, (_unused, s) => ({
    title: `Section ${s}`,
    data: Array.from({length: itemsPerSection}, (_item, i) => ({
      id: `${s}-${i}`,
      title: `Item ${s}-${i}`,
    })),
  }));
}

const sections3x5 = generateSections(3, 5);
const sections5x10 = generateSections(5, 10);
const sections10x20 = generateSections(10, 20);
const sections20x10 = generateSections(20, 10);

const renderItem = ({item}: {item: ItemType}) => (
  <View style={styles.item}>
    <Text>{item.title}</Text>
  </View>
);

const renderSectionHeader = ({section}: {section: SectionType}) => (
  <View style={styles.sectionHeader}>
    <Text style={styles.sectionHeaderText}>{section.title}</Text>
  </View>
);

const keyExtractor = (item: ItemType) => item.id;

class SectionListPerfTest extends ComponentPerfTestBase {
  readonly componentName = 'SectionList';
  readonly category = 'list' as const;
  readonly testId = 'perf-test-section-list';

  createComponent(props?: Record<string, unknown>): React.ReactElement {
    return (
      <SectionList
        testID={this.testId}
        sections={sections3x5}
        renderItem={renderItem}
        renderSectionHeader={renderSectionHeader}
        keyExtractor={keyExtractor}
        style={styles.default}
        {...props}
      />
    );
  }

  getCustomScenarios(): IScenario[] {
    return [
      {
        name: 'with-3-sections-5-items',
        description: 'SectionList with 3 sections × 5 items each',
        run: () => this.measureWithSections(sections3x5),
      },
      {
        name: 'with-5-sections-10-items',
        description: 'SectionList with 5 sections × 10 items each',
        run: () => this.measureWithSections(sections5x10),
      },
      {
        name: 'with-10-sections-20-items',
        description: 'SectionList with 10 sections × 20 items each (200 total)',
        run: () => this.measureWithSections(sections10x20),
      },
      {
        name: 'with-20-sections-10-items',
        description: 'SectionList with 20 sections × 10 items each (200 total)',
        run: () => this.measureWithSections(sections20x10),
      },
      {
        name: 'with-section-separator',
        description: 'SectionList with SectionSeparatorComponent',
        run: () => this.measureWithSectionSeparator(),
      },
      {
        name: 'with-item-separator',
        description: 'SectionList with ItemSeparatorComponent',
        run: () => this.measureWithItemSeparator(),
      },
      {
        name: 'with-header-footer',
        description:
          'SectionList with ListHeaderComponent and ListFooterComponent',
        run: () => this.measureWithHeaderFooter(),
      },
      {
        name: 'with-section-footer',
        description: 'SectionList with renderSectionFooter',
        run: () => this.measureWithSectionFooter(),
      },
      {
        name: 'with-sticky-section-headers',
        description: 'SectionList with stickySectionHeadersEnabled',
        run: () => this.measureWithStickySectionHeaders(),
      },
      {
        name: 'with-empty-list',
        description: 'SectionList with empty sections and ListEmptyComponent',
        run: () => this.measureWithEmptyList(),
      },
    ];
  }

  private async measureWithSections(
    sections: SectionType[],
  ): Promise<PerfMetrics> {
    const totalItems = sections.reduce((sum, s) => sum + s.data.length, 0);
    return measurePerf(
      <SectionList
        testID={this.testId}
        sections={sections}
        renderItem={renderItem}
        renderSectionHeader={renderSectionHeader}
        keyExtractor={keyExtractor}
        style={styles.default}
      />,
      {
        name: `${this.componentName} with-${sections.length}-sections-${totalItems}-items`,
        runs: 10,
      },
    );
  }

  private async measureWithSectionSeparator(): Promise<PerfMetrics> {
    return measurePerf(
      <SectionList
        testID={this.testId}
        sections={sections5x10}
        renderItem={renderItem}
        renderSectionHeader={renderSectionHeader}
        keyExtractor={keyExtractor}
        SectionSeparatorComponent={() => (
          <View style={styles.sectionSeparator} />
        )}
        style={styles.default}
      />,
      {name: `${this.componentName} with-section-separator`, runs: 10},
    );
  }

  private async measureWithItemSeparator(): Promise<PerfMetrics> {
    return measurePerf(
      <SectionList
        testID={this.testId}
        sections={sections5x10}
        renderItem={renderItem}
        renderSectionHeader={renderSectionHeader}
        keyExtractor={keyExtractor}
        ItemSeparatorComponent={() => <View style={styles.itemSeparator} />}
        style={styles.default}
      />,
      {name: `${this.componentName} with-item-separator`, runs: 10},
    );
  }

  private async measureWithHeaderFooter(): Promise<PerfMetrics> {
    return measurePerf(
      <SectionList
        testID={this.testId}
        sections={sections5x10}
        renderItem={renderItem}
        renderSectionHeader={renderSectionHeader}
        keyExtractor={keyExtractor}
        ListHeaderComponent={
          <View style={styles.listHeader}>
            <Text>List Header</Text>
          </View>
        }
        ListFooterComponent={
          <View style={styles.listFooter}>
            <Text>List Footer</Text>
          </View>
        }
        style={styles.default}
      />,
      {name: `${this.componentName} with-header-footer`, runs: 10},
    );
  }

  private async measureWithSectionFooter(): Promise<PerfMetrics> {
    return measurePerf(
      <SectionList
        testID={this.testId}
        sections={sections5x10}
        renderItem={renderItem}
        renderSectionHeader={renderSectionHeader}
        renderSectionFooter={({section}) => (
          <View style={styles.sectionFooter}>
            <Text>{`${section.data.length} items`}</Text>
          </View>
        )}
        keyExtractor={keyExtractor}
        style={styles.default}
      />,
      {name: `${this.componentName} with-section-footer`, runs: 10},
    );
  }

  private async measureWithStickySectionHeaders(): Promise<PerfMetrics> {
    return measurePerf(
      <SectionList
        testID={this.testId}
        sections={sections5x10}
        renderItem={renderItem}
        renderSectionHeader={renderSectionHeader}
        keyExtractor={keyExtractor}
        stickySectionHeadersEnabled={true}
        style={styles.default}
      />,
      {name: `${this.componentName} with-sticky-section-headers`, runs: 10},
    );
  }

  private async measureWithEmptyList(): Promise<PerfMetrics> {
    return measurePerf(
      <SectionList
        testID={this.testId}
        sections={[]}
        renderItem={renderItem}
        renderSectionHeader={renderSectionHeader}
        keyExtractor={keyExtractor}
        ListEmptyComponent={
          <View style={styles.empty}>
            <Text>No sections to display</Text>
          </View>
        }
        style={styles.default}
      />,
      {name: `${this.componentName} with-empty-list`, runs: 10},
    );
  }
}

const sectionListPerfTest = new SectionListPerfTest();

describe('SectionList Performance', () => {
  test('mount time', async () => {
    const perf = await sectionListPerfTest.measureMount();
    expect(perf).toMatchPerfSnapshot();
  });

  test('unmount time', async () => {
    const perf = await sectionListPerfTest.measureUnmount();
    expect(perf).toMatchPerfSnapshot();
  });

  test('rerender time', async () => {
    const perf = await sectionListPerfTest.measureRerender();
    expect(perf).toMatchPerfSnapshot();
  });

  describe('SectionList-Specific Scenarios', () => {
    const scenarios = sectionListPerfTest.getCustomScenarios();

    test('with-3-sections-5-items', async () => {
      const perf = await scenarios[0].run();
      expect(perf).toMatchPerfSnapshot();
    });

    test('with-5-sections-10-items', async () => {
      const perf = await scenarios[1].run();
      expect(perf).toMatchPerfSnapshot({minAbsoluteDelta: 5});
    });

    test('with-10-sections-20-items', async () => {
      const perf = await scenarios[2].run();
      expect(perf).toMatchPerfSnapshot({
        maxDurationIncrease: 15,
        minAbsoluteDelta: 10,
      });
    });

    test('with-20-sections-10-items', async () => {
      const perf = await scenarios[3].run();
      expect(perf).toMatchPerfSnapshot({
        maxDurationIncrease: 15,
        minAbsoluteDelta: 10,
      });
    });

    test('with-section-separator', async () => {
      const perf = await scenarios[4].run();
      expect(perf).toMatchPerfSnapshot({minAbsoluteDelta: 5});
    });

    test('with-item-separator', async () => {
      const perf = await scenarios[5].run();
      expect(perf).toMatchPerfSnapshot({minAbsoluteDelta: 5});
    });

    test('with-header-footer', async () => {
      const perf = await scenarios[6].run();
      expect(perf).toMatchPerfSnapshot({minAbsoluteDelta: 5});
    });

    test('with-section-footer', async () => {
      const perf = await scenarios[7].run();
      expect(perf).toMatchPerfSnapshot({minAbsoluteDelta: 5});
    });

    test('with-sticky-section-headers', async () => {
      const perf = await scenarios[8].run();
      expect(perf).toMatchPerfSnapshot({minAbsoluteDelta: 5});
    });

    test('with-empty-list', async () => {
      const perf = await scenarios[9].run();
      expect(perf).toMatchPerfSnapshot();
    });
  });
});

const styles = StyleSheet.create({
  default: {
    height: 300,
  },
  item: {
    padding: 10,
    borderBottomWidth: 1,
    borderBottomColor: '#ccc',
  },
  sectionHeader: {
    padding: 8,
    backgroundColor: '#f0f0f0',
  },
  sectionHeaderText: {
    fontWeight: 'bold',
  },
  sectionFooter: {
    padding: 6,
    backgroundColor: '#fafafa',
  },
  sectionSeparator: {
    height: 2,
    backgroundColor: '#999',
  },
  itemSeparator: {
    height: 1,
    backgroundColor: '#ddd',
  },
  listHeader: {
    padding: 10,
    backgroundColor: '#e0e0e0',
  },
  listFooter: {
    padding: 10,
    backgroundColor: '#e0e0e0',
  },
  empty: {
    padding: 20,
    alignItems: 'center',
  },
});
