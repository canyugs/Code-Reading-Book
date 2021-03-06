/**
*Copyright (c) 2000-2001, Jim Crafton
*All rights reserved.
*Redistribution and use in source and binary forms, with or without
*modification, are permitted provided that the following conditions
*are met:
*	Redistributions of source code must retain the above copyright
*	notice, this list of conditions and the following disclaimer.
*
*	Redistributions in binary form must reproduce the above copyright
*	notice, this list of conditions and the following disclaimer in 
*	the documentation and/or other materials provided with the distribution.
*
*THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
*AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
*LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
*A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS
*OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
*EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
*PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
*PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
*LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
*NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
*SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*NB: This software will not save the world.
*/

/* Generated by Together */

#ifndef DEFAULTTREEMODEL_H
#define DEFAULTTREEMODEL_H




namespace VCF{

#define DEFAULTTREEMODEL_CLASSID		"3126B226-2819-11d4-B53A-00C04F0196DA"

class APPKIT_API DefaultTreeModel : public AbstractTreeModel  
{
public:

	BEGIN_CLASSINFO(DefaultTreeModel, "VCF::DefaultTreeModel", "VCF::AbstractTreeModel", DEFAULTTREEMODEL_CLASSID)	
	END_CLASSINFO(DefaultTreeModel)

	DefaultTreeModel();
	
	virtual ~DefaultTreeModel();

	void init();

	virtual void onItemPaint( ItemEvent* event );

    virtual void onItemChanged( ItemEvent* event );

    virtual void onItemSelected( ItemEvent* event );

	virtual void onItemAdded( ItemEvent* event );
	
	virtual void onItemDeleted( ItemEvent* event );
};

};
#endif //DEFAULTTREEMODEL_H



