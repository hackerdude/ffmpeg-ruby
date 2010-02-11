require 'mkmf'
#require 'rubygems'
#require 'ruby-debug'
#Debugger.start

def boom(v)
  puts(v)
  exit 1
end

#debugger
if RUBY_PLATFORM == "universal-darwin10.0"
  # TODO Set the archflags to -arch x86_64 ONLY if it's a 64-bit snow leopard machine.
	#ENV['ARCHFLAGS'] = "-arch i386 -arch x86_64"
	$CFLAGS.sub!("-arch x86_64", "")
end
unless ( have_header("libavformat/avformat.h") || 
         find_header("libavformat/avformat.h", "/opt/local/include", "/usr/local/include", "/usr/include") ) and 
       ( have_library("avformat", "av_register_all") or 
		     find_library("avformat", "av_register_all", "/opt/local/lib", "/usr/local/lib", "/usr/lib") )
	boom(<<EOL)
You need to install or tell me where to find ffmpeg. Check the mkmf.log file to see what erred out.
EOL
end

create_makefile("ffmpeg_ruby")
