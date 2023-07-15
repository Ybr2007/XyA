#pragma once
#include <vector>
#include <functional>
#include <LexicalAnalysis/Token.h>
#include <Utils/StringUtils.hpp>


namespace XyA
{
    /* 词法分析相关命名空间 */
    namespace LexicalAnalysis
    {
        /* 词法分析器 */
        class TokenAnalyzer
        {
        public:
            /*
            * 异常回调列表
            当在词法分析流程中发现了源码中的异常时, 调用异常回调列表中的所有回调

            回调函数类型: std::function<void(std::string_view, size_t)>
            回调函数参数:
                std::string_view: 异常信息
                size_t: 异常位置
             */
            std::vector<std::function<void(std::string_view, TokenPos)>> exception_callbacks;

            /*
            * 将源码分析为Token序列
            ! 注意: 返回值类型为std::vecotr<Token*>*, 在Token和Token Vector使用完毕后均需要在外部delete
            参数：
                (std::string_view) source: 源代码
            返回值:
                std::vector<Token*>* 经词法分析得到的Token序列
            */
            std::vector<Token*>* analyze_source(std::string_view source);

        private:
            std::string_view __analyzing_source;
            size_t __cur_char_ptr;
            size_t __cur_row;
            size_t __cur_column;
            bool __last_is_identifier;
            bool __finished;

            Token* __analyze_token();
            bool __try_match_char_token(Token* token, char chr, TokenType token_type);
            bool __try_match_string_token(Token* token, const std::string& str, TokenType token_type, bool keyword_mode = false);


            void __skip_space();
            void __skip_comment();

            bool __get_number_literal(std::string& result, bool& is_float);
            bool __get_string_literal(std::string& result);
            bool __get_identifier(std::string& result);

            bool __try_move_ptr(size_t step = 1);
            bool __at_end() const;
            bool __match(std::string str, bool keyword_mode = false) const;
            char __cur_char() const;
            char __next_char() const;
            TokenPos __cur_pos() const;

            void __throw_exception(std::string_view message, TokenPos pos) const;
        };
    }
}