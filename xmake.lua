set_languages("cxx20")  -- 指定c++版本


target("XyA")
    set_kind("binary")  -- 生成二进制文件
    add_files("main.cpp")
    add_includedirs("Source")  -- 设置include目录

    add_cxflags("-w")  -- 隐藏警告