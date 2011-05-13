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
	
	char *extensions = NULL;
	gme_type_t const *gme_types = gme_type_list();
	size_t ext_offs = 0;
	for(int i = 0; gme_types[i] != NULL; i++) {
		const char *ext = gme_types[i]->extension_;
		extensions = realloc(extensions, ext_offs + strlen(ext) + 1);
		memcpy(extensions + ext_offs, ext, strlen(ext));
		ext_offs += strlen(ext);
		if(gme_types[i+1] != NULL)
			extensions[ext_offs] = ' ';
		else
			extensions[ext_offs] = '\0';
		ext_offs += 1;
	}
	desc->file_extensions = extensions;

	
	return desc;
}
