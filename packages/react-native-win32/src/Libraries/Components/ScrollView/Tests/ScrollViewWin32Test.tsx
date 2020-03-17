'use strict';
import * as React from 'react';
import { Text, Button, /*StyleSheet*/ } from 'react-native';
import { ScrollViewWin32 } from '../ScrollViewWin32';
import { ViewWin32 } from '../../View/ViewWin32';

class BiDirectionalTestComponent extends React.Component<{}> {

    private onPressButton = () => {
        alert('You clicked the button!')
    };

    public render() {
        return (
            <ScrollViewWin32 biDirectional>
                <ViewWin32 style={[{ flexDirection: 'row'}]}> 
                <ViewWin32 style={[{ flexDirection: 'column'}]}>
                    <ViewWin32 style={[{ width: 220, height: 70, padding: 10 }]}>
                        <Button
                            onPress={this.onPressButton}
                            title="Button 1"
                            color="#FF3D00"
                        />
                    </ViewWin32>
                    <Text style={{ fontSize: 22, padding: 10 }}>javatpoint</Text>
                    <ViewWin32 style={[{ width: 220, height: 70, padding: 10 }]}>
                        <Button
                            onPress={this.onPressButton}
                            title="Button 2"
                            color="#3D00FF"
                        />
                    </ViewWin32>
                    <Text style={{ fontSize: 22, padding: 10 }}>React Native ScrollView Example</Text>
                    <ViewWin32 style={[{ width: 220, height: 70, padding: 10 }]}>
                        <Button
                            onPress={this.onPressButton}
                            title="Button 3"
                            color="#FFFF3D"
                        />
                    </ViewWin32>
                    <Text style={{ fontSize: 22, padding: 10 }}>If you like</Text>
                    <ViewWin32 style={[{ width: 220, height: 70, padding: 10 }]}>
                        <Button
                            onPress={this.onPressButton}
                            title="Button 4"
                            color="#FF3DFF"
                        />
                    </ViewWin32>
                    <Text style={{ fontSize: 22, padding: 10 }}>Scrolling horizontal</Text>
                    <ViewWin32 style={[{ width: 220, height: 70, padding: 10 }]}>
                        <Button
                            onPress={this.onPressButton}
                            title="Button 5"
                            color="#3DFF00"
                        />
                    </ViewWin32>
                </ViewWin32>
                <Text style={{ fontSize: 20 }}>Scroll me plz</Text>
                <Button title={'Button 1'} onPress={this.onPressButton} />
                <Text style={{ fontSize: 20 }}>React Native Example of ScrollView</Text>
                <Button title={'Button 2'} onPress={this.onPressButton} />
                <Text style={{ fontSize: 20 }}>React Native ScrollView Example</Text>
                <Button title={'Button 3'} onPress={this.onPressButton} />
                <Text style={{ fontSize: 20 }}>If you like</Text>
                <Button title={'Button 4'} onPress={this.onPressButton} />
                <Text style={{ fontSize: 20 }}>Scrolling down</Text>
                <Button title={'Button 5'} onPress={this.onPressButton} />
                <Text style={{ fontSize: 20 }}>Scrolling down</Text>
                <Button title={'Button 6'} onPress={this.onPressButton} />
                <Text style={{ fontSize: 20 }}>What's the best</Text>
                <Button title={'Button 7'} onPress={this.onPressButton} />
                <Text style={{ fontSize: 20 }}>What's the best</Text>
                <Button title={'Button 8'} onPress={this.onPressButton} />
                <Text style={{ fontSize: 20 }}>Framework around?</Text>
                <Button title={'Button 9'} onPress={this.onPressButton} />
                <Text style={{ fontSize: 20 }}>Framework around?</Text>
                <Button title={'Button 10'} onPress={this.onPressButton} />
                <Text style={{ fontSize: 20 }}>React Native</Text>
                <Button title={'Button 11'} onPress={this.onPressButton} />
                <Text style={{ fontSize: 20 }}>Scroll me plz</Text>
                <Button title={'Button 12'} onPress={this.onPressButton} />
                <Text style={{ fontSize: 20 }}>Scroll me plz</Text>
                <Button title={'Button 13'} onPress={this.onPressButton} />
                <Text style={{ fontSize: 20 }}>If you like</Text>
                <Button title={'Button 14'} onPress={this.onPressButton} />
                <Text style={{ fontSize: 20 }}>If you like</Text>
                <Button title={'Button 15'} onPress={this.onPressButton} />
                <Text style={{ fontSize: 20 }}>Scrolling down</Text>
                <Button title={'Button 16'} onPress={this.onPressButton} />
                <Text style={{ fontSize: 20 }}>Framework around?</Text>
                <Button title={'Button 17'} onPress={this.onPressButton} />
                <Text style={{ fontSize: 20 }}>React Native</Text>
                <Button title={'Button 18'} onPress={this.onPressButton} />
                <Text style={{ fontSize: 20 }}>Scroll me plz</Text>
                <Button title={'Button 19'} onPress={this.onPressButton} />
                <Text style={{ fontSize: 20 }}>Scroll me plz</Text>
                <Button title={'Button 20'} onPress={this.onPressButton} />
                <Text style={{ fontSize: 20 }}>If you like</Text>
                <Button title={'Button 21'} onPress={this.onPressButton} />
                <Text style={{ fontSize: 20 }}>If you like</Text>
                <Button title={'Button 22'} onPress={this.onPressButton} />
                <Text style={{ fontSize: 20 }}>Scrolling down</Text>
                <Button title={'Button 23'} onPress={this.onPressButton} />
                </ ViewWin32>
            </ScrollViewWin32>
        );
    }
}

class HorizontalTestComponent extends React.Component<{}> {
    private onPressButton = () => {
        alert('You clicked the button!')
    };

    public render() {
        return (
            <ScrollViewWin32 horizontal>
                <ViewWin32 style={[{ flexDirection: 'row'}]}>
                    <ViewWin32 style={[{ width: 220, height: 70, padding: 10 }]}>
                        <Button
                            onPress={this.onPressButton}
                            title="Button 1"
                            color="#FF3D00"
                        />
                    </ViewWin32>
                    <Text style={{ fontSize: 22, padding: 10 }}>javatpoint</Text>
                    <ViewWin32 style={[{ width: 220, height: 70, padding: 10 }]}>
                        <Button
                            onPress={this.onPressButton}
                            title="Button 2"
                            color="#3D00FF"
                        />
                    </ViewWin32>
                    <Text style={{ fontSize: 22, padding: 10 }}>React Native ScrollView Example</Text>
                    <ViewWin32 style={[{ width: 220, height: 70, padding: 10 }]}>
                        <Button
                            onPress={this.onPressButton}
                            title="Button 3"
                            color="#FFFF3D"
                        />
                    </ViewWin32>
                    <Text style={{ fontSize: 22, padding: 10 }}>If you like</Text>
                    <ViewWin32 style={[{ width: 220, height: 70, padding: 10 }]}>
                        <Button
                            onPress={this.onPressButton}
                            title="Button 4"
                            color="#FF3DFF"
                        />
                    </ViewWin32>
                    <Text style={{ fontSize: 22, padding: 10 }}>Scrolling horizontal</Text>
                    <ViewWin32 style={[{ width: 220, height: 70, padding: 10 }]}>
                        <Button
                            onPress={this.onPressButton}
                            title="Button 5"
                            color="#3DFF00"
                        />
                    </ViewWin32>
                </ViewWin32>
            </ScrollViewWin32>
        );
    }
}

export const title = '<ScrollViewWin32>';
export const displayName = 'ScrollViewWin32 Example';
export const description = 'All the stock View props plus ScrollWin32 specific ones';
export const examples = [
    {
        title: 'BiDi ScrollView example',
        description: '',
        render(): JSX.Element {
            return (
                <ViewWin32>
                    <BiDirectionalTestComponent />
                </ViewWin32>
            );
        },
    },
    {
        title: 'Horizontal ScrollView example',
        description: '',
        render(): JSX.Element {
            return (
                <ViewWin32>
                    <HorizontalTestComponent />
                </ViewWin32>
            );
        },
    }
];