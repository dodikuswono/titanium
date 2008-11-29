#
# Copyright 2008 Appcelerator, Inc.
# 
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
# 
#    http://www.apache.org/licenses/LICENSE-2.0
# 
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License. 
require 'erb'

desc 'build all titanium runtimes'
task :all => [:osx,:win32,:linux] do
end

def get_gears_make_command
	if is_mac?
		args = "BROWSER=SF MODE=opt"
		return "make #{args} || make #{args}"
	elsif is_win?
		if is_cygwin?
			return "cmd /C build.bat"
		else
			return "build.bat"
		end
	end
end

def build_gears
  gears_dir = File.join(ENV['GEARS_TITANIUM'] || File.join(TITANIUM_DIR,'gears-titanium'),'gears')
  die "Couldn't find gears directory" unless File.exists? gears_dir
  puts "Building Google Gears... "
  make_cmd = get_gears_make_command
  FileUtils.cd(gears_dir) do
    puts make_cmd
  	system make_cmd
  end
  File.expand_path(gears_dir)
end

def build_js

    puts "building Titanium Javascript ..."

    titanium_js =<<END
/*!
 * Copyright 2008 Appcelerator, Inc.
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *    http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
END

    # files in the order that we want them
    files = %w(jquery-1.2.6
               prolog
               gears
               dom
               window
               file
               desktop
               geolocation
               database
               http
               localserver
               timer
               workerpool
               tracker
               epilog)

    # make one big ass file
    files.each do |name|
      name.strip!
      js = File.read File.expand_path(File.join(TITANIUM_DIR,'runtime','src',"#{name}.js"))
      titanium_js << js
    end

    version = TITANIUM_CONFIG[:version]

  	template = ERB.new titanium_js
  	jf = template.result(binding)

    tmpfile = Tempfile.new 'titanium'
    tmpfile.puts jf
    tmpfile.close

    jar_path = File.join(TITANIUM_DIR, 'runtime', 'lib', 'yuicompressor-2.3.6.jar')
    tmpfile_path = tmpfile.path

    # compress our file
    if is_cygwin?
      jar_path = cygpath(jar_path)
      tmpfile_path = cygpath(tmpfile_path)
    end

    command = "java -jar \"#{jar_path}\" --type js --charset utf8 \"#{tmpfile_path}\" -o \"#{tmpfile_path}t\""

    puts command
    if is_cygwin?
    	system "cmd /C #{command}"
    else
    	system command
    end
    
    debug_js = File.open(File.join(STAGE_DIR,'titanium-debug.js'),'w')
    debug_js.puts jf
    debug_js.close
    
    js = File.open(File.join(STAGE_DIR,'titanium.js'),'w')
    js.puts File.read("#{tmpfile.path}t")
    js.close

    tmpfile.delete
    
    [js.path,debug_js.path]
end

def package_pieces(os)
  build_dir = "#{File.dirname(__FILE__)}/pieces/#{os.to_s}" 
  build_config = get_config(:titanium, os)
  zipfile = build_config[:output_filename]
  
  FileUtils.mkdir_p STAGE_DIR unless File.exists? STAGE_DIR
  FileUtils.rm_rf zipfile
  
  Zip::ZipFile.open(zipfile, Zip::ZipFile::CREATE) do |zipfile|
    dofiles(build_dir) do |f|
      filename = File.basename(f)
      next if File.basename(filename[0,1]) == '.'
      name = f.to_s.gsub(build_dir+'/','')
      zipfile.add(name,File.join(build_dir,f))
    end
    build_js.each do |js|
      zipfile.add("pieces/titanium/#{File.basename(js)}",js)
    end
    yield zipfile,build_config if block_given? 
    zipfile.get_output_stream('build.yml') do |f|
      config = Marshal::load(Marshal::dump(build_config))
      config.delete :output_filename
      f.puts config.to_yaml
    end
  end
end

desc 'build titanium osx runtime'
task :osx do
  osx_dir = File.expand_path(File.join(TITANIUM_DIR,'runtime','native','osx'))
  FileUtils.cd(osx_dir) do
    package_pieces :osx do |zipfile,config|
      out = File.join(STAGE_DIR,'osxbuild.log')
      puts "building the Mac OSX titanium runtime ..."
      puts "xcodebuild VERSION=#{config[:version]} >#{out}"
      system "xcodebuild VERSION=#{config[:version]} >#{out}"
      release = File.join(osx_dir,'build','Release','Titanium.app')
      dofiles(release) do |f|
        filename = File.basename(f)
        next if File.basename(filename[0,1]) == '.'
        name = f.to_s.gsub(release+'/','')
        zipfile.add("pieces/Titanium.app/#{name}",File.join(release,f))
      end
      puts "building Gears ..."
      gears_dir = build_gears
      gears_plugin = File.join(gears_dir, 'bin-opt', 'installers', 'Safari', 'Gears.plugin')
      dofiles(gears_plugin) do |f|
        filename = File.basename(f)
        next if File.basename(filename[0,1]) == '.'
        name = f.to_s.gsub(gears_plugin+'/','')
        zipfile.add("pieces/Titanium.app/Contents/Plug-ins/Gears.plugin/#{name}",File.join(gears_plugin,f))
      end
    end
  end
end

desc 'build titanium win32 runtime'
task :win32 do
  package_pieces :win32 do |zipfile,config|
    #TODO: package the rest of the stuff that needs to go into the package
    gears_dir = build_gears
    gears_plugin = File.join(gears_dir, 'bin-opt', 'win32-i386', 'npapi', 'gears_titanium.dll')
  	zipfile.add('pieces/gears.dll', gears_plugin)
  end
end

desc 'build titanium linux runtime'
task :linux do
  package_pieces :linux do |zipfile,config|
  end
end

