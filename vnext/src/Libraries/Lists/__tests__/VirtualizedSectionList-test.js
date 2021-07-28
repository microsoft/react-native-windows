/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @format
 * @flow
 * @emails oncall+react_native
 */

'use strict';

const React = require('react');
const ReactTestRenderer = require('react-test-renderer');

const VirtualizedSectionList = require('../VirtualizedSectionList');

describe('VirtualizedSectionList', () => {
  it('renders simple list', () => {
    const component = ReactTestRenderer.create(
      <VirtualizedSectionList
        sections={[
          {title: 's1', data: [{key: 'i1'}, {key: 'i2'}, {key: 'i3'}]},
        ]}
        renderItem={({item}) => <item value={item.key} />}
        getItem={(data, key) => data[key]}
        getItemCount={data => data.length}
      />,
    );
    expect(component).toMatchSnapshot();
  });

  it('renders empty list', () => {
    const component = ReactTestRenderer.create(
      <VirtualizedSectionList
        sections={[]}
        renderItem={({item}) => <item value={item.key} />}
        getItem={(data, key) => data[key]}
        getItemCount={data => data.length}
      />,
    );
    expect(component).toMatchSnapshot();
  });

  it('renders empty list with empty component', () => {
    const component = ReactTestRenderer.create(
      <VirtualizedSectionList
        sections={[]}
        ListEmptyComponent={() => <empty />}
        ListFooterComponent={() => <footer />}
        ListHeaderComponent={() => <header />}
        getItem={(data, key) => data[key]}
        getItemCount={data => data.length}
        renderItem={({item}) => <item value={item.key} />}
      />,
    );
    expect(component).toMatchSnapshot();
  });

  it('renders list with empty component', () => {
    const component = ReactTestRenderer.create(
      <VirtualizedSectionList
        sections={[{title: 's1', data: [{key: 'hello'}]}]}
        ListEmptyComponent={() => <empty />}
        getItem={(data, key) => data[key]}
        getItemCount={data => data.length}
        renderItem={({item}) => <item value={item.key} />}
      />,
    );
    expect(component).toMatchSnapshot();
  });

  it('renders all the bells and whistles', () => {
    const component = ReactTestRenderer.create(
      <VirtualizedSectionList
        ItemSeparatorComponent={() => <separator />}
        ListEmptyComponent={() => <empty />}
        ListFooterComponent={() => <footer />}
        ListHeaderComponent={() => <header />}
        sections={[
          {
            title: 's1',
            data: new Array(5).fill().map((_, ii) => ({id: String(ii)})),
          },
        ]}
        getItem={(data, key) => data[key]}
        getItemCount={data => data.length}
        getItemLayout={({index}) => ({
          index: -1,
          length: 50,
          offset: index * 50,
        })}
        inverted={true}
        keyExtractor={(item, index) => item.id}
        onRefresh={jest.fn()}
        refreshing={false}
        renderItem={({item}) => <item value={item.id} />}
      />,
    );
    expect(component).toMatchSnapshot();
  });

  it('handles separators correctly', () => {
    const infos = [];
    let component;
    ReactTestRenderer.act(() => {
      component = ReactTestRenderer.create(
        <VirtualizedSectionList
          ItemSeparatorComponent={props => <separator {...props} />}
          sections={[
            {title: 's0', data: [{key: 'i0'}, {key: 'i1'}, {key: 'i2'}]},
          ]}
          renderItem={info => {
            infos.push(info);
            return <item title={info.item.key} />;
          }}
          getItem={(data, key) => data[key]}
          getItemCount={data => data.length}
        />,
      );
    });
    expect(component).toMatchSnapshot();

    ReactTestRenderer.act(() => {
      infos[1].separators.highlight();
    });
    expect(component).toMatchSnapshot();
    ReactTestRenderer.act(() => {
      infos[2].separators.updateProps('leading', {press: true});
    });
    expect(component).toMatchSnapshot();
    ReactTestRenderer.act(() => {
      infos[1].separators.unhighlight();
    });
    expect(component).toMatchSnapshot();
  });

  it('handles nested lists', () => {
    const component = ReactTestRenderer.create(
      <VirtualizedSectionList
        sections={[{title: 'outer', data: [{key: 'outer0'}, {key: 'outer1'}]}]}
        renderItem={outerInfo => (
          <VirtualizedSectionList
            sections={[
              {
                title: 'inner',
                data: [
                  {key: outerInfo.item.key + ':inner0'},
                  {key: outerInfo.item.key + ':inner1'},
                ],
              },
            ]}
            horizontal={outerInfo.item.key === 'outer1'}
            renderItem={innerInfo => {
              return <item title={innerInfo.item.key} />;
            }}
            getItem={(data, key) => data[key]}
            getItemCount={data => data.length}
          />
        )}
        getItem={(data, key) => data[key]}
        getItemCount={data => data.length}
      />,
    );
    expect(component).toMatchSnapshot();
  });

  describe('scrollToLocation', () => {
    const ITEM_HEIGHT = 100;

    const createVirtualizedSectionList = props => {
      const component = ReactTestRenderer.create(
        <VirtualizedSectionList
          sections={[
            {title: 's1', data: [{key: 'i1.1'}, {key: 'i1.2'}, {key: 'i1.3'}]},
            {title: 's2', data: [{key: 'i2.1'}, {key: 'i2.2'}, {key: 'i2.3'}]},
          ]}
          renderItem={({item}) => <item value={item.key} />}
          getItem={(data, key) => data[key]}
          getItemCount={data => data.length}
          getItemLayout={(data, index) => ({
            length: ITEM_HEIGHT,
            offset: ITEM_HEIGHT * index,
            index,
          })}
          {...props}
        />,
      );
      const instance = component.getInstance();
      const spy = jest.fn();

      // $FlowFixMe[incompatible-use] wrong types
      // $FlowFixMe[prop-missing] wrong types
      instance._listRef.scrollToIndex = spy;

      return {
        instance,
        spy,
      };
    };

    it('when sticky stickySectionHeadersEnabled={true}, header height is added to the developer-provided viewOffset', () => {
      const {instance, spy} = createVirtualizedSectionList({
        stickySectionHeadersEnabled: true,
      });

      const viewOffset = 25;

      // $FlowFixMe[prop-missing] scrollToLocation isn't on instance
      instance?.scrollToLocation({
        sectionIndex: 0,
        itemIndex: 1,
        viewOffset,
      });
      expect(spy).toHaveBeenCalledWith({
        index: 1,
        itemIndex: 1,
        sectionIndex: 0,
        viewOffset: viewOffset + ITEM_HEIGHT,
      });
    });

    it.each([
      [
        // prevents #18098
        {sectionIndex: 0, itemIndex: 0},
        {
          index: 0,
          itemIndex: 0,
          sectionIndex: 0,
          viewOffset: 0,
        },
      ],
      [
        {sectionIndex: 2, itemIndex: 1},
        {
          index: 11,
          itemIndex: 1,
          sectionIndex: 2,
          viewOffset: 0,
        },
      ],
      [
        {
          sectionIndex: 0,
          itemIndex: 1,
          viewOffset: 25,
        },
        {
          index: 1,
          itemIndex: 1,
          sectionIndex: 0,
          viewOffset: 25,
        },
      ],
    ])(
      'given sectionIndex, itemIndex and viewOffset, scrollToIndex is called with correct params',
      (scrollToLocationParams, expected) => {
        const {instance, spy} = createVirtualizedSectionList();
        // $FlowFixMe[prop-missing] scrollToLocation not on instance
        instance?.scrollToLocation(scrollToLocationParams);
        expect(spy).toHaveBeenCalledWith(expected);
      },
    );
  });
});
