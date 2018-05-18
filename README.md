# LZW
LZW codec implementation for a class.

## Building
```sh
make
```

## Testing
The makefile includes a recipe to encode a file, decode its encoding and confront their hashes (requires `shasum` in `$PATH`).

```sh
# Use default test file (f.txt)
make test

# Use custom test file
make test TESTFILE=<customfile.txt>
```

A more verbosy output can be provided while compiling with the DEBUG variable.

```sh
make clean
make DEBUG=1
```

## Usage

```
	Compress/decompress file
		./lzw COMMAND FILE
	Compress/decompress file, specifying name in output
		./lzw COMMAND FILE (-o|--output) FILEOUT
	Compress/decompress file, sending the processed data to stdout
		./lzw COMMAND FILE --to-stdout
```
