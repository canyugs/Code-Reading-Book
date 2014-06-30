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

#ifndef FRAME_H
#define FRAME_H


namespace VCF{


#define FRAME_CLASSID			"ED88C0A2-26AB-11d4-B539-00C04F0196DA"
	
enum FrameStyleType {
	FST_SIZEABLE = 0,
	FST_NOBORDER,	
	FST_FIXED,
	FST_NOBORDER_FIXED,
	FST_TOOLBAR_BORDER,
	FST_TOOLBAR_BORDER_FIXED
};

static String FrameStyleTypeNames[] = { "FST_SIZEABLE", 
                                         "FST_NOBORDER", 
										 "FST_FIXED",
										 "FST_NOBORDER_FIXED",
										"FST_TOOLBAR_BORDER_SIZEABLE",
										"FST_TOOLBAR_BORDER_FIXED"		 };

/**
 * A frame is a component that houses other components, 
 *   plus has special adornments used for interacting with it. 
 *   These can include minimize, maximize, restore, system, and help buttons on the caption bar,
 * which is also part of the frame. 
 */
class APPKIT_API Frame : public VCF::Control, public AbstractContainer{
public:
	BEGIN_ABSTRACT_CLASSINFO(Frame, "VCF::Frame", "VCF::Control", FRAME_CLASSID )
	PROPERTY( String, "caption", Frame::getCaption, Frame::setCaption, PROP_STRING );
	PROPERTY( bool, "isTopmost", Frame::isFrameTopmost, Frame::setFrameTopmost, PROP_BOOL );
	LABELED_ENUM_PROPERTY( FrameStyleType, "frameStyle", Frame::getFrameStyle, Frame::setFrameStyle, 
						   FST_SIZEABLE, FST_TOOLBAR_BORDER_FIXED, 6, FrameStyleTypeNames);	
	END_CLASSINFO(Frame)
	virtual ~Frame(){};	
	
	Frame();

    virtual void setCaption( const String& caption );

    virtual String getCaption();

	virtual Rect* getClientBounds() = 0;
	virtual void  setClientBounds( Rect* bounds ) = 0;
	
	virtual void paint( GraphicsContext * context );

	virtual void show();

	virtual void hide();

	virtual void close() = 0;

	virtual String toString();

	virtual bool isFrameTopmost();

	virtual void setFrameTopmost( const bool& isTopmost );

	virtual FrameStyleType getFrameStyle(){
		return 	m_frameStyle;
	};

	virtual void setFrameStyle( const FrameStyleType& frameStyle ) = 0;
protected:	
	String m_caption;
	bool m_isTopmostFrame;
	FrameStyleType m_frameStyle;
private:    
    
};

}
#endif //FRAME_H