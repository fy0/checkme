
set_project("checkme")

-- the debug mode
if is_mode("debug") then
    
    -- enable the debug symbols
    set_symbols("debug")

    -- disable optimization
    set_optimize("none")
end

-- the release mode
if is_mode("release") then

    -- set the symbols visibility: hidden
    set_symbols("hidden")

    -- enable fastest optimization
    set_optimize("fastest")

    -- strip all symbols
    set_strip("all")
end

-- add target
target("checkme")

    -- set kind
    set_kind("binary")
    set_basename("checkme_$(plat)_$(arch)_$(mode)_$(kind)")


    -- add files
    add_files("src/*.c")
    add_files("src/*.cpp")
    add_headers("src/*.h")
    add_headers("deps/gl3w/**.h")
    
    add_files("deps/gl3w/**.c")

    -- add deps
    -- add_includedirs("deps/yoga-1.6.0")
    -- add_linkdirs("deps/yoga-1.6.0/build/Release")
    add_includedirs("deps/glfw-3.2.1.bin.WIN32/include")
    add_linkdirs("deps/glfw-3.2.1.bin.WIN32/lib-vc2015")
    add_includedirs("deps/zmq/include")
    add_linkdirs("deps/zmq/lib")

    add_includedirs("deps/gl3w")

    add_includedirs("deps/imgui-1.50")
    add_files("deps/imgui-1.50/*.cpp")

    add_links('kernel32', 'user32', 'gdi32', 'winspool', 'comdlg32', 'advapi32', 'shell32', 'ole32', 'oleaut32', 'uuid', 'OpenGL32', 'winmm', 'psapi', 'shlwapi', 'MSVCRT')
    add_links('glfw3', 'libzmq-v120-mt-4_0_4')


target_end()
