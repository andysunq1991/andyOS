/*
 * =====================================================================================
 *       Filename:  elf.c
 *    Description:  ELF 格式解析相关函数
 *        Version:  1.0
 *        Created:  2018年03月05日
 *       Revision:  none
 *       Compiler:  gcc
 *         Author: andysun
 * =====================================================================================
 */

#include "common.h"
#include "string.h"
#include "elf.h"
#include "vmm.h"

//从mutiboot_t结构体中获取elf信息
elf_t elf_from_multiboot(multiboot_t *mb)
{
	elf_t elf;
	int i = 0;
	elf_section_header_t *sh = (elf_section_header_t *)mb->addr;
	uint32_t shstrtab = sh[mb->shndx].addr;

	for(i=0; i<mb->num; i++)
	{
		const char *name = (const char *)(shstrtab + sh[i].name) + PAGE_OFFSET;
		//在GRUB提供的multiboot信息中寻找内核elf格式所提取的字符串标和符号表
		if(strcmp(name,".strtab")==0)
		{
			elf.strtab = (const char *)sh[i].addr + PAGE_OFFSET;;
			elf.strtabsz = sh[i].size;
		}

		if(strcmp(name,".symtab")==0)
		{
			elf.symtab = (elf_symbol_t *)sh[i].addr + PAGE_OFFSET;;
			elf.symtabsz = sh[i].size;
		}
	}
	return elf;
}

//查看的符号信息elf
const char *elf_lookup_symbol(uint32_t addr, elf_t *elf)
{
	int i = 0;
	for(i=0; i<(elf->symtabsz/sizeof(elf_symbol_t)); i++)
	{
		if(ELF32_ST_TYPE(elf->symtab[i].info)!=0x02)
		{
			continue;
		}

		//通过函数调用地址查到函数的名字
		if((addr>=elf->symtab[i].value) && (addr<=(elf->symtab[i].value+elf->symtab[i].size)))
		{
			return (const char*)((uint32_t)elf->strtab + elf->symtab[i].name);
		}
	}
	return NULL;
}