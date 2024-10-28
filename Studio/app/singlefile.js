// Usage: node build.js
// Description: This script creates a single HTML file with all CSS/JS/PNGs inlined. This is useful for embedding the app in a single file for WebView2.

const fs = require('fs');

fs.readFile('./build/index.html', 'utf8', (err, data) => {
    if (err) {
        console.error(err);
        return;
    }

    console.log('Creating single file...');

    // Remove all script tags
    data = data.replace(/<script.*?>.*?<\/script>/gs, '');

    // Remove all CSS links
    data = data.replace(/<link.*?stylesheet.*?>/gs, '');

    // Add script tag with static/js/*.js contents
    const firstJSFileInFolder = fs.readdirSync('./build/static/js')[0];
    const jsFileContents = fs.readFileSync(`./build/static/js/${firstJSFileInFolder}`, 'utf8');
    data = data.replace('</body>', `<script>${jsFileContents}</script></body>`);

    // Add style tag with static/css/*.css contents
    const firstCSSFileInFolder = fs.readdirSync('./build/static/css')[0];
    const cssFileContents = fs.readFileSync(`./build/static/css/${firstCSSFileInFolder}`, 'utf8');
    data = data.replace('</head>', `<style>${cssFileContents}</style></head>`);

    // Inline all PNGs
    const pngFiles = fs.readdirSync('./public/img/').filter(file => file.endsWith('.png'));
    pngFiles.forEach(pngFile => {
        const base64 = fs.readFileSync(`./public/img/${pngFile}`, 'base64');
        data = data.replace(`"img/${pngFile}"`, `"data:image/png;base64,${base64}"`);
    });

    if (!fs.existsSync('./studio.html')) {
        fs.writeFileSync('./studio.html', '');
    }

    // Save to release/singlefile.html
    fs.writeFileSync('./studio.html', data);

    console.log('Single file created at ./studio.html');

    // Start chrome with the single file
    require('child_process').exec(`start chrome ${__dirname}/studio.html`);
});