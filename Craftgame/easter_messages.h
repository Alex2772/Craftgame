#pragma once
#ifndef SERVER
const char* easter_messages[] = {
	"miraculous!",
	"also try Minecraft!",
	"also try Terraria!",
	"also try Dota 2!",
	"PAPI4",
	"322",
	"spone of sorcery",
	"LIDOV ONIK V YKMAM",
	"WE ARE IN DEEP HIT",
	"run, Forestry, run!",
	"Aaah, flash meat!",
	"get hover here!",
	"BUKSHTABU",
	"borrow the down train, JJ!",
	"wrote in C++!",
	"hello utub!"
};

struct easter_day
{
	unsigned char dayOfMonth;
	unsigned char month;
	const char* name;
};

const easter_day easter_days[] = {
	{1, 1, "Happy New year!"},
	{28, 12, "nk"}, // <----
	{27, 01, "Alex2772"},
	{14, 02, "<3"},
	{8, 03, "*flowers here*"},
	{1, 04, "HL 3 Edition"},
	{9, 05, "victory day"},
	{12, 06, "Россия идишн"}
};

#endif