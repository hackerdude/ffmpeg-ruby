#include <libavformat/avformat.h>
#include <ruby.h>

VALUE cFFMpegAVStream;

void AVStream_mark(void *v);
void AVStream_free(void *v);
VALUE AVStream_canonical_codec_name(VALUE);
VALUE AVStream_codec(VALUE);


// forward declaring some stuff from codec module
void AVCodec_mark(void *v);
void AVCodec_free(void *v);
extern VALUE cFFMpegAVCodec;
VALUE avcodec_canonical_name(AVCodecContext *enc);
