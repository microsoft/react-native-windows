/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @flow strict-local
 * @format
 */

import type {RNTesterModule} from '../../types/RNTesterTypes';
import BasicExample from './FlatList-basic';
import OnEndReachedExample from './FlatList-onEndReached';
import ContentInsetExample from './FlatList-contentInset';
import InvertedExample from './FlatList-inverted';
import onViewableItemsChangedExample from './FlatList-onViewableItemsChanged';
import WithSeparatorsExample from './FlatList-withSeparators';
import MultiColumnExample from './FlatList-multiColumn';

export default ({
  framework: 'React',
  title: 'FlatList',
  category: 'ListView',
  documentationURL: 'https://reactnative.dev/docs/flatlist',
  description: 'Performant, scrollable list of data.',
  showIndividualExamples: true,
  examples: [
    BasicExample,
    OnEndReachedExample,
    ContentInsetExample,
    InvertedExample,
    onViewableItemsChangedExample,
    WithSeparatorsExample,
    MultiColumnExample,
  ],
}: RNTesterModule);
