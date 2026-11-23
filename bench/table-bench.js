// bench/table-bench.js: the State Table for bench pairs, in code.
//
// bench.html loads this file instead of table.js, so a bench pair's rows live
// here. Same format as table.js: signals are the numbers sense-serial.js writes
// (each 0 to 1, named in its SIGNALS), states are checked top to bottom and the
// first true row wins.
//
// The Nano changes state as soon as the filtered reading crosses a rule.
// Keep "nobody" last, so it catches everything the other rows miss.

window.TABLE = {
  // cm is normalized to 0.0 at contact and 1.0 at 400 cm.
  signals: ['cm'],

  states: [
    { name: 'Jaxson',  when: s => s.cm <= 30 / 400,  hold: 0 },
    { name: 'Diego',   when: s => s.cm > 30 / 400 && s.cm <= 120 / 400, hold: 0 },
    { name: '#BOI',    when: s => s.cm > 120 / 400 && s.cm <= 200 / 400, hold: 0 },
    { name: 'nobody', when: s => s.cm > 200 / 400, hold: 0 },
  ],
};
