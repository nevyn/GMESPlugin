#include "local_file_plugin_api.h"
#include "gme/gme.h"

struct GMEPlayerContext {
	unsigned trackno;
	track_info_t track;
	Music_Emu *emu;
	unsigned length_in_samples;
};

spbool_t GMEPlayerOpen(struct SpotifyLFPlaybackPlugin*, const char *path, int song_index);
void GMEPlayerClose(struct SpotifyLFPlaybackPlugin*);

spbool_t GMEPlayerDecode(struct SpotifyLFPlaybackPlugin*, char *dest, int *destlen, spbool_t *final);
spbool_t GMEPlayerSeek(struct SpotifyLFPlaybackPlugin*, unsigned sample);
unsigned int GMEPlayerGetMinimumOutputBufferSize(struct SpotifyLFPlaybackPlugin*);
unsigned int GMEPlayerGetLengthInSamples(struct SpotifyLFPlaybackPlugin*);

void GMEPlayerGetAudioFormat(struct SpotifyLFPlaybackPlugin*, int *samplerate, enum SpotifyLFSoundFormat *format, int *channels);

void GMEPlayerInitialize(struct SpotifyLFPlaybackPlugin *plugin);