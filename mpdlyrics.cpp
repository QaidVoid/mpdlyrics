#include <iostream>
#include <fstream>
#include <vector>
#include <unistd.h>
#include <pwd.h>

#include <mpd/client.h>

struct Lyrics {
  int ms;
  std::string lyrics;
};

/*
 * Read lyrics from LRC file and store in a `vector`
 * The `vector` contains the `Lyrics` struct which stores each line of lyrics
 * including the time in ms
 */
std::vector<Lyrics> fetch_lyrics(struct mpd_song *song) {
  // TODO: Configuration file support
  struct passwd *pw = getpwuid(getuid());
  std::string home_dir = pw->pw_dir;
  std::string music_dir = home_dir +  "/Music/";

  std::vector<Lyrics> lyrics;
  std::string line;
  std::string user = getlogin();
  std::string uri = mpd_song_get_uri(song);

  // remove extension from uri
  int lastIdx = uri.find_last_of(".");
  std::string fullpath = music_dir + uri.substr(0, lastIdx) + ".lrc";

  std::ifstream is(fullpath);

  if (!is) {
    lyrics.push_back(Lyrics {
      .ms = 0,
      .lyrics = " "
    });
    return lyrics;
  }

  while (std::getline(is, line)) {
    if (line.empty()) continue;

    try {
      int min = std::stoi(line.substr(1, 2));
      int sec = std::stoi(line.substr(4, 2));
      int ms = std::stoi(line.substr(7, 2)) * 10;

      int min_ms = min * 60000;
      int sec_ms = sec * 1000;
      int final_ms = min_ms + sec_ms + ms;

      lyrics.push_back(Lyrics {
        .ms =  final_ms,
        .lyrics =  line.substr(10, line.length() - 10)
      });
    } catch(...) {
      continue;
    }
  }
  return lyrics;
}

/*
 * Print current lyrics on standard output
 */
void current_lyrics(struct mpd_connection *conn) {
  enum mpd_state state;
  struct mpd_status *status;
  struct mpd_song *song;
  std::string uri, prev_uri;
  unsigned next_ms = 0, next_idx = 0, time = 0;
  std::vector<Lyrics> lyrics;

  while (1) {
		mpd_command_list_begin(conn, true);
		mpd_send_status(conn);
		mpd_send_current_song(conn);
		mpd_command_list_end(conn);

    status = mpd_recv_status(conn);

    if (!status) {
      // Try reconnecting ...
      conn = mpd_connection_new(NULL, 0, 0);
      next_idx = 0;
      next_ms = 0;
      usleep(500000);
      continue;
    };
    time = mpd_status_get_elapsed_ms(status);
    state = mpd_status_get_state(status);
    mpd_status_free(status);

    if (mpd_connection_get_error(conn) != MPD_ERROR_SUCCESS) {
      std::cout << "ERROR" << std::endl;
    }

    mpd_response_next(conn);

    if (state > 1) {
      while ((song = mpd_recv_song(conn)) != NULL) {
        uri = mpd_song_get_uri(song);

        if (prev_uri != uri) {
          lyrics = fetch_lyrics(song);
          next_ms = 0;
          next_idx = 0;
        }

        if (next_idx != lyrics.size()) {
          if (time >= next_ms) {
            std::cout << lyrics[next_idx].lyrics << std::endl;

            if (lyrics.size() > next_idx + 1) {
              next_ms = lyrics[next_idx + 1].ms;
              next_idx++;
            } else {
              next_ms = mpd_song_get_duration_ms(song);
            }
          }
        }
        prev_uri = uri;
        mpd_song_free(song);
      }

      if (mpd_connection_get_error(conn) != MPD_ERROR_SUCCESS || !mpd_response_finish(conn)) {
        std::cout << "ERROR" << std::endl;
      }
    }
    usleep(10000);
  }
}

int main() {
  struct mpd_connection *conn;
  conn = mpd_connection_new(NULL, 0, 0);

  if (mpd_connection_get_error(conn)) {
    mpd_connection_free(conn);
  }

  current_lyrics(conn);

  mpd_connection_free(conn);
}
