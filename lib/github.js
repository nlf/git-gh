var config = require('./config'),
    Github = require('github'),
    github = new Github({ version: '3.0.0' });

if (config) {
    github.authenticate({ type: 'oauth', token: config.token });
}
module.exports = github;
