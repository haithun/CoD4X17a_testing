/*
===========================================================================
    Copyright (C) 1999-2005 Id Software, Inc.

    This file is part of CoD4X17a-Server source code.

    CoD4X17a-Server source code is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as
    published by the Free Software Foundation, either version 3 of the
    License, or (at your option) any later version.

    CoD4X17a-Server source code is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>
===========================================================================
*/



#ifndef __HUFFMAN_H__
#define __HUFFMAN_H__

#include "msg.h"

#define NYT HMAX                    /* NYT = Not Yet Transmitted */
#define INTERNAL_NODE ( HMAX + 1 )

typedef struct nodetype {
	struct  nodetype *left, *right, *parent; /* tree structure */
//	struct  nodetype *next, *prev; /* doubly-linked list */
//	struct  nodetype **head; /* highest ranked node in block */
	int weight;
	int symbol; //0x10
	struct  nodetype *next, *prev; /* doubly-linked list */
	struct  nodetype **head; /* highest ranked node in block */

} node_t;

#define HMAX 256 /* Maximum symbol */

typedef struct {
	int blocNode;
	int blocPtrs;

	node_t*     tree;
	node_t*     lhead;
	node_t*     ltail;
	node_t*     loc[HMAX + 1];
	node_t**    freelist;

	node_t nodeList[768];
	node_t*     nodePtrs[768];
} huff_t;

typedef struct {
	huff_t compressor;
	huff_t decompressor;
} huffman_t;

void    Huff_Compress( msg_t *buf, int offset );
void    Huff_Decompress( msg_t *buf, int offset );
void    Huff_Init( huffman_t *huff );
void    Huff_addRef( huff_t* huff, byte ch );
int     Huff_Receive( node_t *node, int *ch, byte *fin );
void    Huff_transmit( huff_t *huff, int ch, byte *fout );
//void    Huff_offsetReceive( node_t *node, int *ch, byte *fin, int *offset );
void    Huff_offsetTransmit( huff_t *huff, int ch, byte *fout, int *offset );
void    Huff_putBit( int bit, byte *fout, int *offset );
int     Huff_getBit( byte *fout, int *offset );
int MSG_ReadBitsCompress(const byte* input, byte* outputBuf, int readsize);
void MSG_initHuffman();

//extern huffman_t clientHuffTables;

#endif