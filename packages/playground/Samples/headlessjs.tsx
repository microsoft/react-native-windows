/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 * @format
 */

import * as React from 'react';
import {AppRegistry, StyleSheet, Text, View, Button, NativeModules} from 'react-native';


let todos: {title: string}[] | undefined = undefined;

const fetchTodos = () => fetch('https://jsonplaceholder.typicode.com/todos')
    .then(data => {
        return data.json();
    })
    .then(parsedTodos => {
        todos = parsedTodos.slice(0, 20);
    });
const fetchTodosVoid = async () => { await fetchTodos(); }

const taskName = 'SampleHeadlessTask';

export default class Bootstrap extends React.Component<unknown, {renderTodos: boolean, preFetched: boolean}> {
    constructor(props: unknown) {
        super(props);
        if (todos) {
            this.state = {
                renderTodos: true,
                preFetched: true
            };
        }
        else
        {
            this.state = { renderTodos: false, preFetched: false}
        }
    }

    registerTask = () => {
        NativeModules.HeadlessJsTaskSupport.registerNativeJsTaskHook({taskName: taskName, runInterval: 20 })
    };

    fetchTodosSetState = () => {
        fetchTodos().then(() => {
            this.setState({...this.state, renderTodos: true});
            });
    };

  render() {
    return (
      <View style={styles.container}>
        <Text style={styles.welcome}>HeadlessJS Example!</Text>
        <Button title="register Headless Task" onPress={this.registerTask} />
        <Button title="fetch Todos" onPress={this.fetchTodosSetState} />
        {this.state.preFetched && this.state.renderTodos &&
            <Text style={styles.welcome}>Rendering Todos fetched from headless task!</Text>
        }
        {!this.state.preFetched && this.state.renderTodos &&
            <Text style={styles.welcome}>Rendering Todos fetched from clicking the button!</Text>
        }
        {this.state.renderTodos && todos && todos.map((todo, index) => 
            <View key={index}>
                <Text>{todo.title}</Text>
            </View>)}
      </View>
    );
  }
}

const styles = StyleSheet.create({
  container: {
    flex: 1,
    justifyContent: 'center',
    alignItems: 'center',
    backgroundColor: '#C5CCFF',
  },
  welcome: {
    fontSize: 20,
    textAlign: 'center',
    margin: 10,
  },
});

AppRegistry.registerComponent('Bootstrap', () => Bootstrap);

AppRegistry.registerHeadlessTask(taskName, () => fetchTodosVoid );