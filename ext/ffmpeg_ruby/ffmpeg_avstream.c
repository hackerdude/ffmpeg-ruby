#include "ruby.h"
#include <libavformat/avformat.h>
#include "ffmpeg_avstream.h"

void Init_ffmpeg_ruby_avstream(VALUE module)
{
		cFFMpegAVStream = rb_define_class_under(module, "AVStream", rb_cObject);
		rb_define_method(cFFMpegAVStream, "canonical_codec_name", AVStream_canonical_codec_name, 0);
		rb_define_method(cFFMpegAVStream, "codec", AVStream_codec, 0);
}

void AVStream_free(void *v) {}
void AVStream_mark(void *v) {}

VALUE AVStream_canonical_codec_name(VALUE self)
{
	AVStream *ptr;
	AVCodec *c;

	Data_Get_Struct(self, AVStream, ptr);
	//char buf[256];
	c =avcodec_canonical_name(ptr->codec);
	return c;
}

VALUE AVStream_codec(VALUE self)
{
	AVStream *pStream;
	AVCodec *c;
	Data_Get_Struct(self, AVStream, pStream);
	c = avcodec_find_decoder(pStream->codec->codec_id);
	if (!c) {
		return Qnil;
	}
	VALUE rCodec  = Data_Wrap_Struct(cFFMpegAVCodec, AVCodec_mark, AVCodec_free, c);
	rb_obj_call_init(rCodec,0,0);
	return rCodec;
}
