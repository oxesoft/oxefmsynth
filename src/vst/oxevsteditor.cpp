/*
Oxe FM Synth: a software synthesizer
Copyright (C) 2004-2015  Daniel Moura <oxe@oxesoft.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "public.sdk/source/vst2.x/audioeffectx.h"
#include "oxevsteditor.h"
#include "ostoolkit.h"

//-----------------------------------------------------------------------------

COxeVstEditor::COxeVstEditor (AudioEffectX *effectx, CSynthesizer *synth)
    :
    AEffEditor(effectx),
    effectx(effectx)
{
    oxeeditor = new CEditor(synth);
    effect->setEditor(this);
    hostinterface = NULL;
    toolkit = NULL;
}

//-----------------------------------------------------------------------------

COxeVstEditor::~COxeVstEditor ()
{
    delete oxeeditor;
}

//-----------------------------------------------------------------------------

bool COxeVstEditor::getRect (ERect **erect)
{
    static ERect r = {0, 0, GUI_HEIGHT, GUI_WIDTH };
    *erect = &r;
    return true;
}

//-----------------------------------------------------------------------------

bool COxeVstEditor::open (void *ptr)
{
    // Remember the parent window
    systemWindow = ptr;

    hostinterface = new CVstHostInterface(effectx);
    toolkit = new COSToolkit(ptr, oxeeditor);
    oxeeditor->SetToolkit(toolkit);
    oxeeditor->SetHostInterface(hostinterface);
    this->toolkit->StartWindowProcesses();
    return true;
}

//-----------------------------------------------------------------------------

void COxeVstEditor::close ()
{
    oxeeditor->SetToolkit(NULL);
    oxeeditor->SetHostInterface(NULL);
    delete toolkit;
    toolkit = NULL;
    delete hostinterface;
    hostinterface = NULL;
}
