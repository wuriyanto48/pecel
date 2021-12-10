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

### Connect to `pecel server` with `nc`

```shell
$ nc 127.0.0.1 6009
$ SET 1 wury
$ OK*
$ GET 1
$ wury
```

### TODO
- add auth feature
- more validation and error handling