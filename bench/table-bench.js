// bench/table-bench.js: the State Table for bench pairs, in code.
//
// bench.html loads this file instead of table.js, so a bench pair's rows live
// here. Same format as table.js: signals are the numbers sense-serial.js writes
// (each 0 to 1, named in its SIGNALS), states are checked top to bottom and the
// first true row wins.
//
// hold is in seconds: how long a rule must stay true before the state changes.
// It is a tradeoff you choose per row. A short hold feels instant but flickers
// when a reading wobbles near its line; a long hold is calm but slow. These
// example rows use 0.5, 1 and 2, the same as the working reference build.
// Keep "nobody" last, so it catches everything the other rows miss.

window.TABLE = {
  // cm is normalized to 0.0 at contact and 1.0 at 400 cm.
  signals: ['cm'],

  states: [
    { name: 'Jaxson',  when: s => s.cm <= 30 / 400,  hold: 1.0 },
    { name: 'Diego',   when: s => s.cm > 30 / 400 && s.cm <= 120 / 400, hold: 2.0 },
    { name: 'Brennan', when: s => s.cm > 120 / 400 && s.cm <= 200 / 400, hold: 2.0 },
    { name: 'nobody',  when: s => s.cm > 200 / 400, hold: 3.0 },
  ],
};
