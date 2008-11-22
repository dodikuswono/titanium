require 'fileutils'


def get_home
  if ENV['HOME']
    ENV['HOME']
  elsif ENV['USERPROFILE']
    ENV['USERPROFILE']
  elsif ENV['HOMEDRIVE'] and ENV['HOMEPATH']
    "#{ENV['HOMEDRIVE']}:#{ENV['HOMEPATH']}"
  else
    begin
      File.expand_path '~'
    rescue
      File.expand_path '/'
    end
  end
end

def home_dir
  File.expand_path get_home + '/.appcelerator'
end

js = File.join(home_dir,"releases/titanium/titanium_osx/0.1.1/titanium.js")

dir = File.join(ENV['TARGET_BUILD_DIR'],ENV['UNLOCALIZED_RESOURCES_FOLDER_PATH'],'titanium')

FileUtils.mkdir_p dir unless File.exists? dir
FileUtils.cp js,dir


exit 0