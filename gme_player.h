#include "local_file_plugin_api.h"
#include "gme/gme.h"

struct GMEPlayerContext {
	unsigned trackno;
	track_info_t track;
	Music_Emu *emu;
	unsigned length_in_samples;
};

void *GMEPlayerOpen(struct SpotifyLFPluginDescription *plugin, const char *path, int song_index);
void GMEPlayerClose(struct SpotifyLFPluginDescription *plugin, void *ctx);

spbool GMEPlayerDecode(
	struct SpotifyLFPluginDescription *plugin, void *ctx,
	spbyte *dest,
	size_t *destlen,
	spbool *final
);
spbool GMEPlayerSeek(struct SpotifyLFPluginDescription *plugin, void *ctx, unsigned sample);
size_t GMEPlayerGetMinimumOutputBufferSize(struct SpotifyLFPluginDescription *plugin, void *ctx);
unsigned int GMEPlayerGetLengthInSamples(struct SpotifyLFPluginDescription *plugin, void *ctx);

void GMEPlayerGetAudioFormat(
	struct SpotifyLFPluginDescription *plugin, void *ctx,
	unsigned int *samplerate,
	enum SpotifyLFSoundFormat *format,
	enum SPChannelFormat *channelFormat
);

void GMEPlayerInitialize(struct SpotifyLFPlaybackPlugin *plugin);