//UndoRedoAbout.h
#include "ApplicationKit.h"
#include "UndoRedoAbout.h"
#include "Label.h"

using namespace VCF;


UndoRedoAbout::UndoRedoAbout()
{
	setWidth( 300 );
	setHeight( 100 );

	CommandButton* okBtn = new CommandButton();
	okBtn->setWidth( okBtn->getPreferredWidth() );
	okBtn->setHeight( okBtn->getPreferredHeight() );

	okBtn->setTop( getHeight() - (okBtn->getHeight()+35) );
	okBtn->setLeft( getWidth() - (okBtn->getWidth()+25) );
	add( okBtn );
	okBtn->setFocus( true );
	okBtn->setCommandType( BC_OK );
	okBtn->setCaption( "&OK" );

	Label* label = new Label();
	label->setBounds( &Rect(25, 10, 200, 45) );
	add( label );
	label->setCaption( "About UndoRedo." );

	setCaption( "About UndoRedo" );
}

UndoRedoAbout::~UndoRedoAbout()
{

}

