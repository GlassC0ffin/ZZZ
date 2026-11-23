// table.js: your State Table, in code.
//
// Copy your paper table in here row for row. Nothing else in the starter needs to
// change for a new state to work: sense.js applies these rules, fake-sense.js uses
// the names, piece.js reads the result.
//
// signals: the numbers your sensing writes, each 0 to 1. Each partner owns one.
// states:  checked top to bottom; the first row whose rule is true wins.
//          hold is in seconds. Zero changes state on the first matching reading.
//          Keep "nobody" last, so it catches everything the other rows miss.

window.TABLE = {
  signals: ['cm'],

  states: [
    { name: 'Jaxson',  when: s => s.cm <= 30 / 400,  hold: 0 },
    { name: 'Diego',   when: s => s.cm > 30 / 400 && s.cm <= 120 / 400, hold: 0 },
    { name: '#BOI',    when: s => s.cm > 120 / 400 && s.cm <= 200 / 400, hold: 0 },
    { name: 'nobody', when: s => s.cm > 200 / 400, hold: 0 },
  ],
};
