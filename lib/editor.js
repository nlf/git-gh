var spawn = require('child_process').spawn,
    exec = require('exec-sync'),
    fs = require('fs');

module.exports = function (callback) {
    var filename = exec('mktemp -t gitgh');
    var editor = process.env.EDITOR || 'vim';
    var proc = spawn(editor, [filename], { stdio: 'inherit' });
    proc.on('exit', function () {
        var file = fs.readFileSync(filename, 'utf8');
        fs.unlinkSync(filename);
        callback(null, file);
    });
}
