set_languages("cxx20")  -- 指定c++版本


target("XyA")
    set_kind("binary")  -- 生成二进制文件
    add_files("main.cpp", "Source/**.cpp")
    add_includedirs("Source", "Include")  -- 设置include目录

    if is_mode("debug") then
        set_optimize("none")
    end

    if is_mode("release") then 
        set_optimize("fastest")
    end

    before_build(
        function (target) 
            if is_mode("debug") then
                print("Debug Mode")
            end
            if is_mode("release") then 
                print("Release Mode")
            end
        end
    )