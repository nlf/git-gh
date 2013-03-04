Github extensions for Git
=======

This project hasn't been tested on anything other than OS X 10.8.2. Feel free to try it elsewhere and open an issue if it doesn't work.

Requirements
============
* json-c == 0.10
* iniparser == 3.1
* libcurl >= 1.4

Installation on OSX
===================

First, install a compiler. You're on your own for that. Also, install homebrew because it's the easiest way to get dependencies.

```
brew install libcurl json-c iniparser
git clone git://github.com/nathan-lafreniere/git-gh.git
cd git-gh
make
sudo make install
```

Installation on Ubuntu
======================
```
wget http://ndevilla.free.fr/iniparser/iniparser-3.1.tar.gz
tar xf iniparser-3.1.tar.gz
cd iniparser
make
sudo cp libiniparser* /usr/lib
sudo cp src/*.h /usr/include

wget https://github.com/downloads/json-c/json-c/json-c-0.10.tar.gz
tar xf json-c-0.10.tar.gz
cd json-c-0.10
./configure
make
sudo make install
sudo cp json_object_iterator.h /usr/local/include/json/      *NOTE* this file gets missed by make install, so you must copy it manually

git clone git://github.com/nathan-lafreniere/git-gh.git
cd git-gh
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

```
git review <id>
```

Review a diff for a pull request

```
git accept <id>
```

Merge a pull request
