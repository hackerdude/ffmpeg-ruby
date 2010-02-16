#include "ruby.h"
#include <libavformat/avformat.h>
#include "ffmpeg_avformat.h"

void AVFormatContext_mark(void *v) {}

void AVFormatContext_free(void *v) { 
		// TODO Figure out how to avoid double-closing.
		//AVFormatContext* f = (AVFormatContext*)v;
		//if (f->filename != NULL)
			//av_close_input_file(v); 
}

void AVOutputFormat_mark(void *v){}
void AVOutputFormat_free(void *v){}

VALUE supported_avformats()
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
 

VALUE AVFormatContext_dump_format(VALUE self) {
  AVFormatContext *ptr;
  Data_Get_Struct(self, AVFormatContext, ptr);
	dump_format(ptr, 0, ptr->filename, 0);
	return Qnil;
}
VALUE AVFormatContext_duration(VALUE self) {
  AVFormatContext *ptr;
  Data_Get_Struct(self, AVFormatContext, ptr);
	//ptr->filename = NULL;
	return INT2NUM(ptr->duration);
}
VALUE AVFormatContext_close_file(VALUE self) {
  AVFormatContext *ptr;
  Data_Get_Struct(self, AVFormatContext, ptr);
	av_close_input_file(ptr);
	//ptr->filename = NULL;
	return Qnil;
}
VALUE AVFormatContext_new(VALUE klaas, VALUE ruby_filename) {
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

VALUE AVFormatContext_streams(VALUE self) {
  AVFormatContext *ptr;
  Data_Get_Struct(self, AVFormatContext, ptr);
	VALUE result = rb_ary_new();
	int i;
	for(i=0; i<ptr->nb_streams; i++) {
		// Get a pointer to the codec context for the video stream
		AVStream  *pStream = ptr->streams[i];
		VALUE rStream  = Data_Wrap_Struct(cFFMpegAVStream, AVStream_mark, AVStream_free, pStream);
		rb_obj_call_init(rStream,0,0);
		rb_ary_push(result, rStream);
	}
	return result;
}

/* Return all the AVCodecContext entries for all streams in the file */
VALUE AVFormatContext_codec_contexts(VALUE self) {
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

VALUE AVFormatContext_title(VALUE self) {
  AVFormatContext *ptr;
  Data_Get_Struct(self, AVFormatContext, ptr);
	return rb_str_new2(ptr->title);
}

VALUE AVFormatContext_author(VALUE self) {
  AVFormatContext *ptr;
  Data_Get_Struct(self, AVFormatContext, ptr);
	return rb_str_new2(ptr->author);
}

VALUE AVFormatContext_copyright(VALUE self) {
  AVFormatContext *ptr;
  Data_Get_Struct(self, AVFormatContext, ptr);
	return rb_str_new2(ptr->copyright);
}

VALUE AVFormatContext_bit_rate(VALUE self) {
  AVFormatContext *ptr;
  Data_Get_Struct(self, AVFormatContext, ptr);
	return INT2NUM(ptr->bit_rate);
}

VALUE AVFormatContext_album(VALUE self) {
  AVFormatContext *ptr;
  Data_Get_Struct(self, AVFormatContext, ptr);
	return rb_str_new2(ptr->album);
}


void Init_ffmpeg_ruby_avformat(VALUE module)
{

		cFFMpegAVFormatContext = rb_define_class_under(module, "AVFormatContext", rb_cObject);
		rb_define_singleton_method(cFFMpegAVFormatContext, "new", AVFormatContext_new, 1);
		rb_define_method(cFFMpegAVFormatContext, "close_file", AVFormatContext_close_file, 0);
		rb_define_method(cFFMpegAVFormatContext, "streams", AVFormatContext_streams, 0);
		rb_define_method(cFFMpegAVFormatContext, "duration", AVFormatContext_duration, 0);
		rb_define_method(cFFMpegAVFormatContext, "title", AVFormatContext_title, 0);
		rb_define_method(cFFMpegAVFormatContext, "copyright", AVFormatContext_copyright, 0);
		rb_define_method(cFFMpegAVFormatContext, "author", AVFormatContext_author, 0);
		rb_define_method(cFFMpegAVFormatContext, "album", AVFormatContext_album, 0);
		rb_define_method(cFFMpegAVFormatContext, "bit_rate", AVFormatContext_bit_rate, 0);
		rb_define_method(cFFMpegAVFormatContext, "codec_contexts", AVFormatContext_codec_contexts, 0);
		rb_define_method(cFFMpegAVFormatContext, "dump_format", AVFormatContext_dump_format, 0);

		cFFMpegAVOutputFormat = rb_define_class_under(module, "AVOutputFormat", rb_cObject);
		rb_define_singleton_method(cFFMpegAVOutputFormat, "supported_avformats", supported_avformats, 0);
}
