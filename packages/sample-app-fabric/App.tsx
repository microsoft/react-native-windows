/**
 * Sample React Native App
 * https://github.com/facebook/react-native
 *
 * @format
 */

/**
 * Sample React Native App
 * https://github.com/facebook/react-native
 *
 * @format
 */

import React, {useState} from 'react';
import {SafeAreaView, StatusBar, useColorScheme, View, Text, StyleSheet} from 'react-native';
import {NewAppScreen} from '@react-native/new-app-screen';

import {CalendarView} from 'xaml-calendar-view';

type SectionProps = {
  title: string;
  children?: React.ReactNode;
};

function Section({title, children}: SectionProps): React.JSX.Element {
  return (
    <View style={styles.sectionContainer}>
      <Text style={styles.sectionTitle}>{title}</Text>
      {typeof children === 'string' ? (
        <Text style={styles.sectionDescription}>{children}</Text>
      ) : (
        children
      )}
    </View>
  );
}

function App(): React.JSX.Element {
  const isDarkMode = useColorScheme() === 'dark';

  // Adjust this type if your native event uses a different shape.
  // Many RNW components emit strings or numbers; you can swap to Date if you convert it.
  const [selectedDate, setSelectedDate] = useState<string | undefined>(undefined);

  return (
    <SafeAreaView style={{flex: 1}}>
      <StatusBar
        barStyle={isDarkMode ? 'light-content' : 'dark-content'}
        backgroundColor="transparent"
        translucent
      />

      {/* Your app content below the new screen */}
      <View
        style={[
          styles.content,
          {backgroundColor: isDarkMode ? '#000000' : '#FFFFFF'},
        ]}
      >
        <Section title="Calendar">
          <Text style={styles.sectionDescription}>
            Showing a CalendarView here (displayMode=1 for month view).
          </Text>

          <CalendarView
            style={{width: 400, height: 400}}
            displayMode="1"
            onSelectedDatesChanged={e => {
              setSelectedDate(e.nativeEvent.startDate);
            }}
          />
        </Section>

        <Section title="Selected date">
          <Text style={styles.sectionDescription}>
            {selectedDate ?? '—'}
          </Text>
        </Section>
      </View>
	   {/* New RN welcome screen */}
      <NewAppScreen />
    </SafeAreaView>
  );
}

const styles = StyleSheet.create({
  content: {
    paddingHorizontal: 24,
    paddingVertical: 16,
  },
  sectionContainer: {
    marginTop: 16,
  },
  sectionTitle: {
    fontSize: 20,
    fontWeight: '600',
    marginBottom: 6,
  },
  sectionDescription: {
    fontSize: 16,
    fontWeight: '400',
  },
});

export default App;
