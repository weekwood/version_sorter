Gem::Specification.new do |gemspec|
  gemspec.name = "version_sorter"
  gemspec.summary = "Fast sorting of version strings"
  gemspec.description = "Fast sorting of version strings"
  gemspec.email = "chris@ozmm.org"
  gemspec.homepage = "http://github.com/defunkt/version_sorter"
  gemspec.authors = ["Chris Wanstrath", "K. Adam Christensen"]
  gemspec.version = '1.1.1'
  gemspec.require_paths = ["lib", "ext"]
  gemspec.files.include("ext")
  gemspec.extensions << 'ext/version_sorter/extconf.rb'
end
