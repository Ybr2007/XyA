#define Debug_Display_Tokens
#define Debug_Write_AST_To_Json_File
#define Debug_Display_Instructions
#define Debug_Display_Literals
#define Debug_Display_Memory_Leaks

#ifdef Debug_Display_Literals
#define Debug_Display_Object
#endif


#ifdef Debug_Display_Memory_Leaks
#define Debug_Display_Object
#endif