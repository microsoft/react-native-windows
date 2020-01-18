/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 * @format
 */

import * as React from 'react';
import { AppRegistry, CheckBox, Text, View } from 'react-native';

const styles: any = {
    testRow: {
        flexDirection: 'row',
        marginBottom: 10,
    },
    controlsView: {
        flex: 1,
        flexDirection: 'column',
        width: 250,
        backgroundColor: 'azure',
        padding: 20,
    },
    styledView: {
        flex: 1,
        width: 250,
        alignItems: 'center',
        justifyContent: 'center',
        backgroundColor: '#ccc',
    },
    hoverView: {
        flex: 1,
        width: 250,
        flexDirection: 'column',
        alignItems: 'center',
        justifyContent: 'center',
        backgroundColor: 'lime',
    },
    baseView: {
        width: 200,
        margin: 20,
        backgroundColor: 'orange',
    },
    child: {
        width: 100,
        height: 50,
        fontSize: 20,
        backgroundColor: 'yellow',
        color: 'blue',
    },
};

export default class Bootstrap extends React.Component<
    {},
    {
        focusable: boolean;
        hasStyle: boolean;
        hasBorder: boolean;
        hasRadius: boolean;
        alignCenter: boolean;
        largeBorder: boolean;
        largePadding: boolean;
        enablePointerEvents: boolean;
        isMouseOver: boolean;
        lastPressedKey?: string;
    }
    > {
    constructor(props: {}) {
        super(props);
        this.state = {
            focusable: true,
            hasStyle: true,
            hasBorder: true,
            hasRadius: true,
            alignCenter: true,
            largeBorder: true,
            largePadding: true,
            enablePointerEvents: true,
            isMouseOver: false,
        };
    }

    render(): JSX.Element {
        return (
            <View style={{ flex: 1, flexDirection: 'column' }}>
                {this._renderStyledView()}
                {this._renderHoverView()}
            </View>
        );
    }

    private _renderOption(name: string, checked: boolean, onChange: (value: any) => void): JSX.Element {
        return (<View style={{ flexDirection: 'row' }}>
            <CheckBox
                onValueChange={onChange}
                value={checked}
            />
            <Text>{name}</Text>
        </View>);
    }

    private _renderStyledView(): JSX.Element {
        const viewStyle: any = { ...styles.baseView };

        if (this.state.largePadding) {
            viewStyle.padding = 15;
        }

        if (this.state.hasRadius) {
            viewStyle.borderRadius = 10;
            viewStyle.borderTopRightRadius = 30;
            viewStyle.backgroundColor = 'magenta';
        }

        if (this.state.hasBorder) {
            viewStyle.borderWidth = 1;
            viewStyle.borderColor = 'navy';
            viewStyle.backgroundColor = 'lime';

            if (this.state.largeBorder) {
                viewStyle.borderWidth = 20;
            }

            if (this.state.hasRadius) {
                viewStyle.borderColor = 'crimson';
                viewStyle.backgroundColor = 'pink';
            }
        }

        const viewProps: any = {
            style: this.state.hasStyle ? viewStyle : undefined,
        };

        return (
            <View style={styles.testRow}>
                <View
                    style={styles.controlsView}>
                    {this._renderOption('hasStyle', this.state.hasStyle, value => this.setState({ hasStyle: value }))}
                    {this._renderOption('hasBorder', this.state.hasBorder, value => this.setState({ hasBorder: value }))}
                    {this._renderOption('hasRadius', this.state.hasRadius, value => this.setState({ hasRadius: value }))}
                    {this._renderOption('largeBorder', this.state.largeBorder, value => this.setState({ largeBorder: value }))}
                    {this._renderOption('largePadding', this.state.largePadding, value => this.setState({ largePadding: value }))}
                    {this._renderOption('alignCenter', this.state.alignCenter, value => this.setState({ alignCenter: value }))}
                </View>
                <View
                    style={styles.styledView}>
                    <View {...viewProps}>
                        <Text style={[styles.child, { alignSelf: this.state.alignCenter ? 'center' : 'flex-start' }]}>The Text</Text>
                    </View>
                </View>
            </View>
        );
    }

    private _renderHoverView(): JSX.Element {
        const viewProps: any = {
            acceptsKeyboardFocus: this.state.focusable,
            pointerEvents: this.state.enablePointerEvents ? undefined : 'none',
            onMouseEnter: this._onMouseEnter,
            onMouseLeave: this._onMouseLeave,
            onKeyDown: this._onKeyDown,
        };

        return (
            <View style={styles.testRow}>
                <View
                    style={styles.controlsView}>
                    {this._renderOption('acceptsKeyboardFocus', this.state.focusable, value => this.setState({ focusable: value }))}
                    {this._renderOption('enablePointerEvents', this.state.enablePointerEvents, value => this.setState({ enablePointerEvents: value }))}
                </View>
                <View style={styles.hoverView} {...viewProps}>
                    <Text>{this.state.isMouseOver ? "Mouse is over" : "Mouse is not over"}</Text>
                    <Text>{this.state.lastPressedKey !== undefined ? `lastKey = '${this.state.lastPressedKey}'` : ''}</Text>
                </View>
            </View>
        );
    }

    private _onMouseEnter = (e: any) => {
        this.setState({ isMouseOver: true });
    }

    private _onMouseLeave = (e: any) => {
        this.setState({ isMouseOver: false });
    }

    private _onKeyDown = (e: any) => {
        this.setState({ lastPressedKey: e.nativeEvent.key });
    }
}

AppRegistry.registerComponent('Bootstrap', () => Bootstrap);
