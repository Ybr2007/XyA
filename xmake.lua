set_languages("cxx20")  -- 指定c++版本


target("XyA")
    set_kind("binary")  -- 生成二进制文件
    add_files("main.cpp", "Source/**.cpp")
    add_includedirs("Source", "Include")  -- 设置include目录

    add_cxflags("-w")  -- 隐藏警告
    set_optimize("fastest")