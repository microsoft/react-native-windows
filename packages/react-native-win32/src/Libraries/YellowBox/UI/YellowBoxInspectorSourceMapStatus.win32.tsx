'use strict';
import React = require('react');
import { Text } from 'react-native';

interface IYellowBoxInspectorSourceMapStatusProps {
  status: 'COMPLETE' | 'FAILED' | 'NONE' | 'PENDING';
}

export = (props: IYellowBoxInspectorSourceMapStatusProps) => <Text>{props.status}</Text>;
