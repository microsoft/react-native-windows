#!/usr/bin/env node
/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * Interactive CLI to scaffold a new component performance test.
 *
 * Usage:
 *   node vnext/Scripts/perf/create-perf-test.js
 *   yarn perf:create           (from e2e-test-app-fabric)
 *
 * Generates a `.perf-test.tsx` file with:
 *   - Class extending ComponentPerfTestBase
 *   - Standard mount / unmount / rerender tests
 *   - Placeholder for custom scenarios
 *   - Correct imports, test ID, and category
 *
 * @format
 */

const fs = require('fs');
const path = require('path');
const readline = require('readline');

// ─── Helpers ───

function ask(rl, question, defaultValue) {
  const suffix = defaultValue ? ` (${defaultValue})` : '';
  return new Promise(resolve => {
    rl.question(`${question}${suffix}: `, answer => {
      resolve(answer.trim() || defaultValue || '');
    });
  });
}

function choose(rl, question, options, defaultIndex = 0) {
  return new Promise(resolve => {
    console.log(`\n${question}`);
    options.forEach((opt, i) => {
      const marker = i === defaultIndex ? ' (default)' : '';
      console.log(`  ${i + 1}. ${opt}${marker}`);
    });
    rl.question(`Choice [${defaultIndex + 1}]: `, answer => {
      const idx = parseInt(answer, 10) - 1;
      resolve(
        idx >= 0 && idx < options.length
          ? options[idx]
          : options[defaultIndex],
      );
    });
  });
}

function toPascalCase(str) {
  return str
    .replace(/[-_\s]+(.)?/g, (_, c) => (c ? c.toUpperCase() : ''))
    .replace(/^(.)/, (_, c) => c.toUpperCase());
}

function toKebabCase(str) {
  return str
    .replace(/([a-z])([A-Z])/g, '$1-$2')
    .replace(/[\s_]+/g, '-')
    .toLowerCase();
}

/**
 * Split a props string into individual prop tokens,
 * correctly handling nested braces like onPress={() => {}}.
 */
function splitProps(propsStr) {
  const props = [];
  let current = '';
  let braceDepth = 0;
  let inString = null; // '"' or "'"

  for (let i = 0; i < propsStr.length; i++) {
    const ch = propsStr[i];

    // Track string boundaries
    if ((ch === '"' || ch === "'") && !inString) {
      inString = ch;
    } else if (ch === inString) {
      inString = null;
    }

    if (!inString) {
      if (ch === '{') braceDepth++;
      if (ch === '}') braceDepth--;
    }

    // Split on whitespace only when outside braces/strings
    if (ch === ' ' && braceDepth === 0 && !inString) {
      if (current.trim()) props.push(current.trim());
      current = '';
    } else {
      current += ch;
    }
  }
  if (current.trim()) props.push(current.trim());
  return props;
}

// ─── Categories & their output directories ───

const CATEGORIES = ['core', 'extended', 'interactive', 'list', 'community'];

const PERF_TEST_ROOT = path.resolve(
  __dirname,
  '../../..',
  'packages/e2e-test-app-fabric/test/__perf__',
);

// Known required props for common RN components.
// These are auto-applied so the generated test works out of the box.
const KNOWN_REQUIRED_PROPS = {
  Button: 'title="Perf Test" onPress={() => {}}',
  Switch: 'value={false}',
  Image: 'source={{uri: "https://example.com/img.png"}}',
  FlatList: 'data={[]} renderItem={() => null}',
  SectionList: 'sections={[]} renderItem={() => null}',
  Modal: 'visible={true}',
  TouchableHighlight: 'onPress={() => {}}',
  TouchableOpacity: 'onPress={() => {}}',
  Pressable: 'onPress={() => {}}',
};

// ─── Template ───

function generateTestFile({
  componentName,
  pascalName,
  kebabName,
  importPath,
  category,
  hasChildren,
  childrenText,
  requiredProps,
}) {
  const testId = `perf-test-${kebabName}`;
  const instanceName = `${pascalName.charAt(0).toLowerCase() + pascalName.slice(1)}PerfTest`;

  // Build the createComponent body
  const allProps = [`testID={this.testId}`];
  if (requiredProps) {
    // Parse individual JSX props, handling nested braces like {() => {}}
    allProps.push(...splitProps(requiredProps));
  }
  allProps.push('style={styles.default}', '{...props}');

  let createComponentBody;
  if (hasChildren) {
    const propLines = allProps.map(p => `        ${p}`).join('\n');
    createComponentBody = `    return (\n      <${componentName}\n${propLines}>\n        ${childrenText}\n      </${componentName}>\n    );`;
  } else if (allProps.length > 3) {
    // Multi-line JSX when there are many props
    const propLines = allProps.map(p => `        ${p}`).join('\n');
    createComponentBody = `    return (\n      <${componentName}\n${propLines}\n      />\n    );`;
  } else {
    createComponentBody = `    return <${componentName} ${allProps.join(' ')} />;`;
  }

  return `/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * Performance tests for ${componentName} component.
 *
 * @format
 */

import * as React from 'react';
import {${componentName}, StyleSheet} from '${importPath}';
import {
  ComponentPerfTestBase,
  measurePerf,
} from '@react-native-windows/perf-testing';
import type {IScenario, PerfMetrics} from '@react-native-windows/perf-testing';

class ${pascalName}PerfTest extends ComponentPerfTestBase {
  readonly componentName = '${componentName}';
  readonly category = '${category}' as const;
  readonly testId = '${testId}';

  createComponent(props?: Record<string, unknown>): React.ReactElement {
${createComponentBody}
  }

  getCustomScenarios(): IScenario[] {
    // TODO: Add component-specific scenarios here.
    // Example:
    // return [
    //   {
    //     name: 'custom-scenario',
    //     description: 'Describe what this measures',
    //     run: () => this.measureCustomScenario(),
    //   },
    // ];
    return [];
  }

  // Example custom scenario method – uncomment and adapt:
  // private async measureCustomScenario(): Promise<PerfMetrics> {
  //   return measurePerf(
  //     <${componentName} testID={this.testId} style={styles.default} />,
  //     {
  //       name: \`\${this.componentName} custom-scenario\`,
  //       runs: 10,
  //     },
  //   );
  // }
}

// ─── TEST SUITE ───

const ${instanceName} = new ${pascalName}PerfTest();

describe('${componentName} Performance', () => {
  test('mount time', async () => {
    const perf = await ${instanceName}.measureMount();
    expect(perf).toMatchPerfSnapshot();
  });

  test('unmount time', async () => {
    const perf = await ${instanceName}.measureUnmount();
    expect(perf).toMatchPerfSnapshot();
  });

  test('rerender time', async () => {
    const perf = await ${instanceName}.measureRerender();
    expect(perf).toMatchPerfSnapshot();
  });

  // Uncomment when you add custom scenarios above:
  // describe('${componentName}-Specific Scenarios', () => {
  //   test('custom-scenario', async () => {
  //     const scenario = ${instanceName}.getCustomScenarios()[0];
  //     const perf = await scenario.run();
  //     expect(perf).toMatchPerfSnapshot();
  //   });
  // });
});

const styles = StyleSheet.create({
  default: {
    // Add base styles for your component here
  },
});
`;
}

// ─── Main ───

async function main() {
  const rl = readline.createInterface({
    input: process.stdin,
    output: process.stdout,
  });

  console.log('\n🧪  React Native Windows — Performance Test Generator\n');
  console.log('This will create a new .perf-test.tsx file with mount,');
  console.log('unmount, and rerender tests for your component.\n');
  console.log('Examples:');
  console.log('  Component: ScrollView   | Import: react-native | Category: core     | Children: yes');
  console.log('  Component: Button       | Import: react-native | Category: core     | Children: no');
  console.log('  Component: Image        | Import: react-native | Category: core     | Children: no');
  console.log('  Component: Pressable    | Import: react-native | Category: interactive | Children: yes');
  console.log('  Component: Slider       | Import: @react-native-community/slider | Category: community | Children: no');
  console.log('');

  // 1. Component name
  const rawName = await ask(rl, 'Component name (e.g. ScrollView, Image, Pressable)');
  if (!rawName) {
    console.error('❌  Component name is required.');
    rl.close();
    process.exit(1);
  }

  const pascalName = toPascalCase(rawName);
  const kebabName = toKebabCase(rawName);

  // 2. Import path
  const importPath = await ask(
    rl,
    'Import path for the component',
    'react-native',
  );

  // 3. Category
  const category = await choose(
    rl,
    'Select component category:',
    CATEGORIES,
    0,
  );

  // 4. Does the component have children?
  const hasChildrenAnswer = await ask(
    rl,
    'Does this component render children? (y/N)',
    'N',
  );
  const hasChildren = hasChildrenAnswer.toLowerCase() === 'y';
  let childrenText = 'Sample Content';
  if (hasChildren) {
    const customChildren = await ask(
      rl,
      'Children text/content',
      'Sample Content',
    );
    childrenText = customChildren;
  }

  // 5. Required props (auto-detect from known list, or ask)
  let requiredProps = KNOWN_REQUIRED_PROPS[rawName] || '';
  if (requiredProps) {
    console.log(`\n  Auto-detected required props: ${requiredProps}`);
    const override = await ask(
      rl,
      'Required props (Enter to accept, or type custom)',
      requiredProps,
    );
    requiredProps = override;
  } else {
    const customProps = await ask(
      rl,
      'Required props (e.g. title="Test" onPress={() => {}}) – leave blank if none',
      '',
    );
    requiredProps = customProps;
  }

  rl.close();

  // 5. Determine output path
  const categoryDir = path.join(PERF_TEST_ROOT, category);
  if (!fs.existsSync(categoryDir)) {
    fs.mkdirSync(categoryDir, {recursive: true});
    console.log(`\n📁  Created directory: ${path.relative(process.cwd(), categoryDir)}`);
  }

  const outputFile = path.join(categoryDir, `${pascalName}.perf-test.tsx`);
  if (fs.existsSync(outputFile)) {
    console.error(`\n❌  File already exists: ${path.relative(process.cwd(), outputFile)}`);
    console.error('    Delete it first or choose a different name.');
    process.exit(1);
  }

  // 6. Generate and write
  const content = generateTestFile({
    componentName: rawName,
    pascalName,
    kebabName,
    importPath,
    category,
    hasChildren,
    childrenText,
    requiredProps,
  });

  fs.writeFileSync(outputFile, content, 'utf8');

  const relPath = path.relative(process.cwd(), outputFile);
  console.log(`\n✅  Created: ${relPath}`);
  console.log(`\nNext steps:`);
  console.log(`  1. Review and customize the generated test file`);
  console.log(`  2. Add any component-specific custom scenarios`);
  console.log(`  3. Run:  yarn perf --testPathPattern=${pascalName}`);
  console.log(`  4. Generate baseline:  yarn perf:update --testPathPattern=${pascalName}`);
  console.log('');
}

main().catch(err => {
  console.error('Error:', err);
  process.exit(1);
});
