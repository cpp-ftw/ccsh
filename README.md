# ccsh - a C++ shell

#### Rough plan

1. Create a library that implements shell-like syntax and functionality, e.g.
`ccsh::shell("ls", {"-lah", "--color=auto"}) | ccsh::shell("grep", "rwx")`
2. Add ccsh::shell wrappers for coreutils (e.g. `ccsh::ls()` for `ccsh::shell("ls")`)
3. Pipe this into a C++ interpreter, e.g. cling (https://root.cern.ch/cling), which makes a complete C++ shell.

### TODO

- Add a normal README.md

- ~~Add a flag to command_base to run command in the dtor.~~
- ~~Fix output redirection permission issue.~~

- ~~Improve type safety via `boost::filesystem` (later `std::filesystem`) and `operator""_p`~~
- ~~Invent a fancy syntax for command switches. Giving them as strings (e.g. `ccsh::ls("-lah")`) lacks static checking and type safety.~~
- ~~The same goes for argument *pairs* like `cssh::gcc("...", "-o", "file.o")`~~

`ccsh::ls().l().a().h()` and `ccsh::gcc("file.c").c().o("file.o")` seems to be the best.

- ~~Add operator< for input redirection~~
- ~~Add operator>= for error redirection~~
- ~~Add operator>> for output appending~~
- ~~Add operator>>= for error appending~~
- ~~Add `operator||` and `operator&&`~~

- ~~Add `command operator>(command, std::vector<std::string>&);` and same for <, >>, >=, >>=~~
- ~~Add `command operator>(command, std::string&);` and same for <, >>, >=, >>=~~
- ~~Add `command operator>(command, command_raw_functor);` and same for <, >=~~
- ~~Add `command operator>(command, command_line_functor);` and same for >=~~
- Add `command operator<(command, command_line_functor);`
