task :compile_mac do
	puts "Compiling on a mac"
	`cd ext/ffmpeg_ruby && env ARCHFLAGS="-arch x86_64" ruby extconf.rb --with-avformat-dir=/opt/local && make`
end

task :clean_mac => :clean do
	puts "Cleaning on a mac"
	`rm -rf ext/ffmpeg_ruby/conftest.dSYM`
end
