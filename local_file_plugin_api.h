#ifndef _SPOTIFY_LOCAL_FILE_PLUGIN_API__H
#define _SPOTIFY_LOCAL_FILE_PLUGIN_API__H
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#define SP_LF_PLUGIN_API_VERSION 1

enum SpotifyLFSoundFormat {
	kSoundFormat8BitsPerSample = 8,			// 8 bit per sample Native Endian PCM
	kSoundFormat16BitsPerSample = 16,		// 16 bit per sample Native Endian PCM
	kSoundFormatIEEEFloat = 17,				// 32 bit per sample IEEE FLOAT PCM
};

enum SPFieldType {
	kSPFieldTypeInvalid,

	kSPFieldTypeTitle,
	kSPFieldTypeArtist,
	kSPFieldTypeAlbum,
	kSPFieldTypeAlbumArtist,
	kSPFieldTypeComment,
	kSPFieldTypeComposer,
	kSPFieldTypeContent,
	kSPFieldTypePublisher,
	kSPFieldTypeCopyright,
	kSPFieldTypeUrl,
	kSPFieldTypeEncoded,
	kSPFieldTypeTrack,
	kSPFieldTypeDisc,
	kSPFieldTypeYear,
	kSPFieldTypeOrigArtist,
};

struct SpotifyLFPluginDescription;

struct SpotifyLFPlaybackPlugin {
	void *context;
	struct SpotifyLFPluginDescription *desc;
	
	bool (*open)(struct SpotifyLFPlaybackPlugin*, const char *path, int song_index);
	void (*close)(struct SpotifyLFPlaybackPlugin*);
	
	bool (*decode)(struct SpotifyLFPlaybackPlugin*, char *dest, int *destlen, bool *final);
	bool (*seek)(struct SpotifyLFPlaybackPlugin*, unsigned sample);
	unsigned int (*getMinimumOutputBufferSize)(struct SpotifyLFPlaybackPlugin*);
	unsigned int (*getLengthInSamples)(struct SpotifyLFPlaybackPlugin*);
	
	void (*getAudioFormat)(struct SpotifyLFPlaybackPlugin*, int *samplerate, enum SpotifyLFSoundFormat *format, int *channels);
};

struct SpotifyLFParserPlugin {
	void *context;
	struct SpotifyLFPluginDescription *desc;
	
	bool (*open)(struct SpotifyLFParserPlugin*, const char *path, int song_index);
	void (*close)(struct SpotifyLFParserPlugin*);
	
	unsigned int (*songCount)(struct SpotifyLFParserPlugin*);
	bool (*isStereo)(struct SpotifyLFParserPlugin*);
	unsigned int (*sampleRate)(struct SpotifyLFParserPlugin*);
	unsigned int (*lengthInSamples)(struct SpotifyLFParserPlugin*);

	bool (*hasField)(struct SpotifyLFParserPlugin*, enum SPFieldType type);
	unsigned int (*fieldLength)(struct SpotifyLFParserPlugin*, enum SPFieldType type);
	void (*readField)(struct SpotifyLFParserPlugin*, enum SPFieldType type, char *dest, unsigned int *length);
	void (*writeField)(struct SpotifyLFParserPlugin*, enum SPFieldType type, const char *src, unsigned int write_length);
};


struct SpotifyLFPluginDescription {
	/// Always set this to SP_LF_PLUGIN_API_VERSION
	unsigned int api_version;
	
	char *plugin_name;
	unsigned int plugin_version;
	
	/// Space-separated, NULL-terminated list of extensions (excluding dot) this plugin adds support for
	char *file_extensions;

	void *plugin_context;	
	struct SpotifyLFPlaybackPlugin playback;
	struct SpotifyLFParserPlugin parser;
};

static inline void SPLocalFilePluginInitialize(
	struct SpotifyLFPluginDescription *plugin, 
	const char *name,
	unsigned int version
)
{ 
	memset(plugin, 0, sizeof(struct SpotifyLFPluginDescription));
	plugin->plugin_name = strdup(name);
	plugin->plugin_version = version;
	plugin->api_version = SP_LF_PLUGIN_API_VERSION;
	plugin->playback.desc = plugin;
	plugin->parser.desc = plugin;
}
	

// Implement and export this symbol from your dynamic library
//extern struct SpotifyLFPluginDescription *SpotifyLocalFilePlaybackPluginCreate();


#if __cplusplus
}
#endif
#endif