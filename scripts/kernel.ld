/*
 *      kernel.ld -- 针对 kernel 格式所写的链接脚本
 *
 *           作者是 JamesM 先生，感谢他的慷慨和付出
 *
 *          Original file taken from Bran's Kernel Development 
 *          tutorials: http://www.osdever.net/bkerndev/index.php. 
 *
 *  首先，我们声明了内核程序的入口地址是符号 "start"
 *      这个脚本告诉 ld 程序 如何构造我们的内核映像文件。
 *      然后，我们声明了第一个段 .text 段（代码段）以及它的起始地址 0x100000（1MB）。
 *      接着是 已初始化数据段 .data 和 未初始化数据段 .bss 以及它们采用的4096的页对齐方式。
 *      Linux GCC 增加了额外的数据段 .rodata，这是一个只读的已初始化数据段，放置常量什么的。
 *      简单起见，我们把和 .data 段放在一起好了。
 *
 *      This script tells LD how to set up our kernel image. 
 *      Firstly it tells LD that the start location of our binary should be the symbol 'start'. 
 *      It then tells LD that the .text section (that's where all your code goes) should be first,
 *      and should start at 0x100000 (1MB). 
 *      The .data (initialised static data) and the .bss (uninitialised static data) should be next, 
 *      and each should be page-aligned (ALIGN(4096)).
 *      Linux GCC also adds in an extra data section: .rodata. 
 *      This is for read-only initialised data, such as constants. 
 *      For simplicity we simply bundle this in with the .data section.
 */

ENTRY(start)    /*将符号start的值设置成入口地址,入口地址(entry point): 进程执行的第一条用户空间的指令在进程地址空间的地址)*/
SECTIONS
{
    PROVIDE( kern_start = 0xc0100000);  /*该关键字用于定义这类符号：在目标文件内被引用，但没有在任何目标文件内被定义的符号*/

    . = 0x100000;  /*把定位器符号置为0×100000 (若不指定, 则该符号的初始值为0).*/

    .init.text :
    {
        *(.init.text)
        . = ALIGN(4096);
    }
    /*将所有(*符号代表任意输入文件)输入文件的.text section合并成一个.text section, 该section的地址由定位器符号的值指定, 即0×10000.*/

    .init.data :
    {
        *(.init.data)
        . = ALIGN(4096);
    }

    . += 0xc0000000;

    .text : AT(ADDR(.text) - 0xc0000000)
    {
        *(.text)
        . = ALIGN(4096);
    }
    /*输出section的LMA ：默认情况下，LMA等于VMA，但可以通过关键字AT()指定LMA。
    用关键字AT()指定，括号内包含表达式，表达式的值用于设置LMA。如果不用AT()关键字，那么可用AT>LMA_REGION表达式设置指定该section加载地址的范围。这个属性主要用于构件ROM境象。*/

    .data : AT(ADDR(.data) - 0xc0000000)
    {
        *(.data)
        . = ALIGN(4096);
    }

    .bss : AT(ADDR(.bss) - 0xc0000000)
    {
        *(.bss)
        . = ALIGN(4096);
    }
    .stab : AT(ADDR(.stab) - 0xc0000000)
    {
        *(.stab)
        . = ALIGN(4096);
    }
    .stabstr : AT(ADDR(.stabstr) - 0xc0000000)
    {
        *(.stabstr)
        . = ALIGN(4096);
    }
    PROVIDE(kern_end = . );
    
    /DISCARD/ : { *(.comment) *(.eh_frame) }
}

