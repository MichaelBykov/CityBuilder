import { readFile } from 'fs/promises'
import { createWriteStream, readdirSync, statSync } from 'fs'
import { basename, parse, dirname, join, resolve } from 'path'
import { PNG } from 'pngjs'
import { Command } from 'commander'

/**
 * Converts a hex color code to RGBA
 * @param {string} hex
 * @returns {{r: number, g: number, b: number, a: number}}
 */
function hexToRGBA(hex) {
  const r = parseInt(hex.slice(0, 2), 16);
  const g = parseInt(hex.slice(2, 4), 16);
  const b = parseInt(hex.slice(4, 6), 16);
  const a = 255; // Fully opaque
  return { r, g, b, a };
}

/**
 * Loads a .obj.meta file and converts it to a .png texture image
 * @param {string} path 
 */
async function convert_meta(path) {
  // Prepare output file path
  const { name: inFileName } = parse(basename(path))
  const outFilePath = join(dirname(path), `${inFileName}.png`)

  // Read the .obj.meta file
  const contents = await readFile(path, 'utf8')
  const meta = JSON.parse(contents)
  const colorCodes = meta['Materials'].map((material) => material['Color'].replace('#', ''))

  if (colorCodes.length === 0) {
    throw new Error(`No materials found in ${path}`)
  }
  
  const size = colorCodes.length
  const length = Math.sqrt(colorCodes.length)
  const png = new PNG({ width: length, height: length })

  // Set the pixel colors in the PNG image
  for (let y = 1; y <= length; y++) {
    for (let x = 0; x < length; x++) {
      const colorIndex = size - (y * length) + x
      // console.log(`x: ${x}, y: ${y}, colorIndex: ${colorIndex}`)
      const { r, g, b, a } = hexToRGBA(colorCodes[colorIndex])
      const idx = (length * (y - 1) + x) << 2

      png.data[idx] = r
      png.data[idx + 1] = g
      png.data[idx + 2] = b
      png.data[idx + 3] = a
    }
  }

  // Write the PNG image to a file
  return new Promise<string>((resolve, reject) => {
    png.pack()
      .pipe(createWriteStream(outFilePath))
      .on('finish', () => resolve(inFileName))
      .on('error', (err) => reject({inFileName, err}))
  })
}

/**
 * Recursively walks a directory and calls a callback for each file path
 * @param {string} dir Dir to search
 * @param {function(path)} callback 
 */
function walk(dir, callback) {
  readdirSync(dir).forEach(file => {
    const filePath = join(dir, file)
    if (statSync(filePath).isDirectory()) {
      walk(filePath, callback)
    } else {
      callback(filePath)
    }
  })
}

/**
 * Converts a .obj.meta file or all .obj.meta files in a directory to .png texture images
 * @param {string} path - Path to a .obj.meta file or dir to search for .obj.meta files
 */
async function convert(pathInput) {
  const path = resolve(pathInput)
  const { ext } = parse(path)
  const conversions = []

  if (ext === '.meta') {
    conversions.push(convert_meta(path))
  } else {
    walk(path, async (filePath) => {
      const { ext } = parse(filePath)
      if (ext === '.meta') {
        conversions.push(convert_meta(filePath))
      }
    })
  }

  for (const [i, conversion] of conversions.entries()) {
    conversion.then(file => {
      console.log(`${i + 1}/${conversions.length}: Converted ${file}.obj.meta to ${file}.png`)
    }).catch(({file, err}) => {
      console.log(`${i + 1}/${conversions.length}: Error during conversion of ${file}.obj.meta, ${err?.message}`)
    })
  }
}

const program = new Command()
program
  .arguments('<input>')
  .action(convert)
  .parse(process.argv)
