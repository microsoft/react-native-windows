
'use strict';

const React = require('react');
const ReactTestRenderer = require('react-test-renderer');

const {ViewWindows} = require('../Components/View/ViewWindows');

describe('ViewWindows', () => {
  it('renders basic view', () => {
    const component = ReactTestRenderer.create(
      <ViewWindows/>
    );
    expect(component).toMatchSnapshot();
  });
});