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

# merge titanium config into the mainline config for SDK
TITANIUM_CONFIG = YAML::load_file(File.join(TITANIUM_DIR,'config.yml'))
TITANIUM_TRANSPORT = S3Transport.new(DISTRO_BUCKET, TITANIUM_CONFIG)
TITANIUM_MANIFEST = TITANIUM_TRANSPORT.manifest
TITANIUM_CONFIG[:releases] = build_config(TITANIUM_CONFIG,TITANIUM_MANIFEST)
merge_config(TITANIUM_CONFIG)


namespace :titanium do

  namespace :runtime do
    require File.join(TITANIUM_DIR,'runtime','build.rb')
  end
  
  require File.join(TITANIUM_DIR,'project','build.rb')
end




