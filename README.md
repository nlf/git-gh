Github extensions for Git
=======

This project hasn't been tested on anything other than OS X 10.8.2. Feel free to try it elsewhere and open an issue if it doesn't work.

To install:

First, install a compiler. You're on your own for that. Also, install homebrew because it's the easiest way to get dependencies.

```
brew install libcurl json-c iniparser
make
sudo make install
```

Commands
========

```
git list [filter]
```

List open issues and pull requests. The "filter" parameter can be any of the following

* all
* prs - only show pull requests
* issues - only show issues
* mine - only items that are assigned to you
* unassigned - only items that are unassigned
* milestone x - only items that are associated with milestone "x" (case sensitive)

```
git detail <id>
```

Get detail on issue/pull request with the given id

```
git claim <id>
```

Assign the given issue/pull request to yourself

```
git assign <id> <username>
```

Assign the given issue/pull request to username

```
git close <id>
```

Close the issue/pull request. This will *not* merge a pull request.

```
git comment <id> -m 'comment body'
```

Add a comment to the issue.

```
git issue -t 'title for issue' -m 'body of issue'
```

Open a new issue with the given title and body.

```
git pr -t 'title for pull request' -m 'body of pull request' [-b base] [-h head]
```

Open a new pull request. The "base" parameter is the branch you'd like your branch to be merged into, while the "head" parameter is the branch that you would like to merge. If unspecified, base will be set to "master" and head will be set to the current branch.
