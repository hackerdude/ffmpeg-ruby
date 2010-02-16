require "#{File.dirname(__FILE__)}/../lib/ffmpeg-ruby"
require 'rubygems'
require 'ruby-debug'

puts "Here are the video codecs I support, of the #{FFMpeg::AVCodec.supported_avcodecs.length} codecs:"
FFMpeg::AVCodec.supported_video_codecs.each{|k,v|
  puts "  - #{k}=#{v.long_name} (#{v.name}) of type #{FFMpeg::MAP_CODEC_TYPES[v.codec_type]}"
}

FFMpeg::AVCodec.supported_audio_codecs.each{|k,v|
  puts "  - #{k}=#{v.long_name} (#{v.name}) of type #{FFMpeg::MAP_CODEC_TYPES[v.codec_type]}"
}
puts "Creating new AVFormatContext"
filename = '/Users/david/channels-reader/public/videos/Channels_Playlist_WEB.mov'
if ARGV.length > 0
  filename = ARGV[0]
end
f = FFMpeg::AVFormatContext.new(filename)
"Format Information:"
puts "  Duration: #{f.duration}"
puts "  Title: #{f.title}"
puts "  Copyright: #{f.copyright}"
puts "  Author: #{f.author}"
puts "  Album: #{f.album}"
puts "  Bit Rate: #{f.bit_rate}"

puts "\nStream information:"
f.streams.each{|s|
	puts "  Canonical Codec: #{s.canonical_codec_name}"
	puts "  Codec Long Name: #{s.codec_context.long_name}"
}

puts "\nCodec Context Information for #{filename}"
f.codec_contexts.each_with_index{|ctx,i|
	puts "Codec info for stream ##{i}"
	puts "  Type: #{ctx.codec_type} (#{FFMpeg::MAP_CODEC_TYPES[ctx.codec_type]})"
	puts "  Name: #{ctx.name}"
	puts "  Long Name: #{ctx.long_name}"
	puts "  Width: #{ctx.width}"
	puts "  Height: #{ctx.height}"
	puts "  Audio Channels: #{ctx.channels}"
	puts "  Audio Sample Rate: #{ctx.sample_rate}"
}



puts "-- Now dumping the format:"
f.dump_format

