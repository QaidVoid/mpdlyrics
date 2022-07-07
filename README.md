## Display synchronized Lyrics for current MPD song
It fetches the lyrics from LRC file if exists, then prints the lyrics on standard output

Current implementation uses hard-coded Music directory at `~/Music`

The lyrics file is expected to be in the same directory as the music file inside `~/Music`

### Dependencies
```sh
libmpdclient
```

### Building
Clone the repo
```sh
git clone https://github.com/QaidVoid/mpdlyrics
```
```sh
cd mpdlyrics
```
Build
```sh
make
```
Run
```sh
./mpdlyrics
```

### Future Improvements
- [ ] Configuration options
- [ ] Embedded lyrics support
