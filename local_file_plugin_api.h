#ifndef _SPOTIFY_LOCAL_FILE_PLUGIN_API__H
#define _SPOTIFY_LOCAL_FILE_PLUGIN_API__H
#ifdef __cplusplus
extern "C" {
#endif



/**
	Spotify Local File Plugin API
	=============================
	This file describes the API Spotify exposes for adding support for parsing
	and playing music file formats in Spotify.

	Building
	--------
	1. Implement this API, and export the symbol SpotifyLocalFilePlaybackPluginCreate
	   (see bottom of document).
	2. Create a folder called [your plugin name].splugin
	3. Compile your source into a dynamic library for your platform.
	4. Put the compiled library into
	   [name].splugin/Contents/[MacOS|Win32|{some platform}]/{name}
	   (append .dll if the platform is Windows)
	5. Repeat 3 and 4 for each platform you wish to support.

	(You can also just rename your dynamic library to 
	 [your plugin name].splugin if you only wish to support a single platform)
	 
	Installing
	----------
	MacOS: Install the .splugin into
		   ~/Library/Application Support/Spotify/Plug-Ins/LocalFile/
		   (or any other domain)
	Windows: [TBD]
	Linux: [TBD]
*/

#include <string.h>

typedef unsigned char spbool;
enum { spfalse, sptrue };
typedef unsigned char spbyte;

#define SP_LF_PLUGIN_API_VERSION 4

/// Spotify supports the following output sound formats.
enum SpotifyLFSoundFormat {
	kSoundFormat8BitsPerSample = 8,		// 8 bit per sample Native Endian PCM
	kSoundFormat16BitsPerSample = 16,	// 16 bit per sample Native Endian PCM
	kSoundFormatIEEEFloat = 17,			// 32 bit per sample IEEE FLOAT PCM
};

/// The different types of metadata that a parser can be asked about.
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
	// Future versions may ask for more metadata, be prepared
	// to fail gracefully in such a case.
};

struct SpotifyLFPluginDescription;

/// Used to extract metadata from a file in the plugin's supported formats.
struct SpotifyLFParserPlugin {

	/// Create a metadata parsing context for the given file and song.
	/// This context will be forwarded to all metadata methods below.
	/// @returns your internal state on success, NULL on failure.
	void *(*create)(struct SpotifyLFPluginDescription*, const char *path, int song_index);
	
	/// Close the file and destroy the context previously created with `create`.
	void (*destroy)(struct SpotifyLFPluginDescription*, void *context);
	
	
	/// How many songs does this single file contain? Commonly only one,
	/// but game music files (SID etc) sometimes contain several.
	unsigned int (*getSongCount)(struct SpotifyLFPluginDescription*, void *context);
	
	
	/// Is this file in stereo?
	spbool (*isStereo)(struct SpotifyLFPluginDescription*, void *context);
	
	/// What does the metadata say that this file is in?
	unsigned int (*getSampleRate)(struct SpotifyLFPluginDescription*, void *context);
	
	/// How long does the metadata claim that this file is?
	unsigned int (*getLengthInSamples)(struct SpotifyLFPluginDescription*, void *context);
	
	
	// Field extraction methods. `Type` is the 'column' of data to fetch for this row.
	
	/// Does this song in this file have this column of data?
	spbool (*hasField)(struct SpotifyLFPluginDescription*, void *context, enum SPFieldType type);
	
	/// Read a column of data from this song in this file as a string.
	/// `readField` will be called first with a NULL `dest` to determine the
	/// length of the string.
	/// @returns whether the operation was successful.
	spbool (*readField)(
		struct SpotifyLFPluginDescription*, void *context,
		enum SPFieldType type,
		char *dest,
		size_t *length
	);
	
	/// OPTIONAL: Write a column of data to this song in this file as a string.
	/// You may leave `writeField` as NULL if you do not support editing metadata.
	/// @returns whether the operation was successful.
	spbool (*writeField)(
		struct SpotifyLFPluginDescription*, void *context,
		enum SPFieldType type,
		const char *src,
		size_t write_length);
};

/// Used to decode a file in the plugin's supported formats into a buffer of samples.
struct SpotifyLFPlaybackPlugin {

	/// Create a playback context for the given file and song.
	/// This context will be forwarded to all playback methods below.
	/// @returns your internal state on success, NULL on failure.
	void *(*create)(struct SpotifyLFPluginDescription*, const char *path, int song_index);
	
	/// Close the file, cease decoding and destroy the context previously created with `create`.
	void (*destroy)(struct SpotifyLFPluginDescription*, void *context);
	
	
	/// Decode at most `*destlen` bytes of audio data into the buffer `dest`.
	/// @param dest The sample buffer to deposit your samples into.
	///             If your data is stereo, left and right samples should be interlaced.
	/// @param destlen Set `*destlen`to the number of bytes you have decoded when finished
	/// @param final Set `*final = sptrue` when you have decoded all bytes in the song
	/// @returns whether decoding was successful
	spbool (*decode)(
		struct SpotifyLFPluginDescription*, void *context,
		spbyte *dest,
		size_t *destlen,
		spbool *final
	);
	
	/// Seek to a specific sample offset into the song, and continue decoding
	/// from there instead.
	/// @returns whether seeking was successful	
	spbool (*seek)(struct SpotifyLFPluginDescription*, void *context, unsigned sample);
	
	
	/// How many bytes must the output buffer sent to `decode` be
	/// to be able to decode a frame of samples?
	size_t (*getMinimumOutputBufferSize)(struct SpotifyLFPluginDescription*, void *context);
	
	/// Given that you now have a decoder for this song, how long can you
	/// determine that the song *actually* is? (can be different from what
	/// the parser thought it would be)
	unsigned int (*getLengthInSamples)(struct SpotifyLFPluginDescription*, void *context);
	
	/// Which audio format will you output in the `decode` method?
	void (*getAudioFormat)(
		struct SpotifyLFPluginDescription*, void *context,
		unsigned int *samplerate,
		enum SpotifyLFSoundFormat *format,
		unsigned int *channels
	);
};



/// This struct describes your plugin as a whole; both its parser,
/// player and metadata about the plugin.
struct SpotifyLFPluginDescription {
	/// The API version that your plugin was built for.
	/// Always set this to SP_LF_PLUGIN_API_VERSION.
	unsigned int api_version;
	
	/// The user-visible name of your plugin.
	char *plugin_name;
	unsigned int plugin_version;
	
	/// NULL-terminated list of strings of file extensions (excluding
	/// dot) this plugin adds support for.
	const char **file_extensions;
	
	/// Any context you wish to save that is persistent with this plugin.
	void *plugin_context;	
	
	struct SpotifyLFPlaybackPlugin playback;
	struct SpotifyLFParserPlugin parser;
};


/// Convenience method to null and initialize the basics of your plugin.
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
}
	

/// Implement and export this symbol from your dynamic library. Malloc
/// (or otherwise allocate) a plugin description, initialize it and return it.
/// @returns NULL to indicate failure.
//extern struct SpotifyLFPluginDescription *SpotifyLocalFilePlaybackPluginCreate();


#if __cplusplus
}
#endif
#endif