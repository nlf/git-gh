var fs = require('fs'),
    path = require('path');

function parseConfig(file) {
    var parsed;
    try {
        parsed = JSON.parse(file);
    } catch (e) {
        parsed = false;
    }
    return parsed;
}

function error() {
    console.error('No valid configuration was found');
    console.error();
    console.error('To create one, please run the command: git ghsetup');
    console.error('and follow the prompts');
    process.exit(1);
}

var filePath = path.join(process.env[process.platform === 'win32' ? 'USERPROFILE' : 'HOME'], './.gitgh'),
    exists = fs.existsSync(filePath);

if (!exists) return error();

var stat = fs.statSync(filePath);
if (!stat.isFile()) return error();

var file = fs.readFileSync(filePath, 'utf8');
var config = parseConfig(file);
if (!config || !config.token || !config.user) return error();

module.exports = config;
