#include "local_file_plugin_api.h"
#include "gme/gme.h"

struct GMEParserContext {
	int trackno;
	int track_count;
	track_info_t gmetrack;
	char *filename;
};

void *GMEParserOpen(struct SpotifyLFPluginDescription *plugin, const char *path, int song_index);
void GMEParserClose(struct SpotifyLFPluginDescription *plugin, void *ctx);

unsigned int GMEParserSongCount(struct SpotifyLFPluginDescription *plugin, void *ctx);
enum SPChannelFormat GMEParserGetChannelFormat(struct SpotifyLFPluginDescription *plugin, void *ctx);
unsigned int GMEParserSampleRate(struct SpotifyLFPluginDescription *plugin, void *ctx);
unsigned int GMEParserLengthInSamples(struct SpotifyLFPluginDescription *plugin, void *ctx);

spbool GMEParserHasField(struct SpotifyLFPluginDescription *plugin, void *ctx, enum SPFieldType frame);
spbool GMEParserReadField(struct SpotifyLFPluginDescription *plugin, void *ctx, enum SPFieldType frame, char *dest, size_t *length);

void GMEParserInitialize(struct SpotifyLFParserPlugin *plugin);

unsigned lengthInSamplesOfTrack(const track_info_t *track);