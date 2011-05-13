#include "gme_player.h"
#include "gme_parser.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define self ((struct GMEPlayerContext*)ctx)
static unsigned samplerate = 44100;

static unsigned samples_to_ms(unsigned sample_count) {
	return sample_count/(samplerate/1000);
}

void *GMEPlayerOpen(struct SpotifyLFPluginDescription *plugin, const char *path, int song_index)
{
	struct GMEPlayerContext *ctx = calloc(sizeof(struct GMEPlayerContext), 1);

	self->trackno = song_index;
	
	gme_err_t err;
	err = gme_open_file(path, &self->emu, samplerate);
	if (err) {
		fprintf(stderr, "GME couldn't open file %s: %s\n", path, err);
		free(ctx);
		return spfalse;
	}
	
	err = gme_track_info(self->emu, &self->track, self->trackno);
	if (err) {
		fprintf(stderr, "GME couldn't get track info of track %d in %s: %s\n", song_index, path, err);
		free(ctx);
		return spfalse;
	}
	
	self->length_in_samples = lengthInSamplesOfTrack(&self->track);
	
	err = gme_start_track(self->emu, self->trackno);
	if (err) {
		fprintf(stderr, "GME couldn't play track %d in %s: %s\n", song_index, path, err);
		free(ctx);
		return spfalse;
	}
	
	// Fade out during the last two seconds of playback
	gme_set_fade(self->emu, samples_to_ms(self->length_in_samples)-2000);
	
	return ctx;
}
void GMEPlayerClose(struct SpotifyLFPluginDescription *plugin, void *ctx)
{
	gme_delete(self->emu);
	free(ctx);
}

spbool GMEPlayerDecode(struct SpotifyLFPluginDescription *plugin, void *ctx, spbyte *dest, size_t *destlen, spbool *final)
{
	gme_err_t err;
	err = gme_play(self->emu, *destlen/2, (short*)dest);
	if (err) {
		fprintf(stderr, "GME error while decoding: %s\n", err);
		*destlen = 0;
		return spfalse;
	}
	*final = gme_track_ended(self->emu);

	return sptrue;
}
spbool GMEPlayerSeek(struct SpotifyLFPluginDescription *plugin, void *ctx, unsigned sample)
{
	gme_err_t err;
	err = gme_seek(self->emu, samples_to_ms(sample));
	if (err) {
		fprintf(stderr, "GME couldn't seek %s\n", err);
		return spfalse;
	}
	return sptrue;
}
size_t GMEPlayerGetMinimumOutputBufferSize(struct SpotifyLFPluginDescription *plugin, void *ctx)
{
	return 1024*16;
}
unsigned int GMEPlayerGetLengthInSamples(struct SpotifyLFPluginDescription *plugin, void *ctx)
{
	return self->length_in_samples;
}

void GMEPlayerGetAudioFormat(struct SpotifyLFPluginDescription *plugin, void *ctx, unsigned int *samplerate_, enum SpotifyLFSoundFormat *format, unsigned int *channels)
{
	*samplerate_ = samplerate;
	*format = kSoundFormat16BitsPerSample;
	*channels = 2;
}


void GMEPlayerInitialize(struct SpotifyLFPlaybackPlugin *plugin)
{
	plugin->create = GMEPlayerOpen;
	plugin->destroy = GMEPlayerClose;
	plugin->decode = GMEPlayerDecode;
	plugin->seek = GMEPlayerSeek;
	plugin->getMinimumOutputBufferSize = GMEPlayerGetMinimumOutputBufferSize;
	plugin->getLengthInSamples = GMEPlayerGetLengthInSamples;
	plugin->getAudioFormat = GMEPlayerGetAudioFormat;
}