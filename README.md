# MPlus [![Build Status](https://travis-ci.org/plankp/M-Plus.svg?branch=master)](https://travis-ci.org/plankp/M-Plus) [![Build status](https://ci.appveyor.com/api/projects/status/825xhlrdh0eahyrf/branch/master?svg=true)](https://ci.appveyor.com/project/plankp/m-plus/branch/master) [![Codacy Badge](https://api.codacy.com/project/badge/Grade/ebae1915df4a46fe91ebecf63d1ad25c)](https://www.codacy.com/app/plankp/M-Plus?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=plankp/M-Plus&amp;utm_campaign=Badge_Grade)

A programming language that took a lot from [NtShell](https://github.com/plankp/NtShell)

(and no, just in case you are wondering, the syntax is not 100% compatible so most code will need to be rewritten)

## To build M+

You need:
*  CMake >= 3.0.0
*  C99 supporting compiler
*  C++11 supporting compiler

(tested with visual studio 2013, clang 3.6, and gcc 4.8)

```
% mkdir build
% cd build
% cmake ..
% make
```

## To run M+ tests

`% ./tests`

## To run M+ interpreter

`% ./mplus [MPlus script]`

## To code in M+

See the *__sample__* code in the `sample` folder. That hopefully helps! 
