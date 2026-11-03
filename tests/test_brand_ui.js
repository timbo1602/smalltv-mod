const assert = require('node:assert/strict');
const fs = require('node:fs');
const vm = require('node:vm');

const html = fs.readFileSync('src/webui.html', 'utf8');
const pixelCode = html.slice(html.indexOf('function defaultPixelHex'), html.indexOf('var SCREEN_NAMES'));
const screenCode = html.slice(html.indexOf('var SCREEN_NAMES'), html.indexOf('// field groups'));
const context = {
  Uint8Array,
  Math,
  parseInt,
  PIX: new Uint8Array(1024),
  DEFAULT_PIXEL_ROWS: [0xffffffff,0xffffffff,0xffffffff,0xffffffff,0xffffffff,0xf80000ff,0xf800003f,0xf800001f,0xf800001f,0xf800000f,0xf81ffc0f,0xf81ffc0f,0xf81ffc0f,0xf81ffc0f,0xf81f000f,0xf81f001f,0xf81f001f,0xf81f001f,0xf81f000f,0xf81ffc0f,0xfc1ffc0f,0xfc1ffc0f,0xfe1ffc0f,0xfe1f000f,0xff1f001f,0xff9f001f,0xffff003f,0xffff00ff,0xffffffff,0xffffffff,0xffffffff,0xffffffff],
  SCREEN_ORDER: [],
  gc: () => false,
  sc: () => {},
  gv: () => 'brand',
  esc: value => String(value),
  $: () => null,
  document: {createElement: () => ({})},
  Image: class {},
  URL: {},
  Blob: class {}
};
vm.createContext(context);
vm.runInContext(pixelCode, context);

const expected = context.defaultPixelHex();
assert.equal(expected.length, 256);
assert.match(expected, /^[0-9a-f]{256}$/);
context.pixelFromHex(expected);
assert.equal(context.pixelHex(), expected, 'bitmap hex round-trip');
assert.equal(context.PIX.reduce((sum, bit) => sum + bit, 0), 677,
  'default bitmap exactly matches the supplied black-pixel count');

// A horizontal flip twice and vertical flip twice must be lossless.
context.pixelFlip(true); context.pixelFlip(true);
assert.equal(context.pixelHex(), expected, 'horizontal flip round-trip');
context.pixelFlip(false); context.pixelFlip(false);
assert.equal(context.pixelHex(), expected, 'vertical flip round-trip');

vm.runInContext(screenCode, context);
context.renderScreens = () => {};
context.SCREEN_ORDER = ['brand', 'stocks', 'clock'];
context.moveScreen(2, -1);
assert.deepEqual(Array.from(context.SCREEN_ORDER), ['brand', 'clock', 'stocks']);
context.moveScreen(0, -1);
assert.deepEqual(Array.from(context.SCREEN_ORDER), ['brand', 'clock', 'stocks'], 'bounds-safe move');

console.log('brand bitmap and screen ordering tests passed');
