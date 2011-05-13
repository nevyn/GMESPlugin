#include "local_file_plugin_api.h"
#include "gme/gme.h"

struct GMEPlayerContext {
	unsigned trackno;
	track_info_t track;
	Music_Emu *emu;
	unsigned length_in_samples;
};

bool GMEPlayerOpen(struct SpotifyLFPlaybackPlugin*, const char *path, int song_index);
void GMEPlayerClose(struct SpotifyLFPlaybackPlugin*);

bool GMEPlayerDecode(struct SpotifyLFPlaybackPlugin*, char *dest, int *destlen, bool *final);
bool GMEPlayerSeek(struct SpotifyLFPlaybackPlugin*, unsigned sample);
unsigned int GMEPlayerGetMinimumOutputBufferSize(struct SpotifyLFPlaybackPlugin*);
unsigned int GMEPlayerGetLengthInSamples(struct SpotifyLFPlaybackPlugin*);

void GMEPlayerGetAudioFormat(struct SpotifyLFPlaybackPlugin*, int *samplerate, enum SpotifyLFSoundFormat *format, int *channels);

void GMEPlayerInitialize(struct SpotifyLFPlaybackPlugin *plugin);