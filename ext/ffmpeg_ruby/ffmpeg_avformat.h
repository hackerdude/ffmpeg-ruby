#include <libavformat/avformat.h>
#include <ruby.h>

VALUE cFFMpegAVFormatContext;
VALUE cFFMpegAVOutputFormat;

void AVOutputFormat_mark(void *v);
void AVOutputFormat_free(void *v);

void AVFormatContext_mark(void *v);
void AVFormatContext_free(void *v);

VALUE AVFormatContext_dump_format(VALUE);
VALUE AVFormatContext_duration(VALUE);
VALUE AVFormatContext_close_file(VALUE);
VALUE AVFormatContext_new(VALUE, VALUE);
VALUE AVFormatContext_codec_contexts(VALUE);
VALUE AVFormatContext_streams(VALUE);
VALUE AVFormatContext_title(VALUE);
VALUE AVFormatContext_author(VALUE);
VALUE AVFormatContext_copyright(VALUE);
VALUE AVFormatContext_bit_rate(VALUE);
VALUE AVFormatContext_album(VALUE);
void Init_ffmpeg_ruby_avformat(VALUE module);

// Forward declaring some necessary stuff from other modules
void AVCodecContext_mark(void *v);
void AVCodecContext_free(void *v);
extern VALUE cFFMpegAVCodecContext;

void AVStream_mark(void *v);
void AVStream_free(void *v);
extern VALUE cFFMpegAVStream;
