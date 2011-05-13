#include "local_file_plugin_api.h"
#include <stdlib.h>
#include "gme_parser.h"
#include "gme_player.h"

extern struct SpotifyLFPluginDescription *SpotifyLocalFilePlaybackPluginCreate()
{
	struct SpotifyLFPluginDescription *desc = malloc(sizeof(struct SpotifyLFPluginDescription));
	SPLocalFilePluginInitialize(desc, "Game Music Emu", 1);
	
	GMEParserInitialize(&desc->parser);
	GMEPlayerInitialize(&desc->playback);
	
	gme_type_t const *gme_types = gme_type_list();
	
	unsigned gme_count = 0; for(;gme_types[gme_count] != NULL; gme_count++) {}
	
	const char **extensions = malloc((gme_count+1)*sizeof(char*));
	for(int i = 0; gme_types[i] != NULL; i++)
		extensions[i] = gme_types[i]->extension_;
	extensions[gme_count] = NULL;
	
	desc->file_extensions = extensions;

	
	return desc;
}
