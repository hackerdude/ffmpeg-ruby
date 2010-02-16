VALUE cFFMpegAVCodecContext;
VALUE cFFMpegAVCodec;

void Init_ffmpeg_ruby_avcodec(VALUE module)

void AVCodec_mark(void *v);
void AVCodec_free(void *v);

void AVCodecContext_mark(AVCodecContext v);
void AVCodecContext_free(AVCodecContext v);

VALUE AVCodecContext_codec_type(VALUE self);
VALUE codec_type_map();

VALUE AVCodecContext_codec_id(VALUE self);
VALUE AVCodecContext_width(VALUE self);
VALUE AVCodecContext_height(VALUE self);
VALUE AVCodecContext_channels(VALUE self);
VALUE AVCodecContext_sample_rate(VALUE self);
VALUE AVCodec_long_name(VALUE self);
VALUE AVCodec_name(VALUE self);
VALUE AVCodec_codec_type(VALUE self);
VALUE AVCodecContext_codec_long_name(VALUE self);
VALUE AVCodecContext_codec_name(VALUE self);
VALUE supported_avcodecs();
VALUE supported_video_codecs();
VALUE supported_audio_codecs();
VALUE avcodec_canonical_name(AVCodecContext *enc);

