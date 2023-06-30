# 命名规范

## snake_case

函数、变量

对于private成员, 以双下划线("__")开头

## 大驼峰

文件夹、文件、类、结构体、命名空间、枚举类、枚举值

## 特殊

对于将class/struct的数据转化为std::string, 便于程序员阅读和debug的方法, 统一定义为**std::string ClassName::to_string() const**

# 注释规范

对于一行的简短注释，使用 `//`

对于与下方较多内容有关、较为有概括性的一行的简短说明，使用 `/**/`

对于在被注释内容之前的多行注释，使用以下格式：

```cpp
/*
* 概述（简短注释）
详细内容
! 警告 / 特殊注意事项
栏目名：
	栏目元素1：...
	栏目元素2：...
*/
```

示例：

`Source/Compiler/TokenAnalyzer.hpp` 文件中的 `TokenAnalyzer::analyze_source` 方法，注释如下：

```cpp
/* 
* 将源码分析为Token序列
! 注意: 返回值类型为std::vecotr<Token*>*, 在Token和Token Vector使用完毕后均需要在外部delete
参数：
	(std::string_view) source: 源代码
返回值:
	std::vector<Token*>* 经词法分析得到的Token序列
*/
std::vector<Token*>* analyze_source(std::string_view source);
```

对函数的参数和返回值注释，类似上述格式

对于声明和定义分析的函数，注释在声明处

待办事项使用 `TODO: ` 格式开头

# 内存管理

## new / delete

在new处用注释指出在何处delete
