import React, { memo, useState, useTransition } from 'react';
import { AppRegistry, Button, ScrollView, StyleSheet, Text, TouchableOpacity, View } from 'react-native'

const styles = StyleSheet.create({
  tab: {
    alignItems: 'center',
  },
  post: {
    padding: 5,
    borderTopWidth: 1,
    backgroundColor: 'light-gray',
    alignItems: 'center',
  },
  activeTab: {
    fontSize: 20,
    fontWeight: 'bold',
    alignSelf: 'center',
  },
  pendingTab: {
    color: 'gray',
  },

});

function ContactTab() {
  return (
    <View style={styles.tab}>
      <Text>
        You can find me online here:
      </Text>
      <Text>* admin@mysite.com</Text>
      <Text>* +123456789</Text>
    </View>
  );
}

function AboutTab() {
  return (
    <View style={styles.tab}>
      <Text>Welcome to my profile!</Text>
    </View>
  );
}

const PostsTab = memo(function PostsTab() {
  // Log once. The actual slowdown is inside SlowPost.
  console.log('[ARTIFICIALLY SLOW] Rendering 500 <SlowPost />');

  let items = [];
  for (let i = 0; i < 500; i++) {
    items.push(<SlowPost key={i} index={i} />);
  }
  return (
    <ScrollView>
      {items}
    </ScrollView>
  );
});

function SlowPost({ index }) {
  let startTime = performance.now();
  while (performance.now() - startTime < 1) {
    // Do nothing for 1 ms per item to emulate extremely slow code
  }

  return (
    <TouchableOpacity>
      <View style={styles.post}>
        <Text>
          Post #{index + 1}
        </Text>
      </View>
    </TouchableOpacity>
  );
}

function TabButton({ title, isActive, onClick }) {
  const [isPending, startTransition] = useTransition();
  if (isActive || isPending) {
    const tabStyle = [styles.activeTab, isPending ? styles.pendingTab : null];
    return <Text style={tabStyle}>{title}</Text>
  }
  return (
    <Button
      onPress={() => startTransition(onClick)}
      title={title} />
  )
}


export default function App() {
  const [isPending, startTransition] = useTransition();
  const [tab, setTab] = useState('about');

  function selectTab(nextTab) {
    startTransition(() => {
      setTab(nextTab);
    });
  }

  return (
    <>
      <View style={{ flexDirection: 'row', justifyContent: 'space-around' }}>
        <TabButton
          isActive={tab === 'about'}
          onClick={() => selectTab('about')}
          title="About" />
        <TabButton
          isActive={tab === 'posts'}
          onClick={() => selectTab('posts')}
          title="Posts (slow)" />
        <TabButton
          isActive={tab === 'contact'}
          onClick={() => selectTab('contact')}
          title="Contact" />
      </View>
      {tab === 'about' && <AboutTab />}
      {tab === 'posts' && <PostsTab />}
      {tab === 'contact' && <ContactTab />}
    </>
  );
}

AppRegistry.registerComponent('Bootstrap', () => App);
