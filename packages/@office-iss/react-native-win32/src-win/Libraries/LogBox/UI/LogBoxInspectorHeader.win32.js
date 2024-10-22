/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @flow strict-local
 * @format
 */

'use strict';

import type {LogLevel} from '../Data/LogBoxLog';

import StatusBar from '../../Components/StatusBar/StatusBar';
import View from '../../Components/View/View';
import StyleSheet from '../../StyleSheet/StyleSheet';
import Text from '../../Text/Text';
import Platform from '../../Utilities/Platform';
import LogBoxInspectorHeaderButton from './LogBoxInspectorHeaderButton';
import * as LogBoxStyle from './LogBoxStyle';
import * as React from 'react';

type Props = $ReadOnly<{
  onSelectIndex: (selectedIndex: number) => void,
  selectedIndex: number,
  total: number,
  level: LogLevel,
}>;

export default function LogBoxInspectorHeader(props: Props): React.Node {
  if (props.level === 'syntax') {
    return (
      <View style={[styles.safeArea, styles[props.level]]}>
        <View style={styles.header}>
          <View style={styles.title}>
            <Text style={styles.titleText}>Failed to compile</Text>
          </View>
        </View>
      </View>
    );
  }

  const prevIndex =
    props.selectedIndex - 1 < 0 ? props.total - 1 : props.selectedIndex - 1;
  const nextIndex =
    props.selectedIndex + 1 > props.total - 1 ? 0 : props.selectedIndex + 1;

  const titleText = `Log ${props.selectedIndex + 1} of ${props.total}`;

  return (
    <View style={[styles.safeArea, styles[props.level]]}>
      <View style={styles.header}>
        <LogBoxInspectorHeaderButton
          disabled={props.total <= 1}
          level={props.level}
          image={new String('←')}
          onPress={() => props.onSelectIndex(prevIndex)}
        />
        <View style={styles.title}>
          <Text style={styles.titleText}>{titleText}</Text>
        </View>
        <LogBoxInspectorHeaderButton
          disabled={props.total <= 1}
          level={props.level}
          image={new String('→')}
          onPress={() => props.onSelectIndex(nextIndex)}
        />
      </View>
    </View>
  );
}

const backgroundForLevel = (level: LogLevel) =>
  ({
    warn: {
      default: 'transparent',
      pressed: LogBoxStyle.getWarningDarkColor(),
    },
    error: {
      default: 'transparent',
      pressed: LogBoxStyle.getErrorDarkColor(),
    },
    fatal: {
      default: 'transparent',
      pressed: LogBoxStyle.getFatalDarkColor(),
    },
    syntax: {
      default: 'transparent',
      pressed: LogBoxStyle.getFatalDarkColor(),
    },
  })[level];

const styles = StyleSheet.create({
  syntax: {
    backgroundColor: LogBoxStyle.getFatalColor(),
  },
  fatal: {
    backgroundColor: LogBoxStyle.getFatalColor(),
  },
  warn: {
    backgroundColor: LogBoxStyle.getWarningColor(),
  },
  error: {
    backgroundColor: LogBoxStyle.getErrorColor(),
  },
  header: {
    flexDirection: 'row',
    height: Platform.select({
      android: 48,
      ios: 44,
      // [Win32
      win32: 48,
      // Win32]
    }),
  },
  title: {
    alignItems: 'center',
    flex: 1,
    justifyContent: 'center',
  },
  titleText: {
    color: LogBoxStyle.getTextColor(),
    fontSize: 16,
    fontWeight: '600',
    includeFontPadding: false,
    lineHeight: 20,
  },
  safeArea: {
    paddingTop: Platform.OS === 'android' ? StatusBar.currentHeight : 40,
  },
});
