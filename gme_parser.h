#include "local_file_plugin_api.h"
#include "gme/gme.h"

struct GMEParserContext {
	int trackno;
	int track_count;
	track_info_t gmetrack;
	char *filename;
};

bool GMEParserOpen(struct SpotifyLFParserPlugin *plugin, const char *path, int song_index);
void GMEParserClose(struct SpotifyLFParserPlugin *plugin);

unsigned int GMEParserSongCount(struct SpotifyLFParserPlugin *plugin);
bool GMEParserIsStereo(struct SpotifyLFParserPlugin *plugin);
unsigned int GMEParserSampleRate(struct SpotifyLFParserPlugin *plugin);
unsigned int GMEParserLengthInSamples(struct SpotifyLFParserPlugin *plugin);

bool GMEParserHasField(struct SpotifyLFParserPlugin *plugin, enum SPFieldType frame);
unsigned int GMEParserFieldLength(struct SpotifyLFParserPlugin *plugin, enum SPFieldType frame);
void GMEParserReadField(struct SpotifyLFParserPlugin *plugin, enum SPFieldType frame, char *dest, unsigned int *length);
void GMEParserWriteField(struct SpotifyLFParserPlugin *plugin, enum SPFieldType frame, const char *src, unsigned int write_length);

void GMEParserInitialize(struct SpotifyLFParserPlugin *plugin);

unsigned lengthInSamplesOfTrack(const track_info_t *track);