var gl = null;

function initializeGL() {
  var canvas = document.querySelector("canvas");

  gl = canvas.getContext("webgl") ||
       canvas.getContext("experimental-webgl");

  if (!gl) {
    return;
  }

  initialize();
  update();
}

function loadShader(id, type) {
  var shaderId = gl.createShader(type);
  if(shaderId == 0) {
    throw("Can not create shader");
  }
  var shaderSource = document.getElementById(id);
  if(!shaderSource) {
    throw("Error: shader script '" + id + "' not found");
  }
  gl.shaderSource(shaderId, shaderSource.text);
  gl.compileShader(shaderId);
  var compiled = gl.getShaderParameter(shaderId, gl.COMPILE_STATUS);
  if (!compiled) {
    var error = gl.getShaderInfoLog(shaderId);
    gl.deleteShader(shaderId);
    throw("Can not load shader");
  }
  return shaderId;
}

function linkProgram(vertexShader, fragmentShader) {
  var programHandle = gl.createProgram();
  if (programHandle == 0) {
    throw("Error creating program.");
  }

  // Bind the vertex shader to the program.
  gl.attachShader(programHandle, vertexShader);

  // Bind the fragment shader to the program.
  gl.attachShader(programHandle, fragmentShader);

  // Bind attributes
  //gl.bindAttribLocation(programHandle, 0, "a_Position");
  //gl.bindAttribLocation(programHandle, 1, "a_Color");

  // Link the two shaders together into a program.
  gl.linkProgram(programHandle);

  // Get the link status.
  var linked = gl.getProgramParameter(programHandle, gl.LINK_STATUS);

  // If the link failed, delete the program.
  if (!linked) {
    gl.deleteProgram(programHandle);
    programHandle = 0;
  }

  return programHandle;
}

var VBO = null;
var programHandle = null;

function initialize() {
  var points = new Float32Array([
    -0.5, 0.0, 0.0, 1.0,
     0.5, 0.0, 0.0, 1.0,
     0.0, 0.5, 0.0, 1.0
  ]);

  gl.clearColor(0.0, 0.0, 0.0, 1.0);

  var vertexShader   = loadShader("vertex_shader", gl.VERTEX_SHADER);
  var fragmentShader = loadShader("fragment_shader", gl.FRAGMENT_SHADER);
  programHandle  = linkProgram(vertexShader, fragmentShader);
  gl.deleteShader(vertexShader);
  gl.deleteShader(fragmentShader);

  var positionHandle = gl.getAttribLocation(programHandle, "a_vertex");
  VBO = gl.createBuffer();
  gl.bindBuffer(gl.ARRAY_BUFFER, VBO);
  gl.bufferData(gl.ARRAY_BUFFER, points, gl.STATIC_DRAW);
  gl.enableVertexAttribArray(positionHandle);
  gl.vertexAttribPointer(positionHandle, 4, gl.FLOAT, false, 0, 0);
  gl.bindBuffer(gl.ARRAY_BUFFER, null);

  window.addEventListener("resize", reshape);
}

function reshape() {
  gl.viewport(0, 0, gl.drawingBufferWidth, gl.drawingBufferHeight);
}

function update() {
  gl.clear(gl.COLOR_BUFFER_BIT);

  gl.useProgram(programHandle);
  gl.bindBuffer(gl.ARRAY_BUFFER, VBO);
  gl.drawArrays(gl.TRIANGLES, 0, 3);
  gl.bindBuffer(gl.ARRAY_BUFFER, null);
  gl.useProgram(null);

  window.setTimeout(update, 1000 / 60);
}

