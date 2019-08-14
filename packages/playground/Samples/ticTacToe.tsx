/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 * @format
 */
import * as React from 'react';
import {
  Button,
  SafeAreaView,
  Text,
  TouchableHighlight,
  View,
} from 'react-native';

const newGameState = {
  squares: Array(9).fill(null),
  xIsNext: true,
};

type SquareValue = null | 'X' | 'O';

export default class Bootstrap extends React.Component<
  {},
  {squares: SquareValue[]; xIsNext: boolean}
> {
  constructor(props: {}) {
    super(props);
    this.state = newGameState;
  }

  whoseTurn() {
    return this.state.xIsNext ? 'X' : 'O';
  }

  onNewGame() {
    this.setState(newGameState);
  }

  onMove(i: number) {
    let newSquares = this.state.squares.slice();
    const turn = this.whoseTurn();
    if (this.state.squares[i] || winner(this.state.squares)) {
      return null;
    }
    newSquares[i] = turn;
    this.setState({
      squares: newSquares,
      xIsNext: !this.state.xIsNext,
    });
  }

  render() {
    const style = {
      backgroundColor: 'beige',
      flex: 1,
      alignItems: 'center' as 'center',
    };

    return (
      <SafeAreaView style={style}>
        <Board squares={this.state.squares} onMove={i => this.onMove(i)} />
        <Status
          turn={this.whoseTurn()}
          winner={winner(this.state.squares)}
          onNewGame={() => this.onNewGame()}
        />
      </SafeAreaView>
    );
  }
}

const Board = (props: {
  squares: SquareValue[];
  onMove: (index: number) => void;
}) => {
  const {squares, onMove} = props;
  return (
    <View style={{flex: 1, justifyContent: 'center'}}>
      <Row squares={squares} startIndex={0} onMove={onMove} />
      <Row squares={squares} startIndex={3} onMove={onMove} />
      <Row squares={squares} startIndex={6} onMove={onMove} />
    </View>
  );
};

const Row = (props: {
  squares: SquareValue[];
  startIndex: number;
  onMove: (index: number) => void;
}) => {
  const {squares, startIndex, onMove} = props;
  return (
    <View style={{flexDirection: 'row'}}>
      <Square label={squares[startIndex]} onPress={() => onMove(startIndex)} />
      <Square
        label={squares[startIndex + 1]}
        onPress={() => onMove(startIndex + 1)}
      />
      <Square
        label={squares[startIndex + 2]}
        onPress={() => onMove(startIndex + 2)}
      />
    </View>
  );
};

const Square = (props: {label: SquareValue; onPress: () => void}) => {
  const {label, onPress} = props;
  const style = {
    width: 50,
    height: 50,
    borderColor: 'black',
    borderWidth: 1,
    justifyContent: 'center' as 'center',
  };

  const textStyle = {
    textAlign: 'center' as 'center',
    fontSize: 36,
    fontWeight: 'bold' as 'bold',
  };

  return (
    <TouchableHighlight style={style} onPress={onPress}>
      <Text style={textStyle}>{label ? label : ''}</Text>
    </TouchableHighlight>
  );
};

const Status = (props: {
  turn: 'X' | 'O';
  winner: null | undefined | 'X' | 'O';
  onNewGame: () => void;
}) => {
  const {turn, winner, onNewGame} = props;
  const text =
    winner === null
      ? 'Tie game :-/'
      : winner !== undefined
        ? winner + ' wins!'
        : turn + "'s turn";

  return (
    <View style={{flex: 1}}>
      <Text style={{fontSize: 36, textAlign: 'center'}}>{text}</Text>
      <Button title="Start new game" onPress={onNewGame} />
    </View>
  );
};

const winner = (squares: SquareValue[]) => {
  const lines = [
    [0, 1, 2],
    [3, 4, 5],
    [6, 7, 8],
    [0, 3, 6],
    [1, 4, 7],
    [2, 5, 8],
    [0, 4, 8],
    [2, 4, 6],
  ];

  for (let i = 0; i < lines.length; i++) {
    const [a, b, c] = lines[i];
    if (squares[a] && squares[a] === squares[b] && squares[a] === squares[c]) {
      return squares[a];
    }
  }
  if (squares.indexOf(null) === -1) {
    return null;
  } // tie game
  return undefined;
};
