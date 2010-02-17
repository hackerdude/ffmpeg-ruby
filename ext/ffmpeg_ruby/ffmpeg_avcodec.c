#include "ruby.h"
#include <libavcodec/avcodec.h>

VALUE cFFMpegAVCodec;
VALUE cFFMpegAVCodecContext;

void AVCodec_mark(void *v) {}
void AVCodec_free(void *v) {}

void AVCodecContext_mark(AVCodecContext v) {}
void AVCodecContext_free(AVCodecContext v) {
}

VALUE AVCodecContext_codec_type(VALUE self)
{
  AVCodecContext *ptr;
  Data_Get_Struct(self, AVCodecContext, ptr);
	VALUE result = INT2NUM(ptr->codec_type);
	return result;
}

VALUE codec_type_map()
{
	VALUE result = rb_hash_new();
	rb_hash_aset(result, INT2NUM(CODEC_TYPE_VIDEO), rb_str_new2("Video"));
	rb_hash_aset(result, INT2NUM(CODEC_TYPE_AUDIO), rb_str_new2("Audio"));
	rb_hash_aset(result, INT2NUM(CODEC_TYPE_DATA), rb_str_new2("Data"));
	rb_hash_aset(result, INT2NUM(CODEC_TYPE_SUBTITLE), rb_str_new2("Subtitle"));
	rb_hash_aset(result, INT2NUM(CODEC_TYPE_ATTACHMENT), rb_str_new2("Attachment"));
	return result;
}


VALUE AVCodecContext_codec_id(VALUE self)
{
  AVCodecContext *ptr;
  Data_Get_Struct(self, AVCodecContext, ptr);
	VALUE result = INT2NUM(ptr->codec_id);
	return result;
}

VALUE AVCodecContext_fourcc_tag(VALUE self)
{
  AVCodecContext *enc;
	char buf1[128];

  Data_Get_Struct(self, AVCodecContext, enc);
	// TODO
	if( isprint(enc->codec_tag&0xFF) && isprint((enc->codec_tag>>8)&0xFF)
					&& isprint((enc->codec_tag>>16)&0xFF) && isprint((enc->codec_tag>>24)&0xFF)){
			snprintf(buf1, sizeof(buf1), "%c%c%c%c",
							enc->codec_tag & 0xff,
							(enc->codec_tag >> 8) & 0xff,
							(enc->codec_tag >> 16) & 0xff,
							(enc->codec_tag >> 24) & 0xff);
	} else {
			snprintf(buf1, sizeof(buf1), "0x%04x", enc->codec_tag);
	}
	VALUE result = rb_str_new2(buf1);
	return result;
}

VALUE AVCodecContext_width(VALUE self)
{
  AVCodecContext *ptr;
  Data_Get_Struct(self, AVCodecContext, ptr);
	VALUE result = INT2NUM(ptr->width);
	return result;
}

VALUE AVCodecContext_height(VALUE self)
{
  AVCodecContext *ptr;
  Data_Get_Struct(self, AVCodecContext, ptr);
	VALUE result = INT2NUM(ptr->height);
	return result;
}

VALUE AVCodecContext_channels(VALUE self)
{
  AVCodecContext *ptr;
  Data_Get_Struct(self, AVCodecContext, ptr);
	VALUE result = INT2NUM(ptr->channels);
	return result;
}

VALUE AVCodecContext_sample_rate(VALUE self)
{
  AVCodecContext *ptr;
  Data_Get_Struct(self, AVCodecContext, ptr);
	VALUE result = INT2NUM(ptr->sample_rate);
	return result;
}

VALUE AVCodec_long_name(VALUE self)
{
	char* tm;
  AVCodec *ptr;
  Data_Get_Struct(self, AVCodec, ptr);
	tm = ptr->long_name;
	return rb_str_new2(tm);
}

VALUE AVCodec_name(VALUE self)
{
  AVCodec *ptr;
  Data_Get_Struct(self, AVCodec, ptr);
	return rb_str_new2(ptr->name);
}

VALUE AVCodec_codec_type(VALUE self)
{
	char* tm;
  AVCodec *ptr;
  Data_Get_Struct(self, AVCodec, ptr);
	return INT2NUM(ptr->type);
}

VALUE AVCodecContext_codec_long_name(VALUE self)
{
	char* tm;
  AVCodecContext *ptr;
  Data_Get_Struct(self, AVCodecContext, ptr);
	if (ptr->codec == NULL)
			return Qnil;
	tm = ptr->codec->long_name;
	return rb_str_new2(tm);
}

VALUE AVCodecContext_codec_name(VALUE self)
{
	char* tm;
  AVCodecContext *ptr;
  Data_Get_Struct(self, AVCodecContext, ptr);
	if (ptr->codec == NULL)
			return Qnil;
	tm = ptr->codec->name;
	return rb_str_new2(tm);
}

VALUE supported_avcodecs()
{
	VALUE result = rb_hash_new();
	AVCodec *pNextCodec = NULL;
	pNextCodec = av_codec_next(pNextCodec);
	while (pNextCodec != NULL) {
		VALUE sNextCodecName     = rb_str_new2(pNextCodec->name);
		//VALUE sNextCodecLongName = rb_str_new2(pNextCodec->long_name);
		VALUE codec = Data_Wrap_Struct(cFFMpegAVCodec, AVCodecContext_mark, AVCodecContext_free, pNextCodec);
		rb_hash_aset(result, sNextCodecName, codec);
		pNextCodec = av_codec_next(pNextCodec);
	}
	return result;
}

VALUE supported_video_codecs()
{
	VALUE result = rb_hash_new();
	AVCodec *pNextCodec = NULL;
	pNextCodec = av_codec_next(pNextCodec);
	while (pNextCodec != NULL) {
		if (pNextCodec->type==CODEC_TYPE_VIDEO) {
			VALUE sNextCodecName     = rb_str_new2(pNextCodec->name);
			//VALUE sNextCodecLongName = rb_str_new2(pNextCodec->long_name);
			VALUE codec = Data_Wrap_Struct(cFFMpegAVCodec, AVCodecContext_mark, AVCodecContext_free, pNextCodec);
			rb_hash_aset(result, sNextCodecName, codec);
		}
		pNextCodec = av_codec_next(pNextCodec);
	}
	return result;
}

VALUE supported_audio_codecs()
{
	VALUE result = rb_hash_new();
	AVCodec *pNextCodec = NULL;
	pNextCodec = av_codec_next(pNextCodec);
	while (pNextCodec != NULL) {
		if (pNextCodec->type==CODEC_TYPE_AUDIO) {
			VALUE sNextCodecName     = rb_str_new2(pNextCodec->name);
			//VALUE sNextCodecLongName = rb_str_new2(pNextCodec->long_name);
			VALUE codec = Data_Wrap_Struct(cFFMpegAVCodec, AVCodecContext_mark, AVCodecContext_free, pNextCodec);
			rb_hash_aset(result, sNextCodecName, codec);
		}
		pNextCodec = av_codec_next(pNextCodec);
	}
	return result;
}
/*
 * Similar to avcodec_string on ffmpeg, but only 
 * gets the name using thse same logic. Uses the AVI tags
 */
VALUE avcodec_canonical_name(AVCodecContext *enc)
{
    const char *codec_name;
    AVCodec *p;
    char buf1[128];

		p = avcodec_find_decoder(enc->codec_id);
    if (p) {
        codec_name = p->name;
    } else if (enc->codec_id == CODEC_ID_MPEG2TS) {
        /* fake mpeg2 transport stream codec (currently not
 *            registered) */
        codec_name = "mpeg2ts";
    // } else if (enc->codec_name[0] != '\0') {
        // codec_name = enc->codec_name;
    } else {
        /* output avi tags */
        if(   isprint(enc->codec_tag&0xFF) && isprint((enc->codec_tag>>8)&0xFF)
           && isprint((enc->codec_tag>>16)&0xFF) && isprint((enc->codec_tag>>24)&0xFF)){
            snprintf(buf1, sizeof(buf1), "%c%c%c%c",
                     enc->codec_tag & 0xff,
                     (enc->codec_tag >> 8) & 0xff,
                     (enc->codec_tag >> 16) & 0xff,
                     (enc->codec_tag >> 24) & 0xff);
        } else {
            snprintf(buf1, sizeof(buf1), "0x%04x", enc->codec_tag);
        }
        codec_name = buf1;
    }

		return rb_str_new2(codec_name); // TODO on the ruby side, check enc->mb_decision to add the (hq) if we want to.
}


void Init_ffmpeg_ruby_avcodec(VALUE module)
{

		cFFMpegAVCodec = rb_define_class_under(module, "AVCodec", rb_cObject);
		rb_define_method(cFFMpegAVCodec, "name", AVCodec_name, 0);
		rb_define_method(cFFMpegAVCodec, "long_name", AVCodec_long_name, 0);
		rb_define_method(cFFMpegAVCodec, "codec_type", AVCodec_codec_type, 0);
		rb_define_singleton_method(cFFMpegAVCodec, "supported_avcodecs", supported_avcodecs, 0);
		rb_define_singleton_method(cFFMpegAVCodec, "supported_video_codecs", supported_video_codecs, 0);
		rb_define_singleton_method(cFFMpegAVCodec, "supported_audio_codecs", supported_audio_codecs, 0);
		cFFMpegAVCodecContext = rb_define_class_under(module, "AVCodecContext", rb_cObject);
		rb_define_method(cFFMpegAVCodecContext, "long_name", AVCodecContext_codec_long_name, 0);
		rb_define_method(cFFMpegAVCodecContext, "name", AVCodecContext_codec_name, 0);
		rb_define_method(cFFMpegAVCodecContext, "codec_type", AVCodecContext_codec_type, 0);
		rb_define_method(cFFMpegAVCodecContext, "fourcc_tag", AVCodecContext_fourcc_tag, 0);
		rb_define_method(cFFMpegAVCodecContext, "codec_id", AVCodecContext_codec_id, 0);
		rb_define_method(cFFMpegAVCodecContext, "width", AVCodecContext_width, 0);
		rb_define_method(cFFMpegAVCodecContext, "height", AVCodecContext_height, 0);
		rb_define_method(cFFMpegAVCodecContext, "channels", AVCodecContext_channels, 0);
		rb_define_method(cFFMpegAVCodecContext, "sample_rate", AVCodecContext_sample_rate, 0);

		// Codec Types, for AVCodecContext
		rb_define_const(module, "CODEC_TYPE_VIDEO", INT2NUM(CODEC_TYPE_VIDEO));
		rb_define_const(module, "CODEC_TYPE_AUDIO", INT2NUM(CODEC_TYPE_AUDIO));
		rb_define_const(module, "CODEC_TYPE_DATA", INT2NUM(CODEC_TYPE_DATA));
		rb_define_const(module, "CODEC_TYPE_SUBTITLE", INT2NUM(CODEC_TYPE_SUBTITLE));
		rb_define_const(module, "CODEC_TYPE_ATTACHMENT", INT2NUM(CODEC_TYPE_ATTACHMENT));
		rb_define_const(module, "MAP_CODEC_TYPES", codec_type_map());

}
