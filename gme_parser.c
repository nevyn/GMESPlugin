#include "gme_parser.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define MIN(a, b) ( (a) < (b) ? (a) : (b) )

static const unsigned samplerate = 44100;
#define self ((struct GMEParserContext*)plugin->context)

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


bool GMEParserOpen(struct SpotifyLFParserPlugin *plugin, const char *path, int song_index)
{
	plugin->context = calloc(sizeof(struct GMEParserContext), 1);
	

	
	Music_Emu *emu;
	gme_err_t err;
	err = gme_open_file(path, &emu, gme_info_only);
	if (err) {
		fprintf(stderr, "GMEParserOpen(%s): %s\n", path, err);
		return false;
	}
	
	self->track_count = gme_track_count(emu);

	self->trackno = song_index;
	err = gme_track_info(emu, &self->gmetrack, self->trackno);
	if (err) {
		fprintf(stderr, "GMEParserOpen(%s): %s\n", path, err);
		return false;
	}
	
	self->filename = strdup(path);
	
	return true;
}
void GMEParserClose(struct SpotifyLFParserPlugin *plugin)
{
	if(plugin->context) {
		free(self->filename);
		
		free(plugin->context);
		plugin->context = NULL;
	}
}

unsigned int GMEParserSongCount(struct SpotifyLFParserPlugin *plugin)
{
	return self->track_count;
}
bool GMEParserIsStereo(struct SpotifyLFParserPlugin *plugin)
{
	return true;
}
unsigned int GMEParserSampleRate(struct SpotifyLFParserPlugin *plugin)
{
	return samplerate;
}
unsigned int GMEParserLengthInSamples(struct SpotifyLFParserPlugin *plugin)
{
	return lengthInSamplesOfTrack(&self->gmetrack);
}

bool GMEParserHasField(struct SpotifyLFParserPlugin *plugin, enum SPFieldType frame)
{
	return GMEParserFieldLength(plugin, frame) != 0;
}
unsigned int GMEParserFieldLength(struct SpotifyLFParserPlugin *plugin, enum SPFieldType type)
{
	switch (type) {
		case kSPFieldTypeTitle:
			if(strlen(self->gmetrack.song) > 0) return strlen(self->gmetrack.song);
			return 255;
		case kSPFieldTypeArtist: return strlen(self->gmetrack.author);
		case kSPFieldTypeAlbum: 
		case kSPFieldTypeAlbumArtist: return strlen(self->gmetrack.game) ?:((self->track_count==1)?255:0);
		case kSPFieldTypeComment: return strlen(self->gmetrack.comment);
		case kSPFieldTypeComposer: return strlen(self->gmetrack.dumper);
		case kSPFieldTypeCopyright: return strlen(self->gmetrack.copyright);
		case kSPFieldTypePublisher: return strlen(self->gmetrack.system);
		case kSPFieldTypeTrack: return 16;
	}
	return 0;
}
void GMEParserReadField(struct SpotifyLFParserPlugin *plugin, enum SPFieldType type, char *dest, unsigned int *length)
{
	#define copy_setlen(attr) { strncpy(dest, self->gmetrack.attr, *length); *length = MIN(strlen(self->gmetrack.attr), *length); return; }
	switch (type) {
		case kSPFieldTypeTitle:
			if(strlen(self->gmetrack.song) > 0)
				copy_setlen(song)
			else {
				if(self->track_count==1) {
					// If single track in file, use file name as song
					const char *filename = strrchr(self->filename, '/')+1;
					strncpy(dest, filename, *length);
					strrchr(dest, '.')[0] = '\0';
				} else
					*length = snprintf(dest, *length, "Track %02d", self->trackno+1);
				
			}
			return;
		case kSPFieldTypeArtist: copy_setlen(author);
		case kSPFieldTypeAlbum:
		case kSPFieldTypeAlbumArtist:;
			if(strlen(self->gmetrack.game) > 0)
				copy_setlen(game)
			else {
				if(self->track_count==1) {
					// If single track in file, use folder name as game
					strncpy(dest, self->filename, *length);
					strrchr(dest, '/')[0] = '\0';
					memmove(dest, strrchr(dest, '/')+1, strrchr(dest, '/') - dest-1);
				} else
					*length = 0;
			}
			return;
		case kSPFieldTypeComment: copy_setlen(comment);
		case kSPFieldTypeComposer: copy_setlen(dumper);
		case kSPFieldTypeCopyright: copy_setlen(copyright);
		case kSPFieldTypePublisher: copy_setlen(system);
		case kSPFieldTypeTrack:
			*length = snprintf(dest, *length, "%d", self->trackno+1);
			return;
	}
}
void GMEParserWriteField(struct SpotifyLFParserPlugin *plugin, enum SPFieldType frame, const char *src, unsigned int write_length)
{
	// nop
}



void GMEParserInitialize(struct SpotifyLFParserPlugin *plugin)
{
	plugin->open = GMEParserOpen;
	plugin->close = GMEParserClose;
	plugin->songCount = GMEParserSongCount;
	plugin->isStereo = GMEParserIsStereo;
	plugin->sampleRate = GMEParserSampleRate;
	plugin->lengthInSamples = GMEParserLengthInSamples;
	plugin->hasField = GMEParserHasField;
	plugin->fieldLength = GMEParserFieldLength;
	plugin->readField = GMEParserReadField;
	plugin->writeField = GMEParserWriteField;
}