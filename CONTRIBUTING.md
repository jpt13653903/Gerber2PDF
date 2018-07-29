# Contributing to Gerber2PDF

[Suggestions](https://github.com/jpt13653903/Gerber2PDF/issues) and [pull-requests](https://github.com/jpt13653903/Gerber2PDF/pulls) are always welcome.  Consult the ["help wanted"](https://github.com/jpt13653903/Gerber2PDF/issues?q=is%3Aissue+is%3Aopen+label%3A%22help+wanted%22) issues for stuff that is slightly higher priority.

## General Workflow

If you'd like to make contributions to the project, the preferred method for the general public is to create a fork, make the changes and then [create a pull request](https://github.com/jpt13653903/Gerber2PDF/pulls) for that fork.  Project collaborators should create a branch, make changes and then [create a pull request](https://github.com/jpt13653903/Gerber2PDF/pulls) for that branch.

## Dependencies

Please keep dependencies to an absolute minimum.  The vision is that the target user can download a single executable file and then use it.  Any framework or library in use should either be so standard that all target systems are guaranteed to have it (C run-time, Win32 API on Windows, POSIX Threads on Linux, etc.) or statically linked into the project binary.

Another reason is to ease the build process for developers.  Developers should be able to clone the repository and run "make", without installing anything other than the C++ compiler.

