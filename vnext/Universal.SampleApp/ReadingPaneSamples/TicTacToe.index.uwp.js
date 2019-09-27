/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 * @format
 * @flow
 */

import React, {Component} from 'react';
import {
  Alert,
  AppRegistry,
  NativeModules,
  StyleSheet,
  Text,
  TouchableWithoutFeedback,
  View,
  NativeEventEmitter,
} from 'react-native';

class Square extends Component {
  render() {
    let styleColor = styles.emptySquare;
    if (this.props.value === 'X') {
      // X is blue
      styleColor = styles.XSquare;
    } else if (this.props.value === 'O') {
      // O is green
      styleColor = styles.OSquare;
    }

    let styleBoard = styles.emptySquare;
    if (this.props.enabled) {
      if (this.props.xIsNext) {
        styleBoard = styles.EnabledBoardX;
      } else {
        styleBoard = styles.EnabledBoardO;
      }
      // styleBoard = styles.EnabledBoard;
    }

    return (
      <TouchableWithoutFeedback onPress={() => this.props.onPress()}>
        <View
          style={[styles.square, styleColor, styleBoard]}
          underlayColor="#f1c40f"
        />
      </TouchableWithoutFeedback>
    );
  }
}

class SmallBoard extends Component {
  renderSquare(i) {
    return (
      <Square
        value={this.props.squares[i]}
        enabled={this.props.enabled}
        xIsNext={this.props.xIsNext}
        onPress={() => this.props.onPress(i)}
      />
    );
  }

  render() {
    let status; // = this.props.board;
    if (this.props.winner) {
      status = 'Winner!';
    } //: ' + NameFromInternal(winner);
    else if (this.props.enabled) {
      status = 'Next play';
    }

    return (
      <View style={{flexDirection: 'column', padding: 3}}>
        <Text style={{fontSize: 8, height: 10}}>{status}</Text>
        <View style={{flexDirection: 'row'}}>
          {this.renderSquare(0)}
          {this.renderSquare(1)}
          {this.renderSquare(2)}
        </View>
        <View style={{flexDirection: 'row'}}>
          {this.renderSquare(3)}
          {this.renderSquare(4)}
          {this.renderSquare(5)}
        </View>
        <View style={{flexDirection: 'row'}}>
          {this.renderSquare(6)}
          {this.renderSquare(7)}
          {this.renderSquare(8)}
        </View>
      </View>
    );
  }
}

class BigBoard extends Component {
  constructor() {
    super();

    const squares = Array(9).fill(null);

    this.state = {
      zzhistory: [
        {
          squares: Array(9).fill(null),
        },
      ],
      allBoards: [
        {squares: squares.slice()},
        {squares: squares.slice()},
        {squares: squares.slice()},
        {squares: squares.slice()},
        {squares: squares.slice()},
        {squares: squares.slice()},
        {squares: squares.slice()},
        {squares: squares.slice()},
        {squares: squares.slice()},
      ],
      winners: Array(9).fill(false),
      gameWinner: null,
      allBoardsz: Array(9).fill({squares: Array(9).fill(squares.slice())}),
      noMovesYet: true,
      xIsNext: true,
      iAmX: true,
      lastBoard: -1,
      lastI: -1,
      BluePlayer: '', // X
      BluePlayerFriendly: '',
      GreenPlayer: '', // O
      GreenPlayerFriendly: '',

      subject: 'Loading...',
      msgs: Array(0),

      inited: false,
    };

    NativeModules.ReadingPane.CurrentConversation(js =>
      this.LoadedConversation(js),
    );

    const rpEmitter = new NativeEventEmitter(NativeModules.ReadingPane);
    rpEmitter.addListener('readingPaneChanged', () =>
      this.ReadingPaneChanged(),
    );
  }

  ReadingPaneChanged() {
    NativeModules.ReadingPane.CurrentConversation(js =>
      this.LoadedConversation(js),
    );
  }

  LoadedConversation(js) {
    if (js.result === false) {
      return;
    }

    if (!this.state.inited) {
      let start = CalcStartState(js);
      this.setState(start);
    }

    this.setState({
      subject: js.Subject,
      msgs: js.Messages,
    });
  }

  handleClick(board, i) {
    if (this.state.lastI !== -1 && this.state.lastI !== board) {
      return;
    }

    const current = this.state.allBoards[board];
    const squares = current.squares.slice();
    if (this.state.gameWinner || squares[i]) {
      return;
    }

    squares[i] = this.state.xIsNext ? 'X' : 'O';

    const localAllBoards = this.state.allBoards.slice();
    localAllBoards[board].squares[i] = this.state.xIsNext ? 'X' : 'O';

    const winnersState = calculateWinners(localAllBoards);

    // setState is async so make a copy here, todo use setstate callback
    let saveState = {
      allBoards: localAllBoards,
      winners: winnersState.winners,
      gameWinner: winnersState.gameWinner,
      lastI: i,
      lastBoard: board,
      xIsNext: !this.state.xIsNext,
      BluePlayer: this.state.BluePlayer,
      GreenPlayer: this.state.GreenPlayer,
    };

    this.setState({
      allBoards: localAllBoards,
      winners: winnersState.winners,
      gameWinner: winnersState.gameWinner,
      xIsNext: !this.state.xIsNext,
      noMovesYet: false,
      lastBoard: board,
      lastI: i,
    });

    const boardState = ComputeBoardState(saveState);
    NativeModules.ReadingPane.SendMail(
      '<HTML><BODY>&lt;BoardState&gt;' +
        boardState +
        '&lt;/BoardState&gt;</BODY></HTML>',
    );
  }

  renderBoard(board) {
    const current = this.state.allBoards[board];
    const boardDesc =
      board + ' lB:' + this.state.lastBoard + ' lI:' + this.state.lastI;
    let boardEnabled = this.state.lastI === -1 || this.state.lastI === board;
    if (this.state.gameWinner != null) {
      boardEnabled = false;
    }
    return (
      <SmallBoard
        squares={current.squares}
        board={boardDesc}
        enabled={boardEnabled}
        winner={this.state.winners[board]}
        xIsNext={this.state.xIsNext}
        onPress={i => this.handleClick(board, i)}
      />
    );
  }

  setBoard(text) {
    const boardState = ParseBoardState(text);
    if (boardState.fail) {
      return;
    }
    this.setState({
      allBoards: boardState.allBoards,
      lastBoard: boardState.lastBoard,
      lastI: boardState.lastI,
      xIsNext: boardState.xIsNext,
    });
    return;
  }

  render() {
    let status;
    status = 'Next player: ' + NameFromInternal(this.state.xIsNext ? 'X' : 'O');
    if (this.state.gameWinner) {
      status =
        'Winner:' +
        (this.state.gameWinner === 'X'
          ? this.state.GreenPlayer
          : this.state.BluePlayer);
    } else if (this.state.noMovesYet) {
      status = 'Click a square to start';
    } else if (
      (this.state.xIsNext && this.state.iAmX) ||
      (!this.state.xIsNext && !this.state.iAmX)
    ) {
      status = "It's your move!";
    } else {
      status =
        'Waiting for ' +
        (this.state.iAmX ? this.state.GreenPlayer : this.state.BluePlayer);
    }

    let status2 = 'iAmX(blue):' + this.state.iAmX;
    let status3 = 'Blue: ' + this.state.BluePlayer;
    let status4 = 'Green:' + this.state.GreenPlayer;

    //const boardState = ComputeBoardState(this.state.allBoards, this.state.lastBoard, this.state.lastI);
    const boardState = ComputeBoardState(this.state);

    return (
      <View style={{flexDirection: 'row'}}>
        <View style={{flexDirection: 'column', padding: 10, width: 180}}>
          <Text style={{fontSize: 10}}> {status} </Text>
          <Text style={{fontSize: 10}} />
          <Text style={{fontSize: 10}}> {status2} </Text>
          {/* Players */}
          <Text style={{fontSize: 10}}> {status3} </Text>
          <Text style={{fontSize: 10}}> {status4} </Text>
          {/* Board state */}
          <View style={{height: 10}} />
          <Text style={{fontSize: 10}}> Board State: </Text>
          <Text style={{fontSize: 10}}> {boardState} </Text>
        </View>
        <View style={{flexDirection: 'column'}}>
          <View style={{flexDirection: 'row'}}>
            {this.renderBoard(0)}
            {this.renderBoard(1)}
            {this.renderBoard(2)}
          </View>
          <View style={{flexDirection: 'row'}}>
            {this.renderBoard(3)}
            {this.renderBoard(4)}
            {this.renderBoard(5)}
          </View>
          <View style={{flexDirection: 'row'}}>
            {this.renderBoard(6)}
            {this.renderBoard(7)}
            {this.renderBoard(8)}
          </View>
        </View>
      </View>
    );
  }
}

function CalcStartState(js) {
  // Find me:
  let meEmail = js.Account.Email;
  let meDisplay = js.Account.DisplayName;

  // Find latest board
  let loadInfo = FindAndLoadBoard(js, meEmail);
  let foundBoard = loadInfo.result;

  if (foundBoard) {
    // If blank green, become player 2
    if (loadInfo.BluePlayer !== meEmail && loadInfo.GreenPlayer === '') {
      loadInfo.GreenPlayer = meEmail;
    }

    return {
      allBoards: loadInfo.allBoards,
      winners: loadInfo.winners,
      gameWinner: loadInfo.gameWinner,
      lastBoard: loadInfo.lastBoard,
      lastI: loadInfo.lastI,
      xIsNext: loadInfo.xIsNext,
      GreenPlayer: loadInfo.GreenPlayer,
      BluePlayer: loadInfo.BluePlayer,
      iAmX: loadInfo.BluePlayer === meEmail,
      noMovesYet: false,
    };
  }

  return {
    BluePlayer: meEmail,
    BluePlayerFriendly: meDisplay,
  };
}

function calculateWinners(allBoards) {
  let winners = Array(9).fill(false);
  let gameWinner = null;
  for (let i = 0; i < 9; i++) {
    let winner = calculateWinner(allBoards[i].squares);
    winners[i] = winner != null;
    if (winners[i]) {
      gameWinner = winner;
    }
  }

  return {winners: winners, gameWinner: gameWinner};
}

function htmlDecode(str) {
  var map = {amp: '&', lt: '<', gt: '>', quot: '"', '#039': "'"};
  str = str.replace(/&([^;]+);/g, (m, c) => map[c]);
  str = str.replace('<span>', '');
  return str;
}

function FindAndLoadBoard(js) {
  // Look for Board data
  for (let i = 0; i < js.Messages.length; i++) {
    let msgBody = htmlDecode(js.Messages[i].HTMLBodyBytes);
    let start = msgBody.indexOf('<BoardState>');
    if (start !== -1) {
      let end = msgBody.indexOf('</BoardState>', start);
      var boardState = msgBody
        .substring(start + 12)
        .substring(0, end - start - 12);

      try {
        let s = ParseBoardState(boardState);
        return s;
      } catch (e) {
        return {result: false};
      }
    }
  }

  return {result: false};
}

function calculateWinner(squares) {
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
  return null;
}

function NameFromInternal(internal) {
  if (internal === 'X') {
    return 'Blue';
  }
  if (internal === 'O') {
    return 'Green';
  }

  return null;
}

function ComputeBoardState(state) {
  let json = JSON.stringify({
    allBoards: state.allBoards,
    lastIndex: state.lastI,
    lastBoard: state.lastBoard,
    xIsNext: state.xIsNext,
    X: state.BluePlayer,
    O: state.GreenPlayer,
  });
  return json;
}

function ParseBoardState(text) {
  let boardState = JSON.parse(text);
  let winnersState = calculateWinners(boardState.allBoards);
  return {
    allBoards: boardState.allBoards,
    lastBoard: boardState.lastBoard,
    lastI: boardState.lastIndex,
    xIsNext: boardState.xIsNext,
    BluePlayer: boardState.X,
    GreenPlayer: boardState.O,
    winners: winnersState.winners,
    gameWinner: winnersState.gameWinner,
    result: true,
  };
}

export default class Bootstrap extends Component {
  _handleButtonPress = () => {
    Alert.alert('Button pressed!', 'You did it!');
  };

  render() {
    return (
      <View style={styles.container}>
        <BigBoard />
      </View>
    );
  }
}

const styles = StyleSheet.create({
  container: {
    flex: 1,
    justifyContent: 'center',
    alignItems: 'center',
    //    backgroundColor: '#F5FCFF',
  },
  welcome: {
    fontSize: 20,
    textAlign: 'center',
    margin: 10,
  },
  instructions: {
    textAlign: 'center',
    color: '#333333',
    marginBottom: 5,
  },

  square: {
    borderWidth: 2,
    borderColor: 'black',
    height: 18,
    marginRight: -2,
    marginTop: -2,
    padding: 0,
    width: 18,
    backgroundColor: 'white',
  },
  emptySquare: {},
  XSquare: {
    backgroundColor: 'blue',
  },
  OSquare: {
    backgroundColor: 'green',
  },
  EnabledBoard: {
    borderColor: 'green',
  },
  EnabledBoardX: {
    borderColor: 'blue',
  },
  EnabledBoardO: {
    borderColor: 'green',
  },
});

AppRegistry.registerComponent('Bootstrap', () => Bootstrap);
