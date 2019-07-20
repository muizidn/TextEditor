task :cmake do
  puts `cmake -H. -Bbuild`
end

task :make => :cmake do
  puts `make -C build`
end

task :run do
  exec './build/TextEditor'
end
