# Pecel

An Experimental `Distributed Key Value Store` written in `C`

### Building

Requirements:
- `gcc` compiler https://gcc.gnu.org/
- `cmake` https://cmake.org/download/

Build from source:
```shell
$ make build
```

### Running
The build results will be in the `bin` folder
```shell
$ ./bin/pecel pecel.conf 
```

### TODO
- add auth feature
- more validation and error handling