Github extensions for Git
=======

NOTE
====
This branch is the C rewrite. Most of it does not work or doesn't exist. Don't use it.

To install:

First, install a compiler. You're on your own for that. Also, install libcurl, json-c, and iniparser C libraries.
This currently assumes you already have a ~/.gitgh file created with your config in it. I don't know what will happen if you don't.

```
make
sudo make install // just kidding, there's no install, but the binaries will be in the "bin" directory
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
