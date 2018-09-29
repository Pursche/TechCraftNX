#pragma once

//-----------------------------------------------------------------------------
// nxlink support
//-----------------------------------------------------------------------------
#include <switch.h>
#include <stdio.h>
#include <unistd.h>
#define TRACE(fmt,...) printf("%s: " fmt "\n", __PRETTY_FUNCTION__, ## __VA_ARGS__)

void initNxLink();
void deinitNxLink();