/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @format
 * @emails oncall+react_native
 */

'use strict';

const React = require('react');
const ReactTestRenderer = require('react-test-renderer');

const VirtualizedList = require('../VirtualizedList');

describe('VirtualizedList', () => {
  it('renders simple list', () => {
    const component = ReactTestRenderer.create(
      <VirtualizedList
        data={[{key: 'i1'}, {key: 'i2'}, {key: 'i3'}]}
        renderItem={({item}) => <item value={item.key} />}
        getItem={(data, index) => data[index]}
        getItemCount={(data) => data.length}
      />,
    );
    expect(component).toMatchSnapshot();
  });

  it('renders simple list using ListItemComponent', () => {
    function ListItemComponent({item}) {
      return <item value={item.key} />;
    }
    const component = ReactTestRenderer.create(
      <VirtualizedList
        data={[{key: 'i1'}, {key: 'i2'}, {key: 'i3'}]}
        ListItemComponent={ListItemComponent}
        getItem={(data, index) => data[index]}
        getItemCount={(data) => data.length}
      />,
    );
    expect(component).toMatchSnapshot();
  });

  it('warns if both renderItem or ListItemComponent are specified. Uses ListItemComponent', () => {
    jest.spyOn(console, 'warn').mockImplementationOnce(() => {});
    function ListItemComponent({item}) {
      return <item value={item.key} testID={`${item.key}-ListItemComponent`} />;
    }
    const component = ReactTestRenderer.create(
      <VirtualizedList
        data={[{key: 'i1'}]}
        ListItemComponent={ListItemComponent}
        renderItem={({item}) => (
          <item value={item.key} testID={`${item.key}-renderItem`} />
        )}
        getItem={(data, index) => data[index]}
        getItemCount={(data) => data.length}
      />,
    );

    expect(console.warn).toBeCalledWith(
      'VirtualizedList: Both ListItemComponent and renderItem props are present. ListItemComponent will take precedence over renderItem.',
    );
    expect(component).toMatchSnapshot();
    console.warn.mockRestore();
  });

  it('throws if no renderItem or ListItemComponent', () => {
    // Silence the React error boundary warning; we expect an uncaught error.
    const consoleError = console.error;
    jest.spyOn(console, 'error').mockImplementation((message) => {
      if (message.startsWith('The above error occurred in the ')) {
        return;
      }
      consoleError(message);
    });

    const componentFactory = () =>
      ReactTestRenderer.create(
        <VirtualizedList
          data={[{key: 'i1'}, {key: 'i2'}, {key: 'i3'}]}
          getItem={(data, index) => data[index]}
          getItemCount={(data) => data.length}
        />,
      );
    expect(componentFactory).toThrow(
      'VirtualizedList: Either ListItemComponent or renderItem props are required but none were found.',
    );

    console.error.mockRestore();
  });

  it('renders empty list', () => {
    const component = ReactTestRenderer.create(
      <VirtualizedList
        data={[]}
        renderItem={({item}) => <item value={item.key} />}
        getItem={(data, index) => data[index]}
        getItemCount={(data) => data.length}
      />,
    );
    expect(component).toMatchSnapshot();
  });

  it('renders empty list after batch', () => {
    const component = ReactTestRenderer.create(
      <VirtualizedList
        data={[]}
        renderItem={({item}) => <item value={item.key} />}
        getItem={(data, index) => data[index]}
        getItemCount={(data) => data.length}
      />,
    );

    ReactTestRenderer.act(() => {
      simulateLayout(component, {
        viewport: {width: 10, height: 50},
        content: {width: 10, height: 200},
      });

      performAllBatches();
    });

    expect(component).toMatchSnapshot();
  });

  it('renders null list', () => {
    const component = ReactTestRenderer.create(
      <VirtualizedList
        data={undefined}
        renderItem={({item}) => <item value={item.key} />}
        getItem={(data, index) => data[index]}
        getItemCount={(data) => 0}
      />,
    );
    expect(component).toMatchSnapshot();
  });

  it('renders empty list with empty component', () => {
    const component = ReactTestRenderer.create(
      <VirtualizedList
        data={[]}
        ListEmptyComponent={() => <empty />}
        ListFooterComponent={() => <footer />}
        ListHeaderComponent={() => <header />}
        getItem={(data, index) => data[index]}
        getItemCount={(data) => data.length}
        renderItem={({item}) => <item value={item.key} />}
      />,
    );
    expect(component).toMatchSnapshot();
  });

  it('renders list with empty component', () => {
    const component = ReactTestRenderer.create(
      <VirtualizedList
        data={[{key: 'hello'}]}
        ListEmptyComponent={() => <empty />}
        getItem={(data, index) => data[index]}
        getItemCount={(data) => data.length}
        renderItem={({item}) => <item value={item.key} />}
      />,
    );
    expect(component).toMatchSnapshot();
  });

  it('renders all the bells and whistles', () => {
    const component = ReactTestRenderer.create(
      <VirtualizedList
        ItemSeparatorComponent={() => <separator />}
        ListEmptyComponent={() => <empty />}
        ListFooterComponent={() => <footer />}
        ListHeaderComponent={() => <header />}
        data={new Array(5).fill().map((_, ii) => ({id: String(ii)}))}
        getItem={(data, index) => data[index]}
        getItemCount={(data) => data.length}
        getItemLayout={({index}) => ({length: 50, offset: index * 50})}
        inverted={true}
        keyExtractor={(item, index) => item.id}
        onRefresh={jest.fn()}
        refreshing={false}
        renderItem={({item}) => <item value={item.id} />}
      />,
    );
    expect(component).toMatchSnapshot();
  });

  it('test getItem functionality where data is not an Array', () => {
    const component = ReactTestRenderer.create(
      <VirtualizedList
        data={new Map([['id_0', {key: 'item_0'}]])}
        getItem={(data, index) => data.get('id_' + index)}
        getItemCount={(data: Map) => data.size}
        renderItem={({item}) => <item value={item.key} />}
      />,
    );
    expect(component).toMatchSnapshot();
  });

  it('handles separators correctly', () => {
    const infos = [];
    const component = ReactTestRenderer.create(
      <VirtualizedList
        ItemSeparatorComponent={(props) => <separator {...props} />}
        data={[{key: 'i0'}, {key: 'i1'}, {key: 'i2'}]}
        renderItem={(info) => {
          infos.push(info);
          return <item title={info.item.key} />;
        }}
        getItem={(data, index) => data[index]}
        getItemCount={(data) => data.length}
      />,
    );
    expect(component).toMatchSnapshot();
    infos[1].separators.highlight();
    expect(component).toMatchSnapshot();
    infos[2].separators.updateProps('leading', {press: true});
    expect(component).toMatchSnapshot();
    infos[1].separators.unhighlight();
  });

  it('handles nested lists', () => {
    const component = ReactTestRenderer.create(
      <VirtualizedList
        data={[{key: 'outer0'}, {key: 'outer1'}]}
        renderItem={(outerInfo) => (
          <VirtualizedList
            data={[
              {key: outerInfo.item.key + ':inner0'},
              {key: outerInfo.item.key + ':inner1'},
            ]}
            horizontal={outerInfo.item.key === 'outer1'}
            renderItem={(innerInfo) => {
              return <item title={innerInfo.item.key} />;
            }}
            getItem={(data, index) => data[index]}
            getItemCount={(data) => data.length}
          />
        )}
        getItem={(data, index) => data[index]}
        getItemCount={(data) => data.length}
      />,
    );
    expect(component).toMatchSnapshot();
  });

  it('returns the viewableItems correctly in the onViewableItemsChanged callback after changing the data', () => {
    const ITEM_HEIGHT = 800;
    let data = [{key: 'i1'}, {key: 'i2'}, {key: 'i3'}];
    const nativeEvent = {
      contentOffset: {y: 0, x: 0},
      layoutMeasurement: {width: 300, height: 600},
      contentSize: {width: 300, height: data.length * ITEM_HEIGHT},
      zoomScale: 1,
      contentInset: {right: 0, top: 0, left: 0, bottom: 0},
    };
    const onViewableItemsChanged = jest.fn();
    const props = {
      data,
      renderItem: ({item}) => <item value={item.key} />,
      getItem: (items, index) => items[index],
      getItemCount: (items) => items.length,
      getItemLayout: (items, index) => ({
        length: ITEM_HEIGHT,
        offset: ITEM_HEIGHT * index,
        index,
      }),
      onViewableItemsChanged,
    };

    const component = ReactTestRenderer.create(<VirtualizedList {...props} />);

    const instance = component.getInstance();

    instance._onScrollBeginDrag({nativeEvent});
    instance._onScroll({
      timeStamp: 1000,
      nativeEvent,
    });

    expect(onViewableItemsChanged).toHaveBeenCalledTimes(1);
    expect(onViewableItemsChanged).toHaveBeenLastCalledWith(
      expect.objectContaining({
        viewableItems: [expect.objectContaining({isViewable: true, key: 'i1'})],
      }),
    );
    data = [{key: 'i4'}, ...data];
    component.update(<VirtualizedList {...props} data={data} />);

    instance._onScroll({
      timeStamp: 2000,
      nativeEvent: {
        ...nativeEvent,
        contentOffset: {y: 100, x: 0},
      },
    });

    expect(onViewableItemsChanged).toHaveBeenCalledTimes(2);
    expect(onViewableItemsChanged).toHaveBeenLastCalledWith(
      expect.objectContaining({
        viewableItems: [expect.objectContaining({isViewable: true, key: 'i4'})],
      }),
    );
  });

  it('getScrollRef for case where it returns a ScrollView', () => {
    const listRef = React.createRef(null);

    ReactTestRenderer.create(
      <VirtualizedList
        data={[{key: 'i1'}, {key: 'i2'}, {key: 'i3'}]}
        renderItem={({item}) => <item value={item.key} />}
        getItem={(data, index) => data[index]}
        getItemCount={(data) => data.length}
        ref={listRef}
      />,
    );

    const scrollRef = listRef.current.getScrollRef();

    // This is checking if the ref acts like a ScrollView. If we had an
    // `isScrollView(ref)` method, that would be preferred.
    expect(scrollRef.scrollTo).toBeInstanceOf(jest.fn().constructor);
  });

  it('getScrollRef for case where it returns a View', () => {
    const listRef = React.createRef(null);

    ReactTestRenderer.create(
      <VirtualizedList
        data={[{key: 'outer0'}, {key: 'outer1'}]}
        renderItem={(outerInfo) => (
          <VirtualizedList
            data={[
              {key: outerInfo.item.key + ':inner0'},
              {key: outerInfo.item.key + ':inner1'},
            ]}
            renderItem={(innerInfo) => {
              return <item title={innerInfo.item.key} />;
            }}
            getItem={(data, index) => data[index]}
            getItemCount={(data) => data.length}
            ref={listRef}
          />
        )}
        getItem={(data, index) => data[index]}
        getItemCount={(data) => data.length}
      />,
    );
    const scrollRef = listRef.current.getScrollRef();

    // This is checking if the ref acts like a host component. If we had an
    // `isHostComponent(ref)` method, that would be preferred.
    expect(scrollRef.measure).toBeInstanceOf(jest.fn().constructor);
    expect(scrollRef.measureLayout).toBeInstanceOf(jest.fn().constructor);
    expect(scrollRef.measureInWindow).toBeInstanceOf(jest.fn().constructor);
  });
  it('does not call onEndReached when onContentSizeChange happens after onLayout', () => {
    const ITEM_HEIGHT = 40;
    const layout = {width: 300, height: 600};
    let data = Array(20)
      .fill()
      .map((_, index) => ({key: `key-${index}`}));
    const onEndReached = jest.fn();
    const props = {
      data,
      initialNumToRender: 10,
      onEndReachedThreshold: 2,
      windowSize: 21,
      renderItem: ({item}) => <item value={item.key} />,
      getItem: (items, index) => items[index],
      getItemCount: (items) => items.length,
      getItemLayout: (items, index) => ({
        length: ITEM_HEIGHT,
        offset: ITEM_HEIGHT * index,
        index,
      }),
      onEndReached,
    };

    const component = ReactTestRenderer.create(<VirtualizedList {...props} />);

    const instance = component.getInstance();

    instance._onLayout({nativeEvent: {layout}});

    const initialContentHeight = props.initialNumToRender * ITEM_HEIGHT;

    // We want to test the unusual case of onContentSizeChange firing after
    // onLayout, which can cause https://github.com/facebook/react-native/issues/16067
    instance._onContentSizeChange(300, initialContentHeight);
    instance._onContentSizeChange(300, data.length * ITEM_HEIGHT);
    performAllBatches();

    expect(onEndReached).not.toHaveBeenCalled();

    instance._onScroll({
      timeStamp: 1000,
      nativeEvent: {
        contentOffset: {y: initialContentHeight, x: 0},
        layoutMeasurement: layout,
        contentSize: {...layout, height: data.length * ITEM_HEIGHT},
        zoomScale: 1,
        contentInset: {right: 0, top: 0, left: 0, bottom: 0},
      },
    });
    performAllBatches();

    expect(onEndReached).toHaveBeenCalled();
  });

  it('provides a trace when a listKey collision occurs', () => {
    const errors = [];
    jest.spyOn(console, 'error').mockImplementation((...args) => {
      // Silence the DEV-only React error boundary warning.
      if ((args[0] || '').startsWith('The above error occurred in the ')) {
        return;
      }
      errors.push(args);
    });
    const commonProps = {
      data: [{key: 'cell0'}],
      getItem: (data, index) => data[index],
      getItemCount: (data) => data.length,
      renderItem: ({item}) => <item value={item.key} />,
    };
    try {
      ReactTestRenderer.create(
        <VirtualizedList
          {...commonProps}
          horizontal={false}
          listKey="level0"
          renderItem={() => (
            <VirtualizedList
              {...commonProps}
              horizontal={true}
              listKey="level1"
              renderItem={() => (
                <>
                  {/* Force a collision */}
                  <VirtualizedList
                    {...commonProps}
                    horizontal={true}
                    listKey="level2"
                  />
                  <VirtualizedList
                    {...commonProps}
                    horizontal={true}
                    listKey="level2"
                  />
                </>
              )}
            />
          )}
        />,
      );
      expect(errors).toMatchInlineSnapshot(`
        Array [
          Array [
            "A VirtualizedList contains a cell which itself contains more than one VirtualizedList of the same orientation as the parent list. You must pass a unique listKey prop to each sibling list.

        VirtualizedList trace:
          Child (horizontal):
            listKey: level2
            cellKey: cell0
          Parent (horizontal):
            listKey: level1
            cellKey: cell0
          Parent (vertical):
            listKey: level0
            cellKey: rootList",
          ],
        ]
      `);
    } finally {
      console.error.mockRestore();
    }
  });

  it('throws if using scrollToIndex with index less than 0', () => {
    const component = ReactTestRenderer.create(
      <VirtualizedList
        data={[{key: 'i1'}, {key: 'i2'}, {key: 'i3'}]}
        renderItem={({item}) => <item value={item.key} />}
        getItem={(data, index) => data[index]}
        getItemCount={(data) => data.length}
      />,
    );
    const instance = component.getInstance();

    expect(() => instance.scrollToIndex({index: -1})).toThrow(
      'scrollToIndex out of range: requested index -1 but minimum is 0',
    );
  });

  it('throws if using scrollToIndex when item length is less than 1', () => {
    const component = ReactTestRenderer.create(
      <VirtualizedList
        data={[]}
        renderItem={({item}) => <item value={item.key} />}
        getItem={(data, index) => data[index]}
        getItemCount={(data) => data.length}
      />,
    );
    const instance = component.getInstance();

    expect(() => instance.scrollToIndex({index: 1})).toThrow(
      'scrollToIndex out of range: item length 0 but minimum is 1',
    );
  });

  it('throws if using scrollToIndex when requested index is bigger than or equal to item length', () => {
    const component = ReactTestRenderer.create(
      <VirtualizedList
        data={[{key: 'i1'}, {key: 'i2'}, {key: 'i3'}]}
        renderItem={({item}) => <item value={item.key} />}
        getItem={(data, index) => data[index]}
        getItemCount={(data) => data.length}
      />,
    );
    const instance = component.getInstance();

    expect(() => instance.scrollToIndex({index: 3})).toThrow(
      'scrollToIndex out of range: requested index 3 is out of 0 to 2',
    );
  });

  it('forwards correct stickyHeaderIndices when all in initial render window', () => {
    const items = generateItemsStickyEveryN(10, 3);
    const ITEM_HEIGHT = 10;

    const component = ReactTestRenderer.create(
      <VirtualizedList
        initialNumToRender={10}
        {...baseItemProps(items)}
        {...fixedHeightItemLayoutProps(ITEM_HEIGHT)}
      />,
    );

    // The initial render is specified to be the length of items provided.
    // Expect that all sticky items (1 every 3) are passed to the underlying
    // scrollview.
    expect(component).toMatchSnapshot();
  });

  it('forwards correct stickyHeaderIndices when ListHeaderComponent present', () => {
    const items = generateItemsStickyEveryN(10, 3);
    const ITEM_HEIGHT = 10;

    const component = ReactTestRenderer.create(
      <VirtualizedList
        ListHeaderComponent={() => React.createElement('Header')}
        initialNumToRender={10}
        {...baseItemProps(items)}
        {...fixedHeightItemLayoutProps(ITEM_HEIGHT)}
      />,
    );

    // The initial render is specified to be the length of items provided.
    // Expect that all sticky items (1 every 3) are passed to the underlying
    // scrollview, indices offset by 1 to account for the the header component.
    expect(component).toMatchSnapshot();
  });

  it('forwards correct stickyHeaderIndices when partially in initial render window', () => {
    const items = generateItemsStickyEveryN(10, 3);

    const ITEM_HEIGHT = 10;

    const component = ReactTestRenderer.create(
      <VirtualizedList
        initialNumToRender={5}
        {...baseItemProps(items)}
        {...fixedHeightItemLayoutProps(ITEM_HEIGHT)}
      />,
    );

    // The initial render is specified to be half the length of items provided.
    // Expect that all sticky items of index < 5 are passed to the underlying
    // scrollview.
    expect(component).toMatchSnapshot();
  });

  it('renders sticky headers in viewport on batched render', () => {
    const items = generateItemsStickyEveryN(10, 3);
    const ITEM_HEIGHT = 10;

    let component;
    ReactTestRenderer.act(() => {
      component = ReactTestRenderer.create(
        <VirtualizedList
          initialNumToRender={1}
          windowSize={1}
          {...baseItemProps(items)}
          {...fixedHeightItemLayoutProps(ITEM_HEIGHT)}
        />,
      );
    });

    ReactTestRenderer.act(() => {
      simulateLayout(component, {
        viewport: {width: 10, height: 50},
        content: {width: 10, height: 100},
      });
      performAllBatches();
    });

    // A windowSize of 1 means we will render just the viewport height (50dip).
    // Expect 5 10dip items to eventually be rendered, with sticky headers in
    // the first 5 propagated.
    expect(component).toMatchSnapshot();
  });

  it('keeps sticky headers above viewport visualized', () => {
    const items = generateItemsStickyEveryN(20, 3);
    const ITEM_HEIGHT = 10;

    let component;
    ReactTestRenderer.act(() => {
      component = ReactTestRenderer.create(
        <VirtualizedList
          initialNumToRender={1}
          windowSize={1}
          {...baseItemProps(items)}
          {...fixedHeightItemLayoutProps(ITEM_HEIGHT)}
        />,
      );
    });

    ReactTestRenderer.act(() => {
      simulateLayout(component, {
        viewport: {width: 10, height: 50},
        content: {width: 10, height: 200},
      });
      performAllBatches();
    });

    ReactTestRenderer.act(() => {
      simulateScroll(component, {x: 0, y: 150});
      performAllBatches();
    });

    // Scroll to the bottom 50 dip (last five items) of the content. Expect the
    // last five items to be rendered (possibly more if realization window is
    // larger), along with the most recent sticky header above the realization
    // region, even though they are out of the viewport window in layout
    // coordinates. This is because they will remain rendered even once
    // scrolled-past in layout space.
    expect(component).toMatchSnapshot();
  });
});

it('unmounts sticky headers moved below viewport', () => {
  const items = generateItemsStickyEveryN(20, 3);
  const ITEM_HEIGHT = 10;

  let component;
  ReactTestRenderer.act(() => {
    component = ReactTestRenderer.create(
      <VirtualizedList
        initialNumToRender={1}
        windowSize={1}
        {...baseItemProps(items)}
        {...fixedHeightItemLayoutProps(ITEM_HEIGHT)}
      />,
    );
  });

  ReactTestRenderer.act(() => {
    simulateLayout(component, {
      viewport: {width: 10, height: 50},
      content: {width: 10, height: 200},
    });
    performAllBatches();
  });

  ReactTestRenderer.act(() => {
    simulateScroll(component, {x: 0, y: 150});
    performAllBatches();
  });

  ReactTestRenderer.act(() => {
    simulateScroll(component, {x: 0, y: 0});
    performAllBatches();
  });

  // Scroll to the bottom 50 dip (last five items) of the content, then back up
  // to the first 5. Ensure that sticky items are unmounted once they are below
  // the render area.
  expect(component).toMatchSnapshot();
});

it('renders offset cells in initial render when initialScrollIndex set', () => {
  const items = generateItems(10);
  const ITEM_HEIGHT = 10;

  const component = ReactTestRenderer.create(
    <VirtualizedList
      initialScrollIndex={4}
      initialNumToRender={4}
      {...baseItemProps(items)}
      {...fixedHeightItemLayoutProps(ITEM_HEIGHT)}
    />,
  );

  // Check that the first render respects initialScrollIndex
  expect(component).toMatchSnapshot();
});

it('does not over-render when there is less than initialNumToRender cells', () => {
  const items = generateItems(10);
  const ITEM_HEIGHT = 10;

  const component = ReactTestRenderer.create(
    <VirtualizedList
      initialScrollIndex={4}
      initialNumToRender={20}
      {...baseItemProps(items)}
      {...fixedHeightItemLayoutProps(ITEM_HEIGHT)}
    />,
  );

  // Check that the first render clamps to the last item when intialNumToRender
  // goes over it.
  expect(component).toMatchSnapshot();
});

it('retains intitial render if initialScrollIndex == 0', () => {
  const items = generateItems(20);
  const ITEM_HEIGHT = 10;

  let component;
  ReactTestRenderer.act(() => {
    component = ReactTestRenderer.create(
      <VirtualizedList
        initialNumToRender={5}
        initialScrollIndex={0}
        windowSize={1}
        {...baseItemProps(items)}
        {...fixedHeightItemLayoutProps(ITEM_HEIGHT)}
      />,
    );
  });

  ReactTestRenderer.act(() => {
    simulateLayout(component, {
      viewport: {width: 10, height: 50},
      content: {width: 10, height: 200},
    });
    performAllBatches();
  });

  ReactTestRenderer.act(() => {
    simulateScroll(component, {x: 0, y: 150});
    performAllBatches();
  });

  // If initialScrollIndex is 0 (the default), we should never unmount the top
  // initialNumToRender as part of the "scroll to top optimization", even after
  // scrolling to the bottom five items.
  expect(component).toMatchSnapshot();
});

it('discards intitial render if initialScrollIndex != 0', () => {
  const items = generateItems(20);
  const ITEM_HEIGHT = 10;

  let component;
  ReactTestRenderer.act(() => {
    component = ReactTestRenderer.create(
      <VirtualizedList
        initialScrollIndex={5}
        initialNumToRender={5}
        windowSize={1}
        {...baseItemProps(items)}
        {...fixedHeightItemLayoutProps(ITEM_HEIGHT)}
      />,
    );
  });

  ReactTestRenderer.act(() => {
    simulateLayout(component, {
      viewport: {width: 10, height: 50},
      content: {width: 10, height: 200},
    });
    performAllBatches();
  });

  ReactTestRenderer.act(() => {
    simulateScroll(component, {x: 0, y: 150});
    performAllBatches();
  });

  // If initialScrollIndex is not 0, we do not enable retaining initial render
  // as part of "scroll to top" optimization.
  expect(component).toMatchSnapshot();
});

it('expands render area by maxToRenderPerBatch on tick', () => {
  const items = generateItems(20);
  const ITEM_HEIGHT = 10;

  const props = {
    initialNumToRender: 5,
    maxToRenderPerBatch: 2,
  };

  let component;
  ReactTestRenderer.act(() => {
    component = ReactTestRenderer.create(
      <VirtualizedList
        {...baseItemProps(items)}
        {...fixedHeightItemLayoutProps(ITEM_HEIGHT)}
        {...props}
      />,
    );
  });

  ReactTestRenderer.act(() => {
    simulateLayout(component, {
      viewport: {width: 10, height: 50},
      content: {width: 10, height: 200},
    });
    performNextBatch();
  });

  // We start by rendering 5 items in the initial render, but have default
  // windowSize, enabling eventual rendering up to 20 viewports worth of
  // content. We limit this to rendering 2 items per-batch via
  // maxToRenderPerBatch, so we should only have 7 items rendered after the
  // initial timer tick.
  expect(component).toMatchSnapshot();
});

it('does not adjust render area until content area layed out', () => {
  const items = generateItems(20);
  const ITEM_HEIGHT = 10;

  let component;

  ReactTestRenderer.act(() => {
    component = ReactTestRenderer.create(
      <VirtualizedList
        initialNumToRender={5}
        windowSize={10}
        {...baseItemProps(items)}
        {...fixedHeightItemLayoutProps(ITEM_HEIGHT)}
      />,
    );
  });

  ReactTestRenderer.act(() => {
    simulateViewportLayout(component, {width: 10, height: 50});
    performAllBatches();
  });

  // We should not start layout-based logic to expand rendered area until
  // content is layed out. Expect only the 5 initial items to be rendered after
  // processing all batch work, even though the windowSize allows for more.
  expect(component).toMatchSnapshot();
});

it('does not adjust render area with non-zero initialScrollIndex until scrolled', () => {
  const items = generateItems(20);
  const ITEM_HEIGHT = 10;

  let component;
  ReactTestRenderer.act(() => {
    component = ReactTestRenderer.create(
      <VirtualizedList
        initialNumToRender={5}
        initialScrollIndex={1}
        windowSize={10}
        maxToRenderPerBatch={10}
        {...baseItemProps(items)}
        {...fixedHeightItemLayoutProps(ITEM_HEIGHT)}
      />,
    );
  });

  ReactTestRenderer.act(() => {
    simulateLayout(component, {
      viewport: {width: 10, height: 50},
      content: {width: 10, height: 200},
    });
    performAllBatches();
  });

  // Layout information from before the time we scroll to initial index may not
  // correspond to the area "initialScrollIndex" points to. Expect only the 5
  // initial items (starting at initialScrollIndex) to be rendered after
  // processing all batch work, even though the windowSize allows for more.
  expect(component).toMatchSnapshot();
});

it('adjusts render area with non-zero initialScrollIndex after scrolled', () => {
  const items = generateItems(20);
  const ITEM_HEIGHT = 10;

  let component;
  ReactTestRenderer.act(() => {
    component = ReactTestRenderer.create(
      <VirtualizedList
        initialNumToRender={5}
        initialScrollIndex={1}
        windowSize={10}
        maxToRenderPerBatch={10}
        {...baseItemProps(items)}
        {...fixedHeightItemLayoutProps(ITEM_HEIGHT)}
      />,
    );
  });

  ReactTestRenderer.act(() => {
    simulateLayout(component, {
      viewport: {width: 10, height: 50},
      content: {width: 10, height: 200},
    });

    simulateScroll(component, {x: 0, y: 10});
    performAllBatches();
  });

  // We should expand the render area after receiving a message indcating we
  // arrived at initialScrollIndex.
  expect(component).toMatchSnapshot();
});

it('renders initialNumToRender cells when virtualization disabled', () => {
  const items = generateItems(10);
  const ITEM_HEIGHT = 10;

  const component = ReactTestRenderer.create(
    <VirtualizedList
      initialNumToRender={5}
      initialScrollIndex={1}
      disableVirtualization
      {...baseItemProps(items)}
      {...fixedHeightItemLayoutProps(ITEM_HEIGHT)}
    />,
  );

  // We should render initialNumToRender items with no spacers on initial render
  // when virtualization is disabled
  expect(component).toMatchSnapshot();
});

it('renders no spacers up to initialScrollIndex on first render when virtualization disabled', () => {
  const items = generateItems(10);
  const ITEM_HEIGHT = 10;

  let component;
  ReactTestRenderer.act(() => {
    component = ReactTestRenderer.create(
      <VirtualizedList
        initialNumToRender={2}
        initialScrollIndex={4}
        maxToRenderPerBatch={1}
        disableVirtualization
        {...baseItemProps(items)}
        {...fixedHeightItemLayoutProps(ITEM_HEIGHT)}
      />,
    );
  });

  // There should be no spacers present in an offset initial render with
  // virtualiztion disabled. Only initialNumToRender items starting at
  // initialScrollIndex.
  expect(component).toMatchSnapshot();
});

it('expands first in viewport to render up to maxToRenderPerBatch on initial render', () => {
  const items = generateItems(10);
  const ITEM_HEIGHT = 10;

  let component;
  ReactTestRenderer.act(() => {
    component = ReactTestRenderer.create(
      <VirtualizedList
        initialNumToRender={2}
        initialScrollIndex={4}
        maxToRenderPerBatch={10}
        {...baseItemProps(items)}
        {...fixedHeightItemLayoutProps(ITEM_HEIGHT)}
      />,
    );
  });

  // When virtualization is disabled we may render items before initialItemIndex
  // if initialItemIndex + initialNumToRender < maToRenderPerBatch. Expect cells
  // 0-3 to be rendered in this example, even though initialScrollIndex is 4.
  expect(component).toMatchSnapshot();
});

it('renders items before initialScrollIndex on first batch tick when virtualization disabled', () => {
  const items = generateItems(10);
  const ITEM_HEIGHT = 10;

  let component;
  ReactTestRenderer.act(() => {
    component = ReactTestRenderer.create(
      <VirtualizedList
        initialNumToRender={1}
        initialScrollIndex={5}
        maxToRenderPerBatch={1}
        disableVirtualization
        {...baseItemProps(items)}
        {...fixedHeightItemLayoutProps(ITEM_HEIGHT)}
      />,
    );
  });

  ReactTestRenderer.act(() => {
    simulateLayout(component, {
      viewport: {width: 10, height: 50},
      content: {width: 10, height: 100},
    });
    performNextBatch();
  });

  // When virtualization is disabled, we render "maxToRenderPerBatch" items
  // sequentially per batch tick. Any items not yet rendered before
  // initialScrollIndex are currently rendered at this time. Expect the first
  // tick to render all items before initialScrollIndex, along with
  // maxToRenderPerBatch after.
  expect(component).toMatchSnapshot();
});

it('eventually renders all items when virtualization disabled', () => {
  const items = generateItems(10);
  const ITEM_HEIGHT = 10;

  let component;
  ReactTestRenderer.act(() => {
    component = ReactTestRenderer.create(
      <VirtualizedList
        initialNumToRender={5}
        initialScrollIndex={1}
        windowSize={1}
        maxToRenderPerBatch={10}
        disableVirtualization
        {...baseItemProps(items)}
        {...fixedHeightItemLayoutProps(ITEM_HEIGHT)}
      />,
    );
  });

  ReactTestRenderer.act(() => {
    simulateLayout(component, {
      viewport: {width: 10, height: 50},
      content: {width: 10, height: 100},
    });
    performAllBatches();
  });

  // After all batch ticks, all items should eventually be rendered when\
  // virtualization is disabled.
  expect(component).toMatchSnapshot();
});

it('retains initial render region when an item is appended', () => {
  const items = generateItems(10);
  const ITEM_HEIGHT = 10;

  let component;
  ReactTestRenderer.act(() => {
    component = ReactTestRenderer.create(
      <VirtualizedList
        initialNumToRender={3}
        {...baseItemProps(items)}
        {...fixedHeightItemLayoutProps(ITEM_HEIGHT)}
      />,
    );
  });

  ReactTestRenderer.act(() => {
    component.update(
      <VirtualizedList
        initialNumToRender={3}
        {...baseItemProps(items)}
        {...fixedHeightItemLayoutProps(ITEM_HEIGHT)}
        data={generateItems(11)}
      />,
    );
  });

  // Adding an item to the list before batch render should keep the existing
  // rendered items rendered. Expect the first 3 items rendered, and a spacer
  // for 8 items (including the 11th, added item).
  expect(component).toMatchSnapshot();
});

it('retains batch render region when an item is appended', () => {
  const items = generateItems(10);
  const ITEM_HEIGHT = 10;

  let component;
  ReactTestRenderer.act(() => {
    component = ReactTestRenderer.create(
      <VirtualizedList
        initialNumToRender={1}
        maxToRenderPerBatch={1}
        {...baseItemProps(items)}
        {...fixedHeightItemLayoutProps(ITEM_HEIGHT)}
      />,
    );
  });

  ReactTestRenderer.act(() => {
    simulateLayout(component, {
      viewport: {width: 10, height: 50},
      content: {width: 10, height: 100},
    });
    performAllBatches();
  });

  ReactTestRenderer.act(() => {
    component.update(
      <VirtualizedList
        initialNumToRender={1}
        maxToRenderPerBatch={1}
        {...baseItemProps(items)}
        {...fixedHeightItemLayoutProps(ITEM_HEIGHT)}
        data={generateItems(11)}
      />,
    );
  });

  // Adding an item to the list after batch render should keep the existing
  // rendered items rendered. We batch render 10 items, then add an 11th. Expect
  // the first ten items to be present, with a spacer for the 11th until the
  // next batch render.
  expect(component).toMatchSnapshot();
});

it('constrains batch render region when an item is removed', () => {
  const items = generateItems(10);
  const ITEM_HEIGHT = 10;

  let component;
  ReactTestRenderer.act(() => {
    component = ReactTestRenderer.create(
      <VirtualizedList
        initialNumToRender={1}
        maxToRenderPerBatch={1}
        {...baseItemProps(items)}
        {...fixedHeightItemLayoutProps(ITEM_HEIGHT)}
      />,
    );
  });

  ReactTestRenderer.act(() => {
    simulateLayout(component, {
      viewport: {width: 10, height: 50},
      content: {width: 10, height: 100},
    });
    performAllBatches();
  });

  ReactTestRenderer.act(() => {
    component.update(
      <VirtualizedList
        initialNumToRender={1}
        maxToRenderPerBatch={1}
        {...baseItemProps(items)}
        {...fixedHeightItemLayoutProps(ITEM_HEIGHT)}
        data={generateItems(5)}
      />,
    );
  });

  // If the number of items is reduced, we should remove the corresponding
  // already rendered items. Expect there to be 5 items present. New items in a
  // previously occupied index may also be immediately rendered.
  expect(component).toMatchSnapshot();
});

it('renders a zero-height tail spacer on initial render if getItemLayout not defined', () => {
  const items = generateItems(10);

  const component = ReactTestRenderer.create(
    <VirtualizedList initialNumToRender={3} {...baseItemProps(items)} />,
  );

  // Do not add space for out-of-viewport content on initial render when we do
  // not yet know how large it should be (no getItemLayout and cell onLayout not
  // yet called). Expect the tail spacer not to occupy space.
  expect(component).toMatchSnapshot();
});

it('renders zero-height tail spacer on batch render if cells not yet measured and getItemLayout not defined', () => {
  const items = generateItems(10);

  let component;
  ReactTestRenderer.act(() => {
    component = ReactTestRenderer.create(
      <VirtualizedList
        initialNumToRender={3}
        maxToRenderPerBatch={1}
        windowSize={1}
        {...baseItemProps(items)}
      />,
    );
  });

  ReactTestRenderer.act(() => {
    simulateLayout(component, {
      viewport: {width: 10, height: 50},
      content: {width: 10, height: 200},
    });
    performNextBatch();
  });

  // Do not add space for out-of-viewport content unless the cell has previously
  // been layed out and measurements cached. Expect the tail spacer not to
  // occupy space.
  expect(component).toMatchSnapshot();
});

it('renders tail spacer up to last measured index if getItemLayout not defined', () => {
  const items = generateItems(10);

  let component;
  ReactTestRenderer.act(() => {
    component = ReactTestRenderer.create(
      <VirtualizedList
        initialNumToRender={3}
        maxToRenderPerBatch={1}
        windowSize={1}
        {...baseItemProps(items)}
      />,
    );
  });

  ReactTestRenderer.act(() => {
    const LAST_MEASURED_CELL = 6;
    for (let i = 0; i <= LAST_MEASURED_CELL; ++i) {
      simulateCellLayout(component, items, i, {
        width: 10,
        height: 10,
        x: 0,
        y: 10 * i,
      });
    }

    simulateLayout(component, {
      viewport: {width: 10, height: 50},
      content: {width: 10, height: 30},
    });
    performNextBatch();
  });

  // If cells in the out-of-viewport area have been measured, their space can be
  // incorporated into the tail spacer, without space for the cells we can not
  // measure until layout. Expect there to be a tail spacer occupying the space
  // for measured, but not yet rendered items (up to and including item 6).
  expect(component).toMatchSnapshot();
});

it('renders tail spacer up to last measured with irregular layout when getItemLayout undefined', () => {
  const items = generateItems(10);

  let component;
  ReactTestRenderer.act(() => {
    component = ReactTestRenderer.create(
      <VirtualizedList
        initialNumToRender={3}
        maxToRenderPerBatch={1}
        windowSize={1}
        {...baseItemProps(items)}
      />,
    );
  });

  ReactTestRenderer.act(() => {
    const LAST_MEASURED_CELL = 6;

    let currentY = 0;
    for (let i = 0; i <= LAST_MEASURED_CELL; ++i) {
      simulateCellLayout(component, items, i, {
        width: 10,
        height: i,
        x: 0,
        y: currentY + i,
      });
      currentY += i;
    }

    simulateLayout(component, {
      viewport: {width: 10, height: 50},
      content: {width: 10, height: 30},
    });
    performNextBatch();
  });

  // If cells in the out-of-viewport area have been measured, their space can be
  // incorporated into the tail spacer, without space for the cells we can not
  // measure until layout. Expect there to be a tail spacer occupying the space
  // for measured, but not yet rendered items (up to and including item 6).
  expect(component).toMatchSnapshot();
});

it('renders full tail spacer if all cells measured', () => {
  const items = generateItems(10);

  let component;
  ReactTestRenderer.act(() => {
    component = ReactTestRenderer.create(
      <VirtualizedList
        initialNumToRender={3}
        maxToRenderPerBatch={1}
        windowSize={1}
        {...baseItemProps(items)}
      />,
    );
  });

  ReactTestRenderer.act(() => {
    const LAST_MEASURED_CELL = 9;
    for (let i = 0; i <= LAST_MEASURED_CELL; ++i) {
      simulateCellLayout(component, items, i, {
        width: 10,
        height: 10,
        x: 0,
        y: 10 * i,
      });
    }

    simulateLayout(component, {
      viewport: {width: 10, height: 50},
      content: {width: 10, height: 30},
    });
    performNextBatch();
  });

  // The tail-spacer should occupy the space of all non-rendered items if all
  // items have been measured.
  expect(component).toMatchSnapshot();
});

it('renders windowSize derived region at top', () => {
  const items = generateItems(10);
  const ITEM_HEIGHT = 10;

  let component;
  ReactTestRenderer.act(() => {
    component = ReactTestRenderer.create(
      <VirtualizedList
        initialNumToRender={1}
        maxToRenderPerBatch={1}
        windowSize={3}
        {...baseItemProps(items)}
        {...fixedHeightItemLayoutProps(ITEM_HEIGHT)}
      />,
    );
  });

  ReactTestRenderer.act(() => {
    simulateLayout(component, {
      viewport: {width: 10, height: 20},
      content: {width: 10, height: 100},
    });
    performAllBatches();
  });

  // A windowSize of 3 means that we should render a viewport's worth of content
  // above and below the current. A 20 dip viewport at the top of the list means
  // we should render the top 4 10-dip items (for the current viewport, and
  // 20dip below).
  expect(component).toMatchSnapshot();
});

it('renders windowSize derived region in middle', () => {
  const items = generateItems(10);
  const ITEM_HEIGHT = 10;

  let component;
  ReactTestRenderer.act(() => {
    component = ReactTestRenderer.create(
      <VirtualizedList
        initialNumToRender={1}
        maxToRenderPerBatch={1}
        windowSize={3}
        {...baseItemProps(items)}
        {...fixedHeightItemLayoutProps(ITEM_HEIGHT)}
      />,
    );
  });

  ReactTestRenderer.act(() => {
    simulateLayout(component, {
      viewport: {width: 10, height: 20},
      content: {width: 10, height: 100},
    });
    performAllBatches();
  });

  ReactTestRenderer.act(() => {
    simulateScroll(component, {x: 0, y: 50});
    performAllBatches();
  });

  // A windowSize of 3 means that we should render a viewport's worth of content
  // above and below the current. A 20 dip viewport in the top of the list means
  // we should render the 6 10-dip items (for the current viewport, 20 dip above
  // and below), along with retaining the top initialNumToRenderItems. We seem
  // to actually render 7 in the middle due to rounding at the moment.
  expect(component).toMatchSnapshot();
});

it('renders windowSize derived region at bottom', () => {
  const items = generateItems(10);
  const ITEM_HEIGHT = 10;

  let component;
  ReactTestRenderer.act(() => {
    component = ReactTestRenderer.create(
      <VirtualizedList
        initialNumToRender={1}
        maxToRenderPerBatch={1}
        windowSize={3}
        {...baseItemProps(items)}
        {...fixedHeightItemLayoutProps(ITEM_HEIGHT)}
      />,
    );
  });

  ReactTestRenderer.act(() => {
    simulateLayout(component, {
      viewport: {width: 10, height: 20},
      content: {width: 10, height: 100},
    });
    performAllBatches();
  });

  ReactTestRenderer.act(() => {
    simulateScroll(component, {x: 0, y: 80});
    performAllBatches();
  });

  // A windowSize of 3 means that we should render a viewport's worth of content
  // above and below the current. A 20 dip viewport at the bottom of the list
  // means we should render the bottom 4 10-dip items (for the current viewport,
  // and 20dip above), along with retaining the top initialNumToRenderItems. We
  // seem to actually render 4 at the bottom due to rounding at the moment.
  expect(component).toMatchSnapshot();
});

function generateItems(count) {
  return Array(count)
    .fill()
    .map((_, i) => ({key: i}));
}

function generateItemsStickyEveryN(count, n) {
  return Array(count)
    .fill()
    .map((_, i) => (i % n === 0 ? {key: i, sticky: true} : {key: i}));
}

function baseItemProps(items) {
  return {
    data: items,
    renderItem: ({item}) =>
      React.createElement('MockCellItem', {value: item.key, ...item}),
    getItem: (data, index) => data[index],
    getItemCount: (data) => data.length,
    stickyHeaderIndices: stickyHeaderIndices(items),
  };
}

function stickyHeaderIndices(items) {
  return items.filter((item) => item.sticky).map((item) => item.key);
}

function fixedHeightItemLayoutProps(height) {
  return {
    getItemLayout: (_, index) => ({
      length: height,
      offset: height * index,
      index,
    }),
  };
}

let lastViewportLayout;
let lastContentLayout;

function simulateLayout(component, args) {
  simulateViewportLayout(component, args.viewport);
  simulateContentLayout(component, args.content);
}

function simulateViewportLayout(component, dimensions) {
  lastViewportLayout = dimensions;
  component.getInstance()._onLayout({nativeEvent: {layout: dimensions}});
}

function simulateContentLayout(component, dimensions) {
  lastContentLayout = dimensions;
  component
    .getInstance()
    ._onContentSizeChange(dimensions.width, dimensions.height);
}

function simulateCellLayout(component, items, itemIndex, dimensions) {
  const instance = component.getInstance();
  const cellKey = instance._keyExtractor(items[itemIndex], itemIndex);
  instance._onCellLayout(
    {nativeEvent: {layout: dimensions}},
    cellKey,
    itemIndex,
  );
}

function simulateScroll(component, position) {
  component.getInstance()._onScroll({
    nativeEvent: {
      contentOffset: position,
      contentSize: lastContentLayout,
      layoutMeasurement: lastViewportLayout,
    },
  });
}

function performAllBatches() {
  jest.runAllTimers();
}

function performNextBatch() {
  jest.runOnlyPendingTimers();
}
