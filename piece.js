// piece.js: reads window.sense inside its own frame loop and answers.
//
// What is here now is a gauge: a colour per state and a circle that follows one
// signal. A gauge proves the sensing works and nothing more. Replace all of it
// with the piece your State Table describes. Read the object, never the camera.

const canvas = document.querySelector('#piece');
const ctx = canvas.getContext('2d');

function resize() {
  canvas.width = innerWidth * devicePixelRatio;
  canvas.height = innerHeight * devicePixelRatio;
}
addEventListener('resize', resize);
resize();

const COLORS = {
  Jaxson: '#c83a31',
  Diego: '#3a9d63',
  '#BOI': '#4a79c7',
  nobody: '#0b0b0d',
};

// A state with no colour above still gets a bright one of its own, picked from
// its name, so a new row in table.js is visible the moment it fires.
function colorFor(state) {
  if (COLORS[state]) return COLORS[state];
  let hash = 0;
  for (const ch of state) hash = (hash * 31 + ch.charCodeAt(0)) % 360;
  return `hsl(${hash}, 85%, 50%)`;
}

function frame() {
  const { state, signals, since } = window.sense;
  const held = performance.now() - since; // ms in this state
  const cm = signals.cm === undefined ? 400 : 400 * signals.cm;

  ctx.fillStyle = colorFor(state);
  ctx.fillRect(0, 0, canvas.width, canvas.height);

  ctx.save();
  ctx.fillStyle = '#f8f3e8';
  ctx.textAlign = 'center';
  ctx.textBaseline = 'middle';
  ctx.font = `700 ${Math.min(canvas.width, canvas.height) * 0.16}px system-ui, sans-serif`;
  ctx.fillText(state.toUpperCase(), canvas.width / 2, canvas.height * 0.43);
  ctx.font = `500 ${Math.min(canvas.width, canvas.height) * 0.07}px ui-monospace, monospace`;
  ctx.fillText(`cm ${Number.isFinite(cm) ? cm.toFixed(1) : '400.0'}`, canvas.width / 2, canvas.height * 0.58);
  ctx.restore();

  requestAnimationFrame(frame);
}
requestAnimationFrame(frame);
