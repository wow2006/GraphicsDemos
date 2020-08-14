var gl = null;

function initialize(event) {
  var canvas = document.querySelector("canvas");

  gl = canvas.getContext("webgl") ||
       canvas.getContext("experimental-webgl");

  if (!gl) {
    return;
  }

  gl.clearColor(0.0, 0.0, 1.0, 1.0);

  window.addEventListener("resize", reshape);

  update();
}

function reshape() {
  gl.viewport(0, 0, gl.drawingBufferWidth, gl.drawingBufferHeight);
}

function update() {
  gl.clear(gl.COLOR_BUFFER_BIT);

  window.setTimeout(update, 1000 / 60);
}

