#include "gme_parser.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define MIN(a, b) ( (a) < (b) ? (a) : (b) )

static const unsigned samplerate = 44100;
#define self ((struct GMEParserContext*)ctx)

unsigned lengthInSamplesOfTrack(const track_info_t *track)
{
	unsigned m = samplerate/1000; // convert millisecs to sample count
	if (track->length && track->length != -1) {
		return track->length*m;
	} else if (track->loop_length && track->loop_length != -1) {
		// Loop thrice
		return (track->intro_length + track->loop_length*3)*m;
	} else {
		// Play for two minutes
		return 2*60*1000*m;
	}
}


void *GMEParserOpen(struct SpotifyLFPluginDescription *plugin, const char *path, int song_index)
{
	struct GMEParserContext *ctx = calloc(sizeof(struct GMEParserContext), 1);
	

	
	Music_Emu *emu;
	gme_err_t err;
	err = gme_open_file(path, &emu, gme_info_only);
	if (err) {
		fprintf(stderr, "GMEParserOpen(%s): %s\n", path, err);
		return spfalse;
	}
	
	self->track_count = gme_track_count(emu);

	self->trackno = song_index;
	err = gme_track_info(emu, &self->gmetrack, self->trackno);
	if (err) {
		fprintf(stderr, "GMEParserOpen(%s): %s\n", path, err);
		return spfalse;
	}
	
	self->filename = strdup(path);
	
	return ctx;
}
void GMEParserClose(struct SpotifyLFPluginDescription *plugin, void *ctx)
{
	free(self->filename);
	free(ctx);
}

unsigned int GMEParserSongCount(struct SpotifyLFPluginDescription *plugin, void *ctx)
{
	return self->track_count;
}
enum SPChannelFormat GMEParserGetChannelFormat(struct SpotifyLFPluginDescription *plugin, void *ctx)
{
	return kSPStereo;
}
unsigned int GMEParserSampleRate(struct SpotifyLFPluginDescription *plugin, void *ctx)
{
	return samplerate;
}
unsigned int GMEParserLengthInSamples(struct SpotifyLFPluginDescription *plugin, void *ctx)
{
	return lengthInSamplesOfTrack(&self->gmetrack);
}

spbool GMEParserHasField(struct SpotifyLFPluginDescription *plugin, void *ctx, enum SPFieldType frame)
{
	size_t length;
	spbool status = GMEParserReadField(plugin, ctx, frame, NULL, &length);
	return status && length > 0;
}
spbool GMEParserReadField(struct SpotifyLFPluginDescription *plugin, void *ctx, enum SPFieldType type, char *dest, size_t *length)
{
	#define copy_setlen(attr) { if(dest) strncpy(dest, self->gmetrack.attr, *length); *length = MIN(strlen(self->gmetrack.attr), *length); return sptrue; }
	switch (type) {
		case kSPFieldTypeTitle:
			if(strlen(self->gmetrack.song) > 0)
				copy_setlen(song)
			else {
				if(self->track_count==1) {
					// If single track in file, use file name as song
					const char *filename = strrchr(self->filename, '/')+1;
					if(dest) {
						strncpy(dest, filename, *length);
						strrchr(dest, '.')[0] = '\0';
						*length = strlen(dest);
					} else
						*length = strlen(filename);
				} else {
					if(dest)
						*length = snprintf(dest, *length, "Track %02d", self->trackno+1);
					else
						*length = 20;
				}
				
			}
			return sptrue;
		case kSPFieldTypeArtist: copy_setlen(author);
		case kSPFieldTypeAlbum:
		case kSPFieldTypeAlbumArtist:;
			if(strlen(self->gmetrack.game) > 0)
				copy_setlen(game)
			else {
				if(self->track_count==1) {
					// If single track in file, use folder name as game
					if(dest) {
						strncpy(dest, self->filename, *length);
						strrchr(dest, '/')[0] = '\0';
						memmove(dest, strrchr(dest, '/')+1, strrchr(dest, '/') - dest-1);
					} else
						*length = strlen(self->filename);
				} else
					*length = 0;
			}
			return sptrue;
		case kSPFieldTypeComment: copy_setlen(comment);
		case kSPFieldTypeComposer: copy_setlen(dumper);
		case kSPFieldTypeCopyright: copy_setlen(copyright);
		case kSPFieldTypePublisher: copy_setlen(system);
		case kSPFieldTypeTrack:
			if(dest)
				*length = snprintf(dest, *length, "%d", self->trackno+1);
			else
				*length = 20;
			return sptrue;
	}
	return spfalse;
}



void GMEParserInitialize(struct SpotifyLFParserPlugin *plugin)
{
	plugin->create = GMEParserOpen;
	plugin->destroy = GMEParserClose;
	plugin->getSongCount = GMEParserSongCount;
	plugin->getChannelFormat = GMEParserGetChannelFormat;
	plugin->getSampleRate = GMEParserSampleRate;
	plugin->getLengthInSamples = GMEParserLengthInSamples;
	plugin->hasField = GMEParserHasField;
	plugin->readField = GMEParserReadField;
}