var ini = require('ini'),
    exec = require('exec-sync'),
    fs = require('fs'),
    path = require('path');

function error() {
    console.error('This doesn\'t appear to be a git repository');
    process.exit(1);
}

function noRemotes() {
    console.error('This repository has no configured remotes');
    process.exit(1);
}

var result = exec('git rev-parse --show-toplevel', true);
if (!result.stdout) return error();

var config = path.join(result.stdout, '.git', 'config');
var exists = fs.existsSync(config);
if (!exists) return error();

var stat = fs.statSync(config);
if (!stat.isFile()) return error();

var file = ini.parse(fs.readFileSync(config, 'utf8'));
var remotes = [],
    re = /^remote\s+\"(\w+)\"/,
    repoRe = /^(git@|https:\/\/)github\.com(:|\/)([\w-_]+)\/([\w-_]+)\.git/,
    parsed;
Object.keys(file).forEach(function (key) {
    parsed = re.exec(key);
    if (parsed) {
        var repo = repoRe.exec(file[key].url);
        if (repo) {
            var remote = { remote: parsed[1], user: repo[3], repo: repo[4] };
            if (parsed[1] === 'origin') {
                remotes.unshift(remote);
            } else {
                remotes.push(remote);
            }
        }
    }
});
if (!remotes.length) return noRemotes();

module.exports = remotes;
