import React, { useState, FC } from 'react';
import { AppRegistry, StyleSheet, Text, View, Modal, TouchableOpacity, FlatList } from 'react-native';

interface PickerProps {
    options: { label: string; value: any }[];
    currentPicker: string;
    selectedLabel: any;
    onSelect: (value: any) => void;
}

interface TestPickerProps {
    options: { label: string; value: any }[];
    id: string;
    selectedLabel: any;
    onSelect: (value: any) => void;
}

const TestPicker: FC<PickerProps> = ({ options, currentPicker, selectedLabel, onSelect }) => {
    const [modalVisible, setModalVisible] = useState(false);
    const [selectedValue, setSelectedValue] = useState<any>(selectedLabel);

    const setSelection = (value: any) => {
        setSelectedValue(value);
        onSelect(value);
        setModalVisible(false);
    };

    const renderPickerItems = (optionsList: { label: string; value: any }[]) => {
        return (
            <FlatList
                style={styles.flatList}
                data={optionsList}
                renderItem={({ item }) => (
                    <TouchableOpacity
                        style={styles.item}
                        onPress={() => setSelection(item.value)}>
                        <Text style={styles.itemText}>{item.label}</Text>
                    </TouchableOpacity>
                )}
                keyExtractor={item => item.value.toString()}
            />
        );
    };

    return (
        <View>
            <View style={styles.rowContainer}>
                <Text style={styles.title}>{currentPicker}</Text>
                <TouchableOpacity onPress={() => setModalVisible(true)}>
                    <Text>{selectedValue !== null ? selectedValue : "Select an option"}</Text>
                </TouchableOpacity>
            </View>

            <Modal
                visible={modalVisible}
                transparent={true}
                animationType="slide"
                onRequestClose={() => setModalVisible(false)}>
                <View>
                    {renderPickerItems(options)}
                    <TouchableOpacity
                        style={styles.closeButton}
                        onPress={() => setModalVisible(false)}>
                        <Text style={styles.closeText}>Close</Text>
                    </TouchableOpacity>
                </View>
            </Modal>
        </View>
    );
};

export const TestPickerView: FC<TestPickerProps> = ({ options, id, selectedLabel, onSelect }) => {
    return (
        <View style={styles.container}>
            <TestPicker options={options} currentPicker={id} onSelect={onSelect} selectedLabel={selectedLabel} />
        </View>
    );
};

const styles = StyleSheet.create({
    closeButton: {
        backgroundColor: 'red',
        paddingVertical: 12,
        paddingHorizontal: 20,
        borderRadius: 5,
        alignItems: 'center',
        justifyContent: 'center',
        marginTop: 20,
        alignSelf: 'center',
    },
    closeText: {
        color: '#fff',
        fontSize: 16,
        fontWeight: 'bold',
    },
    container: {
        justifyContent: 'center',
        alignItems: 'center',
    },
    flatList: {
        width: '100%',
        flexGrow: 1,
        maxHeight: 300,
    },
    rowContainer: {
        flexDirection: 'row',
        alignItems: 'center',
        marginBottom: 5,
    },
    item: {
        paddingVertical: 12,
        paddingHorizontal: 15,
        borderBottomWidth: 1,
        borderBottomColor: '#ddd',
        alignItems: 'flex-start',
    },
    itemText: {
        fontSize: 16,
        color: '#333',
    },
    title: {
        fontWeight: 'bold',
        margin: 5,
        width: 100,
    },
});

AppRegistry.registerComponent('TestPickerView', () => TestPickerView);
