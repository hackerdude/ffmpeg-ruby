#include "ruby.h"

#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>

#include <stdio.h>

/** FFMpeg Module */
VALUE mFFMpeg;
/** AVFormatContext */
VALUE cFFMpegAVFormatContext;
/** FFMpeg::AVCodecContext */
VALUE cFFMpegAVCodecContext;

VALUE cFFMpegAVOutputFormat;
VALUE cFFMpegAVCodec;

static void AVCodec_mark(void *v) {}
static void AVCodec_free(void *v) { }
static void AVOutputFormat_mark(void *v) {}
static void AVOutputFormat_free(void *v) { }

static void AVCodecContext_mark(AVCodecContext v) {}
static void AVCodecContext_free(AVCodecContext v) {
	// avcodec_close(v); Segfault. I would have assumed this would work.
}

static VALUE AVCodecContext_codec_type(VALUE self)
{
  AVCodecContext *ptr;
  Data_Get_Struct(self, AVCodecContext, ptr);
	VALUE result = INT2NUM(ptr->codec_type);
	return result;
}

static VALUE AVCodecContext_codec_id(VALUE self)
{
  AVCodecContext *ptr;
  Data_Get_Struct(self, AVCodecContext, ptr);
	VALUE result = INT2NUM(ptr->codec_id);
	return result;
}

static VALUE AVCodecContext_width(VALUE self)
{
  AVCodecContext *ptr;
  Data_Get_Struct(self, AVCodecContext, ptr);
	VALUE result = INT2NUM(ptr->width);
	return result;
}

static VALUE AVCodecContext_height(VALUE self)
{
  AVCodecContext *ptr;
  Data_Get_Struct(self, AVCodecContext, ptr);
	VALUE result = INT2NUM(ptr->height);
	return result;
}

static VALUE AVCodecContext_channels(VALUE self)
{
  AVCodecContext *ptr;
  Data_Get_Struct(self, AVCodecContext, ptr);
	VALUE result = INT2NUM(ptr->channels);
	return result;
}

static VALUE AVCodecContext_sample_rate(VALUE self)
{
  AVCodecContext *ptr;
  Data_Get_Struct(self, AVCodecContext, ptr);
	VALUE result = INT2NUM(ptr->sample_rate);
	return result;
}

static VALUE AVCodec_long_name(VALUE self)
{
	char* tm;
  AVCodec *ptr;
  Data_Get_Struct(self, AVCodec, ptr);
	tm = ptr->long_name;
	return rb_str_new2(tm);
}

static VALUE AVCodec_name(VALUE self)
{
	char* tm;
  AVCodec *ptr;
  Data_Get_Struct(self, AVCodec, ptr);
	tm = ptr->name;
	return rb_str_new2(tm);
}

static VALUE AVCodec_codec_type(VALUE self)
{
	char* tm;
  AVCodec *ptr;
  Data_Get_Struct(self, AVCodec, ptr);
	return INT2NUM(ptr->type);
}

static VALUE AVCodecContext_codec_long_name(VALUE self)
{
	char* tm;
  AVCodecContext *ptr;
  Data_Get_Struct(self, AVCodecContext, ptr);
	if (ptr->codec == NULL)
			return Qnil;
	tm = ptr->codec->long_name;
	return rb_str_new2(tm);
}

static VALUE
AVCodecContext_codec_name(VALUE self)
{
	char* tm;
  AVCodecContext *ptr;
  Data_Get_Struct(self, AVCodecContext, ptr);
	if (ptr->codec == NULL)
			return Qnil;
	tm = ptr->codec->name;
	return rb_str_new2(tm);
}

static VALUE supported_avcodecs()
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

static VALUE supported_video_codecs()
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

static VALUE supported_audio_codecs()
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

static VALUE supported_avformats()
{
	VALUE result = rb_hash_new();
	AVOutputFormat *pNext = NULL;
	pNext = av_oformat_next(pNext);
	while (pNext != NULL) {
		VALUE sNextFormatName     = rb_str_new2(pNext->name);
		//VALUE sNextLongName = rb_str_new2(pNext->long_name);
		VALUE format = Data_Wrap_Struct(cFFMpegAVOutputFormat, AVOutputFormat_mark, AVOutputFormat_free, pNext);
		rb_obj_call_init(result, 0, 0);
		rb_hash_aset(result, sNextFormatName, format);
		pNext = av_codec_next(pNext);
	}
	return result;
}


static void AVFormatContext_mark(void *v) {}
static void AVFormatContext_free(void *v) { 
		// TODO Figure out how to avoid double-closing.
		//AVFormatContext* f = (AVFormatContext*)v;
		//if (f->filename != NULL)
			//av_close_input_file(v); 
}
 
static VALUE AVFormatContext_dump_format(VALUE self) {
  AVFormatContext *ptr;
  Data_Get_Struct(self, AVFormatContext, ptr);
	dump_format(ptr, 0, ptr->filename, 0);
	return Qnil;
}
static VALUE AVFormatContext_duration(VALUE self) {
  AVFormatContext *ptr;
  Data_Get_Struct(self, AVFormatContext, ptr);
	//ptr->filename = NULL;
	return INT2NUM(ptr->duration);
}
static VALUE AVFormatContext_close_file(VALUE self) {
  AVFormatContext *ptr;
  Data_Get_Struct(self, AVFormatContext, ptr);
	av_close_input_file(ptr);
	//ptr->filename = NULL;
	return Qnil;
}
static VALUE AVFormatContext_new(VALUE klaas, VALUE ruby_filename) {
		Check_Type(ruby_filename, T_STRING);
		char* filename = StringValuePtr(ruby_filename);

		AVFormatContext *pFormatCtx;
		// Open video file
		if(av_open_input_file(&pFormatCtx, filename, NULL, 0, NULL)!=0) {
				rb_raise(rb_eRuntimeError, "Could not open video file %s", filename);
			return NULL;
		}
	
		// Retrieve stream information
		if(av_find_stream_info(pFormatCtx)<0) {
			rb_raise(rb_eRuntimeError, "Could find the stream information in file %s", filename);
			return NULL; // Couldn't find stream information
		}

		VALUE result = Data_Wrap_Struct(klaas, AVFormatContext_mark, AVFormatContext_free, pFormatCtx);
		rb_obj_call_init(result, 0, 0);
		return result;
}

static VALUE codec_type_map()
{
	VALUE result = rb_hash_new();
	rb_hash_aset(result, INT2NUM(CODEC_TYPE_VIDEO), rb_str_new2("Video"));
	rb_hash_aset(result, INT2NUM(CODEC_TYPE_AUDIO), rb_str_new2("Audio"));
	rb_hash_aset(result, INT2NUM(CODEC_TYPE_DATA), rb_str_new2("Data"));
	rb_hash_aset(result, INT2NUM(CODEC_TYPE_SUBTITLE), rb_str_new2("Subtitle"));
	rb_hash_aset(result, INT2NUM(CODEC_TYPE_ATTACHMENT), rb_str_new2("Attachment"));
	return result;
}

/* Return all the AVCodecContext entries for all streams in the file */
static VALUE AVFormatContext_codec_contexts(VALUE self) {
  AVFormatContext *ptr;
  Data_Get_Struct(self, AVFormatContext, ptr);
	VALUE result = rb_ary_new();
	int i, videoStream;
	videoStream=-1;
	for(i=0; i<ptr->nb_streams; i++) {
		// Get a pointer to the codec context for the video stream
		AVCodecContext  *pCodecCtx = ptr->streams[i]->codec;
		AVCodec         *pCodec;
		pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
		pCodecCtx->codec = pCodec;

		// Turn it into something ruby can understand
		VALUE rCodec  = Data_Wrap_Struct(cFFMpegAVCodecContext, AVCodecContext_mark, AVCodecContext_free, pCodecCtx);
		rb_obj_call_init(rCodec,0,0);
		rb_ary_push(result, rCodec);
	}
	//if(videoStream==-1) {
			//rb_raise(rb_eRuntimeError, "No video streams here!");
			//return Qnil;
	//}
	return result;
}

static VALUE AVFormatContext_title(VALUE self) {
  AVFormatContext *ptr;
  Data_Get_Struct(self, AVFormatContext, ptr);
	return rb_str_new2(ptr->title);
}

static VALUE AVFormatContext_author(VALUE self) {
  AVFormatContext *ptr;
  Data_Get_Struct(self, AVFormatContext, ptr);
	return rb_str_new2(ptr->author);
}

static VALUE AVFormatContext_copyright(VALUE self) {
  AVFormatContext *ptr;
  Data_Get_Struct(self, AVFormatContext, ptr);
	return rb_str_new2(ptr->copyright);
}

static VALUE AVFormatContext_bit_rate(VALUE self) {
  AVFormatContext *ptr;
  Data_Get_Struct(self, AVFormatContext, ptr);
	return INT2NUM(ptr->bit_rate);
}

static VALUE AVFormatContext_album(VALUE self) {
  AVFormatContext *ptr;
  Data_Get_Struct(self, AVFormatContext, ptr);
	return rb_str_new2(ptr->album);
}


/** *** Ruby Interface *** **/
void
Init_ruby_ffmpeg()
{
		av_register_all();
		mFFMpeg = rb_define_module("FFMpeg");
		//cFFMpegAVFormatContext = rb_define_class_under(mFFMpeg, "AVFormatContext", rb_cObject);
		cFFMpegAVCodec = rb_define_class_under(mFFMpeg, "AVCodec", rb_cObject);
		rb_define_method(cFFMpegAVCodec, "name", AVCodec_name, 0);
		rb_define_method(cFFMpegAVCodec, "long_name", AVCodec_long_name, 0);
		rb_define_method(cFFMpegAVCodec, "codec_type", AVCodec_codec_type, 0);
		rb_define_singleton_method(cFFMpegAVCodec, "supported_avcodecs", supported_avcodecs, 0);
		rb_define_singleton_method(cFFMpegAVCodec, "supported_video_codecs", supported_video_codecs, 0);
		rb_define_singleton_method(cFFMpegAVCodec, "supported_audio_codecs", supported_audio_codecs, 0);

		cFFMpegAVCodecContext = rb_define_class_under(mFFMpeg, "AVCodecContext", rb_cObject);
		rb_define_method(cFFMpegAVCodecContext, "long_name", AVCodecContext_codec_long_name, 0);
		rb_define_method(cFFMpegAVCodecContext, "name", AVCodecContext_codec_name, 0);
		rb_define_method(cFFMpegAVCodecContext, "codec_type", AVCodecContext_codec_type, 0);
		rb_define_method(cFFMpegAVCodecContext, "codec_id", AVCodecContext_codec_id, 0);
		rb_define_method(cFFMpegAVCodecContext, "width", AVCodecContext_width, 0);
		rb_define_method(cFFMpegAVCodecContext, "height", AVCodecContext_height, 0);
		rb_define_method(cFFMpegAVCodecContext, "channels", AVCodecContext_channels, 0);
		rb_define_method(cFFMpegAVCodecContext, "sample_rate", AVCodecContext_sample_rate, 0);

		cFFMpegAVFormatContext = rb_define_class_under(mFFMpeg, "AVFormatContext", rb_cObject);
		rb_define_singleton_method(cFFMpegAVFormatContext, "new", AVFormatContext_new, 1);
		rb_define_method(cFFMpegAVFormatContext, "close_file", AVFormatContext_close_file, 0);
		rb_define_method(cFFMpegAVFormatContext, "duration", AVFormatContext_duration, 0);
		rb_define_method(cFFMpegAVFormatContext, "title", AVFormatContext_title, 0);
		rb_define_method(cFFMpegAVFormatContext, "copyright", AVFormatContext_copyright, 0);
		rb_define_method(cFFMpegAVFormatContext, "author", AVFormatContext_author, 0);
		rb_define_method(cFFMpegAVFormatContext, "album", AVFormatContext_album, 0);
		rb_define_method(cFFMpegAVFormatContext, "bit_rate", AVFormatContext_bit_rate, 0);
		rb_define_method(cFFMpegAVFormatContext, "codec_contexts", AVFormatContext_codec_contexts, 0);
		rb_define_method(cFFMpegAVFormatContext, "dump_format", AVFormatContext_dump_format, 0);

		cFFMpegAVOutputFormat = rb_define_class_under(mFFMpeg, "AVOutputFormat", rb_cObject);
		rb_define_singleton_method(cFFMpegAVOutputFormat, "supported_avformats", supported_avformats, 0);

		// Codec Types, for AVCodecContext
		rb_define_const(mFFMpeg, "CODEC_TYPE_VIDEO", INT2NUM(CODEC_TYPE_VIDEO));
		rb_define_const(mFFMpeg, "CODEC_TYPE_AUDIO", INT2NUM(CODEC_TYPE_AUDIO));
		rb_define_const(mFFMpeg, "CODEC_TYPE_DATA", INT2NUM(CODEC_TYPE_DATA));
		rb_define_const(mFFMpeg, "CODEC_TYPE_SUBTITLE", INT2NUM(CODEC_TYPE_SUBTITLE));
		rb_define_const(mFFMpeg, "CODEC_TYPE_ATTACHMENT", INT2NUM(CODEC_TYPE_ATTACHMENT));
		rb_define_const(mFFMpeg, "MAP_CODEC_TYPES", codec_type_map());

}

