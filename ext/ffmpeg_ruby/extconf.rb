require 'mkmf'
dir_config("avutil")
dir_config("avformat")
dir_config("avcodec")

puts "ok" if have_library("avutil", "avutil_version", "libavutil/avutil.h")
puts "ok" if have_library("avformat", "av_register_all", "libavformat/avformat.h")
puts "ok" if have_library("avcodec", "avcodec_init", "libavcodec/avcodec.h")

create_makefile("ffmpeg_ruby")
