/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 */

import React, { useState } from "react";
import TestPages, { ITestPage } from './TestPages'
import { TouchableOpacity, Text, View, TextInput, FlatList, Button, StyleSheet } from 'react-native'
import { SEARCH_BUTTON, HOME_BUTTON } from './Consts'


interface TestPageListProps {
  onNavigateTo: (pageTestId: string) => void,
  testPages: ITestPage[]
}

function TestPageList(props: TestPageListProps) {
  const [filter, setFilter] = useState("");

  const _renderItem = ({ item }: {item: ITestPage}) => (
    <TouchableOpacity onPress={() => { props.onNavigateTo(item.testId) }} testID={item.testId}>
      <View>
        <Text>{item.description}</Text>
      </View>
    </TouchableOpacity >
  );

  const data = props.testPages.filter((item: ITestPage) => !filter || (item.testId && item.testId.includes(filter)) || (item.description && item.description.includes(filter)))

  return (
    <View>
      <TextInput placeholder='Search test page' onChangeText={(text) => setFilter(text)} value={filter} testID={SEARCH_BUTTON} />
      <FlatList<ITestPage> data={data} renderItem={_renderItem} keyExtractor={(item) => item.testId}/>
    </View>)
}

interface TestPageDetailProps {
  testPage: ITestPage
}

function TestPageDetail(props: TestPageDetailProps) {
  return <props.testPage.content />
}

interface HeaderProps {
  onNavigateTo: (pageTestId: string) => void,
  description: string
}

function Header(props: HeaderProps) {
  return (
    <View style={styles.headerContainer}>
      <View style={styles.header}>
        <View style={styles.headerCenter}>
          <Text style={styles.title}>{props.description}</Text>
        </View>
        <Button title='Home' onPress={() => { props.onNavigateTo('') }} testID={HOME_BUTTON} />
      </View>
    </View>);
}

const styles = StyleSheet.create({
  headerContainer: {
    borderBottomWidth: StyleSheet.hairlineWidth,
    borderBottomColor: '#96969A',
  },
  header: {
    height: 40,
    flexDirection: 'row'
  },
  headerLeft: {},
  headerCenter: {
    flex: 1,
    position: 'absolute',
    top: 7,
    left: 0,
    right: 0
  },
  title: {
    fontSize: 19,
    fontWeight: '600',
    textAlign: 'center'
  },
  testPageContainer: {
    flex: 1
  }
});


export default function TestApp() {
  const [toPageTestId, setToPageTestId] = useState("");

  const _onNavigateTo = (pageTestId: string) => { setToPageTestId(pageTestId) };

  if (toPageTestId.length > 0) {
    const page = TestPages.filter(testPage => testPage.testId === toPageTestId)[0];
    return (
      <View style={styles.testPageContainer}>
        <Header onNavigateTo={() => { _onNavigateTo('') }} description={page.description} />
        <TestPageDetail testPage={page} />
      </View>)
  } else {
    return (
      <View style={styles.testPageContainer}>
        <Header onNavigateTo={() => { }} description='Home' />
        <TestPageList testPages={TestPages} onNavigateTo={(testPageId) => { _onNavigateTo(testPageId) }} />
      </View>)
  }
}