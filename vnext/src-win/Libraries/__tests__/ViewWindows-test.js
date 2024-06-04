
'use strict';

const React = require('react');
const ReactTestRenderer = require('react-test-renderer');

const View = require('../Components/View/View');

describe('View', () => {
  it('renders basic view', () => {
    const component = ReactTestRenderer.create(
      <View/>
    );
    expect(component).toMatchSnapshot();
  });
});